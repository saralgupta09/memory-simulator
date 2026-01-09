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

int main(int argc, char **argv){

    if (argc > 1 && strcmp(argv[1], "--cli") == 0) {
        cli_run();
        return 0;
    }

#ifdef ENABLE_CACHE

    CacheLevel L1, L2, L3;

    /* Cache initialization (as per your final config) */
    cache_init(&L1, 1024, 64, 4);  // L1
    cache_init(&L2, 2048, 64, 8);  // L2
    cache_init(&L3, 4096, 64, 16);  // L3

    printf("=== CACHE HIT / MISS TEST ===\n");

    /*
        We will use addresses that:
        - Map to same L1 set (to force eviction)
        - Still fit in L2
    */

    uint64_t base = 0x100000;

    uint64_t addr[8];
    for (int i = 0; i < 8; i++)
        addr[i] = base + i * 1024;   // large stride → same set, different lines

    /* ------------------------------------------------
       Phase 1: Cold misses (populate all caches)
       ------------------------------------------------ */
    for (int i = 0; i < 4; i++)
        cache_hierarchy_access(&L1, &L2, &L3, addr[i]);

    /* ------------------------------------------------
       Phase 2: Re-access → L1 hits
       ------------------------------------------------ */
    for (int i = 0; i < 4; i++)
        cache_hierarchy_access(&L1, &L2, &L3, addr[i]);

    /* ------------------------------------------------
       Phase 3: Evict from L1 only (overflow 4-way L1 set)
       ------------------------------------------------ */
    for (int i = 4; i < 8; i++)
        cache_hierarchy_access(&L1, &L2, &L3, addr[i]);

    /* ------------------------------------------------
       Phase 4: Access old addresses again → L2 hits
       ------------------------------------------------ */
    for (int i = 0; i < 4; i++)
        cache_hierarchy_access(&L1, &L2, &L3, addr[i]);

    /* ------------------------------------------------
       Print stats
       ------------------------------------------------ */
    cache_print_stats("L1", &L1);
    cache_print_stats("L2", &L2);
    cache_print_stats("L3", &L3);

#endif
    return 0;
}
