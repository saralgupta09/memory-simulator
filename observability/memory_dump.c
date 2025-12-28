#include <stdio.h>
#include <stdint.h>
#include "../allocator/allocator_internal.h"
#include "memory_dump.h"
#include "../allocator/allocator.h"
#include "../allocator/my_malloc.h"



/* ============================
   OPTIONAL CACHE SUPPORT
   ============================ */
#ifdef ENABLE_CACHE
// Include the header so we know what CacheLevel is
#include "../cache/cache.h" 

extern CacheLevel L1, L2, L3;

static inline void maybe_cache_access(uint64_t addr)
{
    // FIX: Check 'sets', not 'lines'.
    // If 'sets' is NULL, the cache isn't initialized, so we skip.
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

        maybe_cache_access((uint64_t)current);

        uintptr_t start = (uintptr_t)current;
        uintptr_t end   = start + block->size - 1;

        // FIX: Cast block->size to (size_t) to silence the %zu warning
        printf(
            "[0x%08lx - 0x%08lx] %s (%zu bytes)\n",
            (unsigned long)start,
            (unsigned long)end,
            block->in_use ? "USED" : "FREE",
            (size_t)block->size
        );

        current += block->size;
    }
}


















// /* ============================
//    OPTIONAL CACHE SUPPORT
//    ============================ */
// #ifdef ENABLE_CACHE
// #include "../cache/cache.h"
// extern CacheLevel L1, L2, L3;

// static inline void maybe_cache_access(uint64_t addr)
// {
//     if (!L1.lines || !L2.lines || !L3.lines)
//         return;

//     cache_hierarchy_access(&L1, &L2, &L3, addr);
// }
// #endif

// void dump_memory(void)
// {
//     if (!heap || heap_size == 0)
//     {
//         printf("Heap not initialized.\n");
//         return;
//     }

//     printf("\n========== MEMORY DUMP ==========\n");

//     char *current  = (char *)heap;
//     char *heap_end = current + heap_size;

//     while (current < heap_end)
//     {
//         metadata_t *block = (metadata_t *)current;

//         if (block->size == 0)
//         {
//             printf("CORRUPTED BLOCK (size=0)\n");
//             break;
//         }

//         maybe_cache_access((uint64_t)current);

//         uintptr_t start = (uintptr_t)current;
//         uintptr_t end   = start + block->size - 1;

//         printf(
//             "[0x%08lx - 0x%08lx] %s (%zu bytes)\n",
//             (unsigned long)start,
//             (unsigned long)end,
//             block->in_use ? "USED" : "FREE",
//             block->size
//         );

//         current += block->size;
//     }
// }
