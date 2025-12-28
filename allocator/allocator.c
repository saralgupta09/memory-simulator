#include "allocator.h"
#include "my_malloc.h"
#include "../stats/stats.h"
#include "first_fit.h"
#include "allocator_internal.h"
#include <stdlib.h>

enum my_malloc_err ERRNO = NO_ERROR;

/* allocator strategy */
static alloc_strategy_t current_strategy = ALLOC_FIRST_FIT;
static int allocator_initialized = 0;

/* GLOBAL HEAP OWNERSHIP */
void   *heap = NULL;
size_t  heap_size = 0;

/* ============================
   PUBLIC API
   ============================ */

void set_allocator_strategy(alloc_strategy_t strategy)
{
    current_strategy = strategy;
}

int allocator_init(size_t size)
{
    if (allocator_initialized)
        return 1;

    if (size == 0)
        return 0;

    heap = my_sbrk(size);
    if (!heap)
        return 0;

    heap_size = size;

    /* allocator creates heap
       allocation strategies manage blocks */
    first_fit_init(heap, size);

    allocator_initialized = 1;
    return 1;
}

/* ============================
   malloc / free
   ============================ */

void* my_malloc(size_t size)
{
    total_alloc_requests++;

    void* result = NULL;

    switch (current_strategy)
    {
        case ALLOC_FIRST_FIT:
            result = first_fit_malloc(size);
            break;

        default:
            result = NULL;
            break;
    }

    if (result)
        successful_allocs++;
    else
        failed_allocs++;

    return result;
}

void my_free(void* ptr)
{
    if (!ptr)
        return;

    switch (current_strategy)
    {
        case ALLOC_FIRST_FIT:
            first_fit_free(ptr);
            break;

        default:
            break;
    }
}

void* my_calloc(size_t num, size_t size)
{
    size_t total = num * size;
    void* ptr = my_malloc(total);
    if (!ptr)
        return NULL;

    char* p = (char*)ptr;
    for (size_t i = 0; i < total; i++)
        p[i] = 0;

    return ptr;
}

void* my_memmove(void* dest, const void* src, size_t n)
{
    char* d = (char*)dest;
    const char* s = (const char*)src;

    if (d == s) return dest;

    if (d > s)
        for (size_t i = n; i-- > 0;)
            d[i] = s[i];
    else
        for (size_t i = 0; i < n; i++)
            d[i] = s[i];

    return dest;
}
