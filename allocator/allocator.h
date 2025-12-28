#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

typedef enum {
    ALLOC_FIRST_FIT,
    ALLOC_BEST_FIT,
    ALLOC_WORST_FIT,
    ALLOC_BUDDY
} alloc_strategy_t;

/* PUBLIC API */
int allocator_init(size_t size);
void set_allocator_strategy(alloc_strategy_t strategy);

void* my_malloc(size_t size);
void  my_free(void* ptr);
void* my_calloc(size_t num, size_t size);
void* my_memmove(void* dest, const void* src, size_t n);

#endif
