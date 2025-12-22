#include <stdio.h>
#include "allocator/my_malloc.h"
#include "observability/memory_dump.h"

int main(void)
{
    printf("=== Buddy Coalescing Test ===\n");

    void* a = my_malloc(100);   // rounds to 128
    void* b = my_malloc(100);   // rounds to 128 (buddy of a)
    void* c = my_malloc(200);   // rounds to 256

    printf("\nAfter allocations:\n");
    dump_memory();

    my_free(a);
    my_free(b);   // EXPECT: two 128s merge into 256

    printf("\nAfter freeing a and b (expect 256 merge):\n");
    dump_memory();

    my_free(c);   // EXPECT: further merge (to 512 if possible)

    printf("\nAfter freeing c (expect larger merge):\n");
    dump_memory();

    return 0;
}
