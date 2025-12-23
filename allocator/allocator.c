#include "allocator.h"
#include "my_malloc.h"

#include "first_fit.h"
#include "best_fit.h"
#include "worst_fit.h"

enum my_malloc_err ERRNO = NO_ERROR; 
/* buddy allocator later */

static alloc_strategy_t current_strategy = ALLOC_FIRST_FIT;

/* Called once or anytime */
void set_allocator_strategy(alloc_strategy_t strategy)
{
    current_strategy = strategy;
}

/* Override my_malloc */
void* my_malloc(size_t size)
{
    void* ptr = NULL;

    switch (current_strategy)
    {
        case ALLOC_FIRST_FIT:
            ptr = first_fit_malloc(size);
            break;

        case ALLOC_BEST_FIT:
            ptr = best_fit_malloc(size);
            break;

        case ALLOC_WORST_FIT:
            ptr = worst_fit_malloc(size);
            break;

        case ALLOC_BUDDY:
            ptr = NULL; /* later */
            break;
    }

    if (ptr)
        ERRNO = NO_ERROR;
    else
        ERRNO = OUT_OF_MEMORY;

    return ptr;
}


/* Override my_free */
void my_free(void* ptr)
{
    if (!ptr)
        return;

    switch (current_strategy)
    {
        case ALLOC_FIRST_FIT:
            first_fit_free(ptr);
            break;

        case ALLOC_BEST_FIT:
            best_fit_free(ptr);
            break;

        case ALLOC_WORST_FIT:
            worst_fit_free(ptr);
            break;

        case ALLOC_BUDDY:
            /* later */
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
