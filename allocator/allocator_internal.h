#ifndef ALLOCATOR_INTERNAL_H
#define ALLOCATOR_INTERNAL_H

#include <stddef.h>

/* Block metadata used internally by allocator */
typedef struct block {
    size_t start;          // offset from heap base
    size_t size;           // size of block
    int    free;            // 1 = free, 0 = used
    int    id;              // allocation id
    struct block *next;     // linked list
} block_t;

/* internal globals */
extern block_t *block_list_head;
extern void   *heap;
extern size_t  heap_size;

#endif
