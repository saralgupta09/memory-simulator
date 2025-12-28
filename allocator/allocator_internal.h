#ifndef ALLOCATOR_INTERNAL_H
#define ALLOCATOR_INTERNAL_H

#include <stddef.h>

/* Block metadata stored INSIDE heap */
typedef struct metadata {
    size_t size;             // total block size (metadata + payload)
    size_t requested_size;   // bytes requested by user
    int    in_use;

    struct metadata *next;
    struct metadata *prev;
} metadata_t;

/* global heap */
extern void   *heap;
extern size_t  heap_size;

#endif
