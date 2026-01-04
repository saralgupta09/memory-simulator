#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "allocator/my_malloc.h"
#include "simulator/cli.h"
#include "observability/memory_dump.h"
#include "allocator/allocator.h"

#ifdef ENABLE_CACHE
#include "cache/cache.h"

CacheLevel L1;
CacheLevel L2;
CacheLevel L3;
#endif


/* =========================
Global cache levels
(visible to other files)
========================= */

int main(int argc, char **argv)
{
    #ifdef ENABLE_CACHE
        // cache_init(&L1,  128, 64, 1);   // L1: 32KB, 64B block, 4-way
        // cache_init(&L2, 1024, 64, 2);   // L2: 256KB, 8-way
        // cache_init(&L3,  8192, 64, 4); // L3: 1MB, 16-way
        cache_init(&L1, 1024, 64, 4);   // 16 lines, 4-way
        cache_init(&L2, 2048, 64, 4);   // 32 lines, 4-way
        cache_init(&L3, 4096, 64, 8);   // 64 lines, 8-way


    #endif
    
    
    if (argc > 1 && strcmp(argv[1], "--cli") == 0) {
        cli_run();
        return 0;
    }

    // printf("=== BEST FIT TEST ===\n");

    // allocator_init(1024);
    // set_allocator_strategy(ALLOC_FIRST_FIT);

    // void *a = my_malloc(100);   // block A
    // void *b = my_malloc(300);   // block B
    // void *c = my_malloc(100);   // block C

    // my_free(b);                 // create large hole
    // my_free(a);                 // create smaller hole

    // printf("\n-- Free layout before best-fit allocation --\n");
    // dump_memory();

    // set_allocator_strategy(ALLOC_WORST_FIT);
    // my_malloc(90);              // should go into smaller hole

    // printf("\n-- After best-fit allocation (90 bytes) --\n");
    // dump_memory();
    // my_malloc(100);
    // my_malloc(200);
    // my_malloc(200);
    // dump_memory();
//      allocator_init(1024);

//     void *p = my_malloc(500);

// #ifdef ENABLE_CACHE
// printf("\n-- Memory usage phase --\n");
// for (int i = 0; i < 10; i++) {
//     cache_hierarchy_access(&L1, &L2, &L3, (uint64_t)p);
// }
// #endif

//     dump_memory();

printf("\n=== BUDDY ALLOCATOR TEST ===\n");

allocator_init(1024);
set_allocator_strategy(ALLOC_BUDDY);

/*
 Heap = 1024 bytes
 Buddy system should split as:
 1024 → 512 → 256 → 128 → 64
*/

void *a = my_malloc(100);   // needs 128
void *b = my_malloc(100);   // needs another 128
void *c = my_malloc(200);   // needs 256

printf("\n-- After allocations --\n");
dump_memory();

/*
 Expected:
 - One 128 used
 - One 128 used
 - One 256 used
 - Remaining free blocks
*/

my_free(b);
my_free(a);

printf("\n-- After freeing a and b (should merge to 256) --\n");
dump_memory();

/*
 Expected:
 - a and b merge → 256 free
*/

my_free(c);

printf("\n-- After freeing c (should merge to 1024) --\n");
dump_memory();




    return 0;
}