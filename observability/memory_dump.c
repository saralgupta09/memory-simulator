#include <stdio.h>
#include <stdint.h>

#include "../allocator/my_malloc.h"
#include "memory_dump.h"

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
     * NOTE:
     * The heap grows in chunks of 2048 bytes (SBRK_SIZE).
     * For now, we assume a single chunk.
     * We will generalize this later if needed.
     */
    char *heap_end = current + 2048;

    while (current < heap_end)
    {
        metadata_t *block = (metadata_t *)current;

        uintptr_t start = (uintptr_t)current;
        uintptr_t end   = start + block->size - 1;

        if (block->in_use)
        {
            printf("[0x%08lx - 0x%08lx] USED (%zu bytes)\n",
                   (unsigned long)start,
                   (unsigned long)end,
                   (size_t)block->size);
        }
        else
        {
            printf("[0x%08lx - 0x%08lx] FREE (%zu bytes)\n",
                   (unsigned long)start,
                   (unsigned long)end,
                   (size_t)block->size);
        }

        /* Move to the next block */
        current += block->size;
    }

    printf("=================================\n\n");
}
