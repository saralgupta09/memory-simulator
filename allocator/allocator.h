#ifndef ALLOCATOR_H
#define ALLOCATOR_H

typedef enum {
    ALLOC_FIRST_FIT,
    ALLOC_BEST_FIT,
    ALLOC_WORST_FIT,
    ALLOC_BUDDY
} alloc_strategy_t;

/* Change allocator strategy at runtime */
void set_allocator_strategy(alloc_strategy_t strategy);

#endif
