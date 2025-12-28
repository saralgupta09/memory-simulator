#include "worst_fit.h"
#include "my_malloc.h"
#include "allocator_internal.h"
#include <stdint.h>

#define MIN_BLOCK_SIZE (sizeof(metadata_t) + 8)

/* head of worst-fit list */
static metadata_t* wf_head = NULL;

/* ============================
   INIT (called by allocator_init)
   ============================ */
void worst_fit_init(void* heap_start, size_t heap_size)
{
    wf_head = (metadata_t*)heap_start;

    wf_head->size           = heap_size;
    wf_head->requested_size = 0;    /* free block */
    wf_head->in_use         = 0;
    wf_head->next           = NULL;
    wf_head->prev           = NULL;
}

/* ============================
   WORST FIT malloc
   ============================ */
void* worst_fit_malloc(size_t size)
{
    if (!wf_head || size == 0)
        return NULL;

    size_t total = sizeof(metadata_t) + size;

    metadata_t* curr  = wf_head;
    metadata_t* worst = NULL;

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
        split->requested_size = 0;     /* free block */
        split->in_use         = 0;
        split->next           = worst->next;
        split->prev           = worst;

        if (worst->next)
            worst->next->prev = split;

        worst->next = split;
        worst->size = total;
    }

    worst->in_use         = 1;
    worst->requested_size = size;   /* STEP 2 */
    ERRNO = NO_ERROR;

    return (char*)worst + sizeof(metadata_t);
}

/* ============================
   WORST FIT free
   ============================ */
void worst_fit_free(void* ptr)
{
    if (!ptr)
        return;

    metadata_t* block =
        (metadata_t*)((char*)ptr - sizeof(metadata_t));

    block->in_use         = 0;
    block->requested_size = 0;   /* STEP 2 */

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
