#include "worst_fit.h"
#include "my_malloc.h"
#include "allocator_internal.h"
#include <stdint.h>
#include "first_fit.h"


#define MIN_BLOCK_SIZE (sizeof(metadata_t) + 8)

/* ============================
   WORST FIT malloc
   ============================ */
void* worst_fit_malloc(size_t size)
{
    if (!heap_head || size == 0)
        return NULL;

    size_t total = sizeof(metadata_t) + size;

    metadata_t* curr  = heap_head;
    metadata_t* worst = NULL;

    /* find largest suitable free block */
    while (curr)
    {
        if (!curr->in_use && curr->size >= total)
        {
            if (!worst || curr->size > worst->size)
                worst = curr;
        }
        curr = curr->next;
    }

    if (!worst)
    {
        ERRNO = OUT_OF_MEMORY;
        return NULL;
    }

    /* split if possible */
    if (worst->size >= total + MIN_BLOCK_SIZE)
    {
        metadata_t* split =
            (metadata_t*)((char*)worst + total);

        split->size           = worst->size - total;
        split->requested_size = 0;
        split->in_use         = 0;
        split->next           = worst->next;
        split->prev           = worst;

        if (worst->next)
            worst->next->prev = split;

        worst->next = split;
        worst->size = total;
    }

    worst->in_use         = 1;
    worst->requested_size = size;
    ERRNO = NO_ERROR;

    return (char*)worst + sizeof(metadata_t);
}

/* ============================
   WORST FIT free
   (delegated to shared free)
   ============================ */
void worst_fit_free(void* ptr)
{
    /* free + coalescing is strategy-independent */
    first_fit_free(ptr);
}
