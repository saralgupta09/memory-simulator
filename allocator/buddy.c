#include "buddy.h"
#include "allocator_internal.h"
#include "my_malloc.h"
#include <stdint.h>

#define MIN_ORDER 4   /* 2^4 = 16 bytes */
#define MAX_ORDERS 12 /* supports heap up to 2^12 = 4096 (adjust if needed) */

/* buddy free lists */
static metadata_t* freelist[MAX_ORDERS];
static void* heap_base = NULL;

/* =========================
   Helpers
   ========================= */

static int order_for_size(size_t size)
{
    int order = MIN_ORDER;
    size_t block = 1UL << order;

    while (block < size)
    {
        order++;
        block <<= 1;
    }
    return order;
}

static size_t size_for_order(int order)
{
    return (size_t)1 << order;
}

static metadata_t* get_buddy(metadata_t* block)
{
    uintptr_t offset =
        (uintptr_t)((char*)block - (char*)heap_base);

    uintptr_t buddy_offset =
        offset ^ block->size;

    return (metadata_t*)((char*)heap_base + buddy_offset);
}

static void freelist_push(int order, metadata_t* block)
{
    block->next = freelist[order];
    block->prev = NULL;

    if (freelist[order])
        freelist[order]->prev = block;

    freelist[order] = block;
}

static metadata_t* freelist_pop(int order)
{
    metadata_t* block = freelist[order];
    if (!block) return NULL;

    freelist[order] = block->next;
    if (freelist[order])
        freelist[order]->prev = NULL;

    block->next = block->prev = NULL;
    return block;
}

static void freelist_remove(int order, metadata_t* block)
{
    if (block->prev)
        block->prev->next = block->next;
    else
        freelist[order] = block->next;

    if (block->next)
        block->next->prev = block->prev;

    block->next = block->prev = NULL;
}

/* =========================
   INIT
   ========================= */

void buddy_init(void* heap_start, size_t heap_size)
{
    heap_base = heap_start;

    for (int i = 0; i < MAX_ORDERS; i++)
        freelist[i] = NULL;

    int order = order_for_size(heap_size);

    metadata_t* root = (metadata_t*)heap_start;
    root->size = size_for_order(order);
    root->requested_size = 0;
    root->in_use = 0;
    root->next = root->prev = NULL;

    freelist_push(order, root);
}

/* =========================
   MALLOC
   ========================= */

void* buddy_malloc(size_t size)
{
    if (size == 0)
        return NULL;

    size_t total = sizeof(metadata_t) + size;
    int target = order_for_size(total);

    int order = target;
    while (order < MAX_ORDERS && !freelist[order])
        order++;

    if (order >= MAX_ORDERS)
    {
        ERRNO = OUT_OF_MEMORY;
        return NULL;
    }

    metadata_t* block = freelist_pop(order);

    while (order > target)
    {
        order--;

        size_t half = block->size / 2;
        metadata_t* buddy =
            (metadata_t*)((char*)block + half);

        buddy->size = half;
        buddy->requested_size = 0;
        buddy->in_use = 0;
        buddy->next = buddy->prev = NULL;

        block->size = half;

        freelist_push(order, buddy);
    }

    block->in_use = 1;
    block->requested_size = size;
    ERRNO = NO_ERROR;

    return (char*)block + sizeof(metadata_t);
}

/* =========================
   FREE
   ========================= */

void buddy_free(void* ptr)
{
    if (!ptr)
        return;

    metadata_t* block =
        (metadata_t*)((char*)ptr - sizeof(metadata_t));

    block->in_use = 0;
    block->requested_size = 0;

    int order = order_for_size(block->size);

    while (order + 1 < MAX_ORDERS)
    {
        metadata_t* buddy = get_buddy(block);

        if (!buddy || buddy->in_use || buddy->size != block->size)
            break;

        freelist_remove(order, buddy);

        if (buddy < block)
            block = buddy;

        block->size <<= 1;
        order++;
    }

    freelist_push(order, block);
    ERRNO = NO_ERROR;
}
