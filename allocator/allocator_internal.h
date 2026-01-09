#ifndef ALLOCATOR_INTERNAL_H
#define ALLOCATOR_INTERNAL_H

#include <stddef.h>

/* Block metadata stored INSIDE heap */
typedef struct metadata {
    size_t size;             // 8 bytes
    size_t requested_size;   // 8 bytes
    int    in_use;          //  4 bytes (+padding)
    struct metadata *next;  //  8 bytes
    struct metadata *prev;  //  8 bytes
} metadata_t;

extern void   *heap;
extern size_t  heap_size;

extern metadata_t *heap_head;

#endif
