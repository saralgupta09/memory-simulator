#ifndef WORST_FIT_H
#define WORST_FIT_H

#include <stddef.h>
#include "allocator_internal.h"


/* called ONCE by allocator_init */
void worst_fit_init(void* heap_start, size_t heap_size);

void* worst_fit_malloc(size_t size);
void  worst_fit_free(void* ptr);

#endif
