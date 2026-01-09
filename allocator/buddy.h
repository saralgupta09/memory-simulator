#ifndef BUDDY_H
#define BUDDY_H

#include <stddef.h>

void buddy_init(void *heap_start, size_t heap_size);

void* buddy_malloc(size_t size);
void  buddy_free(void *ptr);

#endif
