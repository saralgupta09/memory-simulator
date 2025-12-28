#include "first_fit.h"
#include "my_malloc.h"
#include "allocator_internal.h"   // metadata_t, ERRNO, OUT_OF_MEMORY
#include <stdint.h>

#define MIN_BLOCK_SIZE (sizeof(metadata_t) + 8)

/* head of first-fit list */
static metadata_t* ff_head = NULL;

/* ============================
   INIT (called by allocator_init)
   ============================ */
void first_fit_init(void* heap_start, size_t heap_size)
{
    ff_head = (metadata_t*)heap_start;

    ff_head->size   = heap_size;
    ff_head->in_use = 0;
    ff_head->next   = NULL;
    ff_head->prev   = NULL;
}

/* ============================
   FIRST FIT malloc
   ============================ */
void* first_fit_malloc(size_t size)
{
    if (!ff_head || size == 0)
        return NULL;

    size_t total = sizeof(metadata_t) + size;
    metadata_t* curr = ff_head;

    while (curr)
    {
        if (!curr->in_use && curr->size >= total)
        {
            /* split if possible */
            if (curr->size >= total + MIN_BLOCK_SIZE)
            {
                metadata_t* split =
                    (metadata_t*)((char*)curr + total);

                split->size   = curr->size - total;
                split->in_use = 0;
                split->next   = curr->next;
                split->prev   = curr;

                if (curr->next)
                    curr->next->prev = split;

                curr->next = split;
                curr->size = total;
            }

            curr->in_use = 1;
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
   ============================ */
void first_fit_free(void* ptr)
{
    if (!ptr)
        return;

    metadata_t* block =
        (metadata_t*)((char*)ptr - sizeof(metadata_t));

    block->in_use = 0;

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
