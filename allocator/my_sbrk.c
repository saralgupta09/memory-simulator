#include <errno.h>
#include <stdlib.h>
#include <stddef.h>

#define HEAP_SIZE 0x2000   /* 8192 bytes total heap */

/* ===== GLOBAL HEAP STATE ===== */
void*  heap = NULL;
size_t heap_size = 0;

/*
 * Emulated sbrk
 */
void* my_sbrk(int increment)
{
    static char* fake_heap = NULL;
    static int   current_top = 0;
    void* ret;

    if (fake_heap == NULL)
    {
        fake_heap = calloc(HEAP_SIZE, 1);
        if (!fake_heap)
        {
            errno = ENOMEM;
            return (void*)-1;
        }

        heap = fake_heap;
        heap_size = 0;
    }

    if (current_top + increment > HEAP_SIZE || current_top + increment < 0)
    {
        errno = ENOMEM;
        return (void*)-1;
    }

    ret = fake_heap + current_top;
    current_top += increment;
    heap_size += increment;   // ✅ THIS is what stats reads

    return ret;
}
