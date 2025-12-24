#ifndef __MY_MALLOC_H__
#define __MY_MALLOC_H__

#include <stddef.h>
#include <stdint.h>

/* =========================
   Heap globals (declared once)
   ========================= */
extern void*  heap;
extern size_t heap_size;

/* =========================
   Metadata structure
   ========================= */
typedef struct metadata
{
    short in_use;
    short size;
    struct metadata* next;
    struct metadata* prev;
} metadata_t;

/* =========================
   Error enum
   ========================= */
enum my_malloc_err {
    NO_ERROR,
    OUT_OF_MEMORY,
    SINGLE_REQUEST_TOO_LARGE,
    DOUBLE_FREE_DETECTED
};

extern enum my_malloc_err ERRNO;

/* =========================
   Public allocator API
   ========================= */
void* my_malloc(size_t size);
void  my_free(void* ptr);
void* my_calloc(size_t num, size_t size);
void* my_memmove(void* dest, const void* src, size_t num_bytes);

void* my_sbrk(int);

#endif
