#ifndef BEST_FIT_H
#define BEST_FIT_H

#include <stddef.h>
#include "allocator_internal.h"


/* called ONCE by allocator_init */
void best_fit_init(void* heap_start, size_t heap_size);

void* best_fit_malloc(size_t size);
void  best_fit_free(void* ptr);

#endif
