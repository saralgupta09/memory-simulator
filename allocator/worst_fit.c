#include "worst_fit.h"
#include "my_malloc.h"
#include <stdint.h>

#define SBRK_SIZE 2048
#define MIN_BLOCK_SIZE (sizeof(metadata_t) + 8)

/* Initialize heap once */
static void init_heap_if_needed(void)
{
    if (heap)
        return;

    heap = my_sbrk(SBRK_SIZE);
    if (!heap)
        return;

    /* set global heap size ONCE */
    heap_size = SBRK_SIZE;

    metadata_t* block = (metadata_t*)heap;
    block->size = SBRK_SIZE;
    block->in_use = 0;
    block->next = NULL;
    block->prev = NULL;
}

/* WORST FIT malloc */
void* worst_fit_malloc(size_t size)
{
    init_heap_if_needed();
    if (!heap)
        return NULL;

    size_t total_size = sizeof(metadata_t) + size;

    metadata_t* curr = (metadata_t*)heap;
    metadata_t* worst = NULL;

    while (curr)
    {
        if (!curr->in_use && curr->size >= total_size)
        {
            if (!worst || curr->size > worst->size)
                worst = curr;
        }
        curr = curr->next;
    }

    if (!worst)
        return NULL;

    /* Split if possible */
    if (worst->size >= total_size + MIN_BLOCK_SIZE)
    {
        metadata_t* new_block =
            (metadata_t*)((char*)worst + total_size);

        new_block->size = worst->size - total_size;
        new_block->in_use = 0;
        new_block->next = worst->next;
        new_block->prev = worst;

        if (worst->next)
            worst->next->prev = new_block;

        worst->next = new_block;
        worst->size = total_size;
    }

    worst->in_use = 1;
    return (char*)worst + sizeof(metadata_t);
}

/* WORST FIT free */
void worst_fit_free(void* ptr)
{
    if (!ptr)
        return;

    metadata_t* block =
        (metadata_t*)((char*)ptr - sizeof(metadata_t));

    block->in_use = 0;

    /* Coalesce with next */
    if (block->next && !block->next->in_use)
    {
        metadata_t* next = block->next;
        block->size += next->size;
        block->next = next->next;

        if (next->next)
            next->next->prev = block;
    }

    /* Coalesce with prev */
    if (block->prev && !block->prev->in_use)
    {
        metadata_t* prev = block->prev;
        prev->size += block->size;
        prev->next = block->next;

        if (block->next)
            block->next->prev = prev;
    }
}
