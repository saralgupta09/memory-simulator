#include "best_fit.h"
#include "my_malloc.h"
#include "allocator_internal.h"
#include <stdint.h>

#define MIN_BLOCK_SIZE (sizeof(metadata_t) + 8)

/* head of best-fit list */
static metadata_t* bf_head = NULL;

/* ============================
   INIT (called by allocator_init)
   ============================ */
void best_fit_init(void* heap_start, size_t heap_size)
{
    bf_head = (metadata_t*)heap_start;

    bf_head->size           = heap_size;
    bf_head->requested_size = 0;    /* free block */
    bf_head->in_use         = 0;
    bf_head->next           = NULL;
    bf_head->prev           = NULL;
}

/* ============================
   BEST FIT malloc
   ============================ */
void* best_fit_malloc(size_t size)
{
    if (!bf_head || size == 0)
        return NULL;

    size_t total = sizeof(metadata_t) + size;

    metadata_t* curr = bf_head;
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
        split->requested_size = 0;     /* free block */
        split->in_use         = 0;
        split->next           = best->next;
        split->prev           = best;

        if (best->next)
            best->next->prev = split;

        best->next = split;
        best->size = total;
    }

    best->in_use         = 1;
    best->requested_size = size;    /* STEP 2 */
    ERRNO = NO_ERROR;

    return (char*)best + sizeof(metadata_t);
}

/* ============================
   BEST FIT free
   ============================ */
void best_fit_free(void* ptr)
{
    if (!ptr)
        return;

    metadata_t* block =
        (metadata_t*)((char*)ptr - sizeof(metadata_t));

    block->in_use         = 0;
    block->requested_size = 0;    /* STEP 2 */

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
