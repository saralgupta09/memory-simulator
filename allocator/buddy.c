#include "my_malloc.h"
#include <stdint.h>
#define SBRK_SIZE 2048

#ifdef DEBUG
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x)
#endif

void* heap;

// enum my_malloc_err ERRNO = NO_ERROR;
metadata_t* freelist[8];

/* --- helper functions unchanged --- */

int get_index(size_t size)
{
    int index = 0;
    int free_size = 16;
    while ((int)size > free_size)
    {
        free_size *= 2;
        index++;
    }
    return index;
}

metadata_t* get_buddy(metadata_t* address)
{
    uintptr_t buddy_address =
        (uintptr_t)address ^ (uintptr_t)address->size;

    metadata_t* buddy = (metadata_t*)buddy_address;
    if (address->size == buddy->size)
        return buddy;

    return NULL;
}

void setup_for_removal(metadata_t* one, metadata_t* two)
{
    if (one->next == two) one->next = two->next;
    if (one->prev == two) one->prev = two->prev;
    if (two->next == one) two->next = one->next;
    if (two->prev == one) two->prev = one->prev;
}

metadata_t* remove_from_freelist(metadata_t* node)
{
    metadata_t* next = node->next;
    metadata_t* prev = node->prev;

    int index = get_index(node->size);

    if (prev && next)
    {
        prev->next = next;
        next->prev = prev;
    }
    else if (prev && !next)
    {
        prev->next = NULL;
    }
    else if (!prev && next)
    {
        freelist[index] = next;
        next->prev = NULL;
    }
    else
    {
        freelist[index] = NULL;
    }

    node->next = NULL;
    node->prev = NULL;

    return node;
}

void add_to_freelist(int index, metadata_t* node)
{
    metadata_t* head = freelist[index];

    node->prev = NULL;
    node->next = head;

    if (head)
        head->prev = node;

    freelist[index] = node;
}

void split_memory(int target_index, int current_index, metadata_t* block)
{
    while (current_index > target_index)
    {
        remove_from_freelist(block);

        block->size /= 2;

        metadata_t* buddy =
            (metadata_t*)((char*)block + block->size);

        buddy->size = block->size;
        buddy->in_use = 0;
        buddy->next = NULL;
        buddy->prev = NULL;

        add_to_freelist(current_index - 1, buddy);

        current_index--;
        add_to_freelist(current_index, block);
    }
}

metadata_t* get_first(metadata_t* one, metadata_t* two)
{
    return (one > two) ? two : one;
}

metadata_t* merge_buddies(metadata_t* block, metadata_t* buddy)
{
    metadata_t* first = get_first(block, buddy);
    first->size *= 2;
    return first;
}

void* remove_and_return_block(int index)
{
    metadata_t* block = remove_from_freelist(freelist[index]);
    block->in_use = 1;
    ERRNO = NO_ERROR;
    return (void*)((char*)block + sizeof(metadata_t));
}

/* ===============================
   BUDDY ALLOCATOR PUBLIC API
   =============================== */

void* buddy_malloc(size_t size)
{
    size = sizeof(metadata_t) + size;

    if (size > 2048)
    {
        ERRNO = SINGLE_REQUEST_TOO_LARGE;
        return NULL;
    }

    if (!heap)
    {
        heap = my_sbrk(SBRK_SIZE);
        freelist[7] = (metadata_t*)heap;
        freelist[7]->in_use = 0;
        freelist[7]->size = SBRK_SIZE;
        freelist[7]->next = NULL;
        freelist[7]->prev = NULL;
    }

    if (!heap) return NULL;

    int index = get_index(size);

    if (freelist[index])
        return remove_and_return_block(index);

    int next_index = index + 1;
    while (next_index < 8 && !freelist[next_index])
        next_index++;

    if (next_index < 8)
    {
        split_memory(index, next_index, freelist[next_index]);
        return remove_and_return_block(index);
    }
    else
    {
        void* heap_ptr = my_sbrk(SBRK_SIZE);
        if (heap_ptr == (void*)-1)
        {
            ERRNO = OUT_OF_MEMORY;
            return NULL;
        }

        metadata_t* new_heap = (metadata_t*)heap_ptr;
        new_heap->in_use = 0;
        new_heap->size = SBRK_SIZE;
        new_heap->next = NULL;
        new_heap->prev = NULL;

        add_to_freelist(7, new_heap);
        split_memory(index, 7, freelist[7]);

        return remove_and_return_block(index);
    }
}

void* buddy_calloc(size_t num, size_t size)
{
    if (num * size > 2048)
    {
        ERRNO = SINGLE_REQUEST_TOO_LARGE;
        return NULL;
    }

    void* p = buddy_malloc(num * size);

    for (int i = 0;
         i < (((metadata_t*)p - 1)->size - sizeof(metadata_t));
         i++)
    {
        *((char*)p + i) = 0;
    }

    ERRNO = NO_ERROR;
    return p;
}

void buddy_free(void* ptr)
{
    metadata_t* block = (metadata_t*)((char*)ptr - sizeof(metadata_t));
    metadata_t* buddy = get_buddy(block);

    if (!buddy && !block->in_use)
    {
        ERRNO = DOUBLE_FREE_DETECTED;
        return;
    }

    block->in_use = 0;

    if (buddy && !buddy->in_use && buddy->size != SBRK_SIZE)
    {
        setup_for_removal(block, buddy);
        remove_from_freelist(buddy);
        remove_from_freelist(block);

        metadata_t* first = merge_buddies(block, buddy);
        add_to_freelist(get_index(first->size), first);

        if (get_buddy(first) && first->size != SBRK_SIZE)
            buddy_free((char*)first + sizeof(metadata_t));
    }
    else
    {
        add_to_freelist(get_index(block->size), block);
    }

    ERRNO = NO_ERROR;
}

void* buddy_memmove(void* dest, const void* src, size_t num_bytes)
{
    char* d = (char*)dest;
    char* s = (char*)src;

    if (d == s) return dest;

    if (d > s)
        for (int i = num_bytes - 1; i >= 0; i--)
            d[i] = s[i];
    else
        for (int i = 0; i < num_bytes; i++)
            d[i] = s[i];

    return dest;
}
