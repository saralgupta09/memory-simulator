#include <stdio.h>
#include <stdint.h>
#include "cache/cache.h"

/* =========================
   Global cache levels
   (visible to other files)
   ========================= */
CacheLevel L1;
CacheLevel L2;
CacheLevel L3;

int main() {

    /* Cache configuration */
    cache_init(&L1, 32 * 1024, 64, 4);    // L1: 32KB, 4-way
    cache_init(&L2, 256 * 1024, 64, 8);   // L2: 256KB, 8-way
    cache_init(&L3, 1024 * 1024, 64, 16); // L3: 1MB, 16-way

    /* Test access pattern */
    uint64_t addresses[] = {
        0x1000, 0x2000, 0x3000, 0x1000,
        0x2000, 0x4000, 0x5000, 0x1000,
        0x6000, 0x7000, 0x2000, 0x3000
    };

    int n = sizeof(addresses) / sizeof(addresses[0]);

    for (int i = 0; i < n; i++) {
        cache_hierarchy_access(&L1, &L2, &L3, addresses[i]);
    }

    /* Print cache statistics */
    cache_print_hierarchy_stats(&L1, &L2, &L3);

    /* Cleanup */
    cache_free(&L1);
    cache_free(&L2);
    cache_free(&L3);

    return 0;
}
