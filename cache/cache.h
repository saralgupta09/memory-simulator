#ifndef CACHE_H
#define CACHE_H

#include <stdint.h>

/* =========================
   Cache Line
   ========================= */
typedef struct CacheLine {
    uint8_t  valid;
    uint64_t tag;
    uint64_t fifo_order;
} CacheLine;

/* =========================
   Cache Set
   ========================= */
typedef struct CacheSet {
    CacheLine *lines;
    uint32_t   num_lines;
} CacheSet;

/* =========================
   Cache Statistics
   ========================= */
typedef struct CacheStats {
    uint64_t hits;
    uint64_t misses;
} CacheStats;

/* =========================
   Cache Level
   ========================= */
typedef struct CacheLevel {
    uint32_t cache_size;
    uint32_t block_size;
    uint32_t associativity;
    uint32_t num_sets;

    CacheSet *sets;

    uint64_t fifo_counter;
    CacheStats stats;
} CacheLevel;

/* =========================
   Address Fields
   ========================= */
typedef struct AddressFields {
    uint64_t tag;
    uint32_t set_index;
} AddressFields;

/* =========================
   Cache API (IMPORTANT)
   ========================= */
void cache_init(CacheLevel *cache,
                uint32_t cache_size,
                uint32_t block_size,
                uint32_t associativity);

void cache_free(CacheLevel *cache);

int cache_access(CacheLevel *cache, uint64_t address);

void cache_hierarchy_access(CacheLevel *L1,
                            CacheLevel *L2,
                            CacheLevel *L3,
                            uint64_t address);

void cache_print_stats(const char *name, CacheLevel *cache);

void cache_print_hierarchy_stats(CacheLevel *L1,
                                 CacheLevel *L2,
                                 CacheLevel *L3);

#endif
