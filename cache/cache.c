#include "cache.h"
#include <stdlib.h>
#include <stdio.h>

/* =========================================
   Helper: integer log2 (power of two)
   ========================================= */
static uint32_t log2_uint(uint32_t x) {
    uint32_t result = 0;
    while (x >>= 1) {
        result++;
    }
    return result;
}

/* =========================================
   Cache initialization
   ========================================= */
void cache_init(CacheLevel *cache,
                uint32_t cache_size,
                uint32_t block_size,
                uint32_t associativity) {

    cache->cache_size    = cache_size;
    cache->block_size    = block_size;
    cache->associativity = associativity;
    cache->num_sets = cache_size / (block_size * associativity);

    cache->fifo_counter = 0;
    cache->stats.hits   = 0;
    cache->stats.misses = 0;

    cache->sets = malloc(cache->num_sets * sizeof(CacheSet));
    for (uint32_t i = 0; i < cache->num_sets; i++) {
        cache->sets[i].num_lines = associativity;
        cache->sets[i].lines = malloc(associativity * sizeof(CacheLine));

        for (uint32_t j = 0; j < associativity; j++) {
            cache->sets[i].lines[j].valid = 0;
            cache->sets[i].lines[j].fifo_order = 0;
        }
    }
}

/* =========================================
   Cache cleanup
   ========================================= */
void cache_free(CacheLevel *cache) {
    for (uint32_t i = 0; i < cache->num_sets; i++) {
        free(cache->sets[i].lines);
    }
    free(cache->sets);
}

/* =========================================
   Address decomposition
   ========================================= */
AddressFields extract_address_fields(CacheLevel *cache, uint64_t address) {
    AddressFields f;
    uint32_t offset_bits = log2_uint(cache->block_size);
    uint32_t index_bits  = log2_uint(cache->num_sets);

    f.set_index = (address >> offset_bits) & ((1ULL << index_bits) - 1);
    f.tag = address >> (offset_bits + index_bits);
    return f;
}

/* =========================================
   Internal insert WITHOUT stats
   ========================================= */
static void cache_insert_no_stats(CacheLevel *cache, uint64_t address) {
    AddressFields f = extract_address_fields(cache, address);
    CacheSet *set = &cache->sets[f.set_index];

    /* empty line */
    for (uint32_t i = 0; i < set->num_lines; i++) {
        if (!set->lines[i].valid) {
            set->lines[i].valid = 1;
            set->lines[i].tag = f.tag;
            set->lines[i].fifo_order = cache->fifo_counter++;
            return;
        }
    }

    /* FIFO eviction */
    uint32_t victim = 0;
    uint64_t oldest = set->lines[0].fifo_order;
    for (uint32_t i = 1; i < set->num_lines; i++) {
        if (set->lines[i].fifo_order < oldest) {
            oldest = set->lines[i].fifo_order;
            victim = i;
        }
    }

    set->lines[victim].tag = f.tag;
    set->lines[victim].fifo_order = cache->fifo_counter++;
}

/* =========================================
   Cache access (counts stats)
   ========================================= */
int cache_access(CacheLevel *cache, uint64_t address) {
    AddressFields f = extract_address_fields(cache, address);
    CacheSet *set = &cache->sets[f.set_index];

    for (uint32_t i = 0; i < set->num_lines; i++) {
        if (set->lines[i].valid && set->lines[i].tag == f.tag) {
            cache->stats.hits++;
            return 1;
        }
    }

    cache->stats.misses++;
    cache_insert_no_stats(cache, address);
    return 0;
}

/* =========================================
   Multilevel cache access (CORRECT)
   ========================================= */
void cache_hierarchy_access(CacheLevel *L1,
                            CacheLevel *L2,
                            CacheLevel *L3,
                            uint64_t address) {

    if (cache_access(L1, address)) return;

    if (cache_access(L2, address)) {
        cache_insert_no_stats(L1, address);
        return;
    }

    if (cache_access(L3, address)) {
        cache_insert_no_stats(L2, address);
        cache_insert_no_stats(L1, address);
        return;
    }

    /* memory fetch */
    cache_insert_no_stats(L3, address);
    cache_insert_no_stats(L2, address);
    cache_insert_no_stats(L1, address);
}

/* =========================================
   Cache statistics printing
   ========================================= */
void cache_print_stats(const char *name, CacheLevel *cache) {
    uint64_t h = cache->stats.hits;
    uint64_t m = cache->stats.misses;
    uint64_t t = h + m;

    printf("=== %s Cache Statistics ===\n", name);
    printf("Hits        : %lu\n", h);
    printf("Misses      : %lu\n", m);
    printf("Total Access: %lu\n", t);
    printf("Hit Ratio   : %.4f\n\n", t ? (double)h / t : 0.0);
}

void cache_print_hierarchy_stats(CacheLevel *L1,
                                 CacheLevel *L2,
                                 CacheLevel *L3) {
    printf("########################################\n");
    printf("       CACHE HIERARCHY STATISTICS       \n");
    printf("########################################\n\n");

    cache_print_stats("L1", L1);
    cache_print_stats("L2", L2);
    cache_print_stats("L3", L3);
}
