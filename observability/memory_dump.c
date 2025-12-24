#include <stdio.h>
#include <stdint.h>

#include "../allocator/my_malloc.h"
#include "memory_dump.h"

/* cache integration */
#include "../cache/cache.h"

/* use the global cache levels */
extern CacheLevel L1;
extern CacheLevel L2;
extern CacheLevel L3;

/*
 * Dump the current memory layout by walking the heap linearly.
 * We start from the beginning of the heap and move forward
 * using the size stored in each block's metadata.
 */
void dump_memory(void)
{
    if (heap == NULL)
    {
        printf("Heap not initialized.\n");
        return;
    }

    printf("\n========== MEMORY DUMP ==========\n");

    char *current = (char *)heap;

    /*
     * The heap grows in chunks of 2048 bytes (SBRK_SIZE).
     * For now, we assume a single chunk.
     */
    char *heap_end = current + 2048;

    /* ---- statistics ---- */
    size_t total_used = 0;
    size_t total_free = 0;
    int used_blocks = 0;
    int free_blocks = 0;

    while (current < heap_end)
    {
        /* reading block metadata = memory access */
        metadata_t *block = (metadata_t *)current;

        /* simulate cache access to this block header */
        cache_hierarchy_access(&L1, &L2, &L3, (uint64_t)current);

        uintptr_t start = (uintptr_t)current;
        uintptr_t end   = start + block->size - 1;

        if (block->in_use)
        {
            printf("[0x%08lx - 0x%08lx] USED (%zu bytes)\n",
                   (unsigned long)start,
                   (unsigned long)end,
                   (size_t)block->size);

            total_used += block->size;
            used_blocks++;
        }
        else
        {
            printf("[0x%08lx - 0x%08lx] FREE (%zu bytes)\n",
                   (unsigned long)start,
                   (unsigned long)end,
                   (size_t)block->size);

            total_free += block->size;
            free_blocks++;
        }

        /* Move to the next block */
        current += block->size;
    }

    printf("=================================\n");

    /* ---- summary ---- */
    printf("\n----------- SUMMARY -----------\n");
    printf("Total heap size : %zu bytes\n", total_used + total_free);
    printf("Used memory     : %zu bytes\n", total_used);
    printf("Free memory     : %zu bytes\n", total_free);
    printf("Used blocks     : %d\n", used_blocks);
    printf("Free blocks     : %d\n", free_blocks);
    printf("--------------------------------\n\n");
}
    