#include "best_fit.h"
#include "my_malloc.h"
#include "allocator_internal.h"
#include <stdint.h>
#include "first_fit.h"


#define MIN_BLOCK_SIZE (sizeof(metadata_t) + 8)

/* ============================
   BEST FIT malloc
   ============================ */
void* best_fit_malloc(size_t size)
{
    if (!heap_head || size == 0)
        return NULL;

    size_t total = sizeof(metadata_t) + size;

    metadata_t* curr = heap_head;
    metadata_t* best = NULL;

    /* find smallest suitable free block */
    while (curr)
    {
        if (!curr->in_use && curr->size >= total)
        {
            if (!best || curr->size < best->size)
                best = curr;
        }
        curr = curr->next;
    }

    if (!best)
    {
        ERRNO = OUT_OF_MEMORY;
        return NULL;
    }

    /* split if possible */
    if (best->size >= total + MIN_BLOCK_SIZE)
    {
        metadata_t* split =
            (metadata_t*)((char*)best + total);

        split->size           = best->size - total;
        split->requested_size = 0;
        split->in_use         = 0;
        split->next           = best->next;
        split->prev           = best;

        if (best->next)
            best->next->prev = split;

        best->next = split;
        best->size = total;
    }

    best->in_use         = 1;
    best->requested_size = size;
    ERRNO = NO_ERROR;

    return (char*)best + sizeof(metadata_t);
}

/* ============================
   BEST FIT free
   (delegated to shared free)
   ============================ */
void best_fit_free(void* ptr)
{
    first_fit_free(ptr);
}
