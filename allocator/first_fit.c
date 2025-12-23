#include "first_fit.h"
#include "my_malloc.h"
#include <stdint.h>

#define SBRK_SIZE 2048
#define MIN_BLOCK_SIZE (sizeof(metadata_t) + 8)

/* SINGLE heap definition */
void* heap = NULL;

/* Initialize heap once */
static void init_heap(void)
{
    if (heap) return;

    heap = my_sbrk(SBRK_SIZE);
    if (!heap) {
        ERRNO = OUT_OF_MEMORY;
        return;
    }

    metadata_t* block = (metadata_t*)heap;
    block->size = SBRK_SIZE;
    block->in_use = 0;
    block->next = NULL;
    block->prev = NULL;
}

/* FIRST FIT malloc */
void* first_fit_malloc(size_t size)
{
    init_heap();
    if (!heap) return NULL;

    size_t total = sizeof(metadata_t) + size;
    metadata_t* curr = (metadata_t*)heap;

    while (curr)
    {
        if (!curr->in_use && curr->size >= total)
        {
            if (curr->size >= total + MIN_BLOCK_SIZE)
            {
                metadata_t* split =
                    (metadata_t*)((char*)curr + total);

                split->size = curr->size - total;
                split->in_use = 0;
                split->next = curr->next;
                split->prev = curr;

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

/* FIRST FIT free */
void first_fit_free(void* ptr)
{
    if (!ptr) return;

    metadata_t* block =
        (metadata_t*)((char*)ptr - sizeof(metadata_t));

    block->in_use = 0;

    /* Merge with next */
    if (block->next && !block->next->in_use)
    {
        metadata_t* n = block->next;
        block->size += n->size;
        block->next = n->next;
        if (n->next) n->next->prev = block;
    }

    /* Merge with prev */
    if (block->prev && !block->prev->in_use)
    {
        metadata_t* p = block->prev;
        p->size += block->size;
        p->next = block->next;
        if (block->next) block->next->prev = p;
    }

    ERRNO = NO_ERROR;
}
