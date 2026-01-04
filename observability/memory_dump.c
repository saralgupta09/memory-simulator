#include <stdio.h>
#include <stdint.h>

#include "../allocator/allocator_internal.h"
#include "memory_dump.h"
#include "../allocator/allocator.h"
#include "../allocator/my_malloc.h"

#ifdef ENABLE_CACHE
#include "../cache/cache.h"
extern CacheLevel L1, L2, L3;

static inline void maybe_cache_access(uint64_t addr)
{
    /* Cache must be initialized */
    if (!L1.sets || !L2.sets || !L3.sets)
        return;

    cache_hierarchy_access(&L1, &L2, &L3, addr);
}
#else
static inline void maybe_cache_access(uint64_t addr)
{
    (void)addr;
}
#endif

void dump_memory(void)
{
    if (!heap || heap_size == 0)
    {
        printf("Heap not initialized.\n");
        return;
    }

    printf("\n========== MEMORY DUMP ==========\n");

    char *current  = (char *)heap;
    char *heap_end = current + heap_size;

    while (current < heap_end)
    {
        metadata_t *block = (metadata_t *)current;

        if (block->size == 0)
        {
            printf("CORRUPTED BLOCK (size=0)\n");
            break;
        }

        /* Simulate memory access */
        maybe_cache_access((uint64_t)current);

        uintptr_t start = (uintptr_t)current;
        uintptr_t end   = start + block->size - 1;

        printf("[0x%08lx - 0x%08lx] %s (%zu bytes)\n",
               (unsigned long)start,
               (unsigned long)end,
               block->in_use ? "USED" : "FREE",
               block->size);

        current += block->size;
    }

#ifdef ENABLE_CACHE
    printf("\n----- CACHE STATS -----\n");
    cache_print_stats("L1", &L1);
    cache_print_stats("L2", &L2);
    cache_print_stats("L3", &L3);
    printf("-----------------------\n");
#endif
}
