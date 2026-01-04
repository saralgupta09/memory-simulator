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
        cache_init(&L1,  128, 64, 1);   // L1: 32KB, 64B block, 4-way
        cache_init(&L2, 1024, 64, 2);   // L2: 256KB, 8-way
        cache_init(&L3,  8192, 64, 4); // L3: 1MB, 16-way
    #endif
    
    
    if (argc > 1 && strcmp(argv[1], "--cli") == 0) {
        cli_run();
        return 0;
    }

    printf("=== BEST FIT TEST ===\n");

    allocator_init(1024);
    set_allocator_strategy(ALLOC_FIRST_FIT);

    void *a = my_malloc(100);   // block A
    void *b = my_malloc(300);   // block B
    void *c = my_malloc(100);   // block C

    my_free(b);                 // create large hole
    my_free(a);                 // create smaller hole

    printf("\n-- Free layout before best-fit allocation --\n");
    dump_memory();

    set_allocator_strategy(ALLOC_BEST_FIT);
    my_malloc(90);              // should go into smaller hole

    printf("\n-- After best-fit allocation (90 bytes) --\n");
    dump_memory();

    return 0;
}