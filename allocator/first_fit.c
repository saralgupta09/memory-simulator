#include "first_fit.h"
#include "my_malloc.h"
#include "allocator_internal.h"
#include <stdint.h>

#define MIN_BLOCK_SIZE (sizeof(metadata_t) + 8)

/* ============================
   FIRST FIT malloc
   ============================ */
void* first_fit_malloc(size_t size)
{
    if (!heap_head || size == 0)
        return NULL;

    size_t total = sizeof(metadata_t) + size;
    metadata_t* curr = heap_head;

    while (curr)
    {
        if (!curr->in_use && curr->size >= total)
        {
            /* split if possible */
            if (curr->size >= total + MIN_BLOCK_SIZE)
            {
                metadata_t* split =
                    (metadata_t*)((char*)curr + total);

                split->size           = curr->size - total;
                split->requested_size = 0;
                split->in_use         = 0;
                split->next           = curr->next;
                split->prev           = curr;

                if (curr->next)
                    curr->next->prev = split;

                curr->next = split;
                curr->size = total;
            }

            curr->in_use         = 1;
            curr->requested_size = size;
            ERRNO = NO_ERROR;

            return (char*)curr + sizeof(metadata_t);
        }

        curr = curr->next;
    }

    ERRNO = OUT_OF_MEMORY;
    return NULL;
}

/* ============================
   FIRST FIT free
   (strategy-independent free)
   ============================ */
void first_fit_free(void* ptr)
{
    if (!ptr)
        return;

    metadata_t* block =
        (metadata_t*)((char*)ptr - sizeof(metadata_t));

    block->in_use = 0;
    block->requested_size = 0;

    /* merge with next */
    if (block->next && !block->next->in_use)
    {
        metadata_t* n = block->next;
        block->size += n->size;
        block->next = n->next;
        if (n->next)
            n->next->prev = block;
    }

    /* merge with prev */
    if (block->prev && !block->prev->in_use)
    {
        metadata_t* p = block->prev;
        p->size += block->size;
        p->next = block->next;
        if (block->next)
            block->next->prev = p;
    }

    ERRNO = NO_ERROR;
}
