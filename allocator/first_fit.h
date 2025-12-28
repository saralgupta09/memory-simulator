#ifndef FIRST_FIT_H
#define FIRST_FIT_H

#include <stddef.h>
#include "allocator_internal.h"


/* called ONCE by allocator_init */
void first_fit_init(void* heap_start, size_t heap_size);

void* first_fit_malloc(size_t size);
void  first_fit_free(void* ptr);

#endif
