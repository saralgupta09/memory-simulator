#include "cache.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static uint32_t log2_uint(uint32_t x) {
    uint32_t r = 0;
    while (x >>= 1) r++;
    return r;
}

/* =========================
   Init
   ========================= */
void cache_init(CacheLevel *cache, uint32_t size, uint32_t block_size, uint32_t associativity) {
    // Basic setup
    cache->cache_size = size;
    cache->block_size = block_size;
    cache->associativity = associativity;
    cache->num_sets = size / (block_size * associativity);
    
    if (associativity > 4) cache->policy = CACHE_LRU; 
    else cache->policy = CACHE_LRU; 

    cache->hits = 0;
    cache->misses = 0;
    cache->global_time = 0;

    // Allocate Sets
    cache->sets = (CacheSet *)malloc(cache->num_sets * sizeof(CacheSet));

    for (uint32_t i = 0; i < cache->num_sets; i++) {
        cache->sets[i].num_lines = associativity;
        cache->sets[i].lines = (CacheLine *)calloc(associativity, sizeof(CacheLine));
    }
}


static int cache_level_access(CacheLevel *lvl, uint64_t address) {
    lvl->global_time++;

    uint32_t offset_bits = log2_uint(lvl->block_size);
    uint32_t index_bits  = log2_uint(lvl->num_sets);
    
    uint64_t set_index = (address >> offset_bits) & ((1ULL << index_bits) - 1);
    uint64_t tag       = address >> (offset_bits + index_bits);

    CacheSet *set = &lvl->sets[set_index];

    for (uint32_t i = 0; i < lvl->associativity; i++) {
        CacheLine *line = &set->lines[i];
        
        if (line->valid && line->tag == tag) {
            lvl->hits++;
            
            if (lvl->policy == CACHE_LRU) {
                line->lru_time = lvl->global_time;
            }
            return 1; 
        }
    }

    lvl->misses++;

    for (uint32_t i = 0; i < lvl->associativity; i++) {
        CacheLine *line = &set->lines[i];
        if (!line->valid) {
            line->valid = 1;
            line->tag = tag;
            line->insertion_time = lvl->global_time;
            line->lru_time = lvl->global_time;
            return 0; 
        }
    }

    int victim_index = 0;
    uint64_t min_time = UINT64_MAX;

    for (uint32_t i = 0; i < lvl->associativity; i++) {
        CacheLine *line = &set->lines[i];
        
        uint64_t timestamp = (lvl->policy == CACHE_FIFO) 
                           ? line->insertion_time 
                           : line->lru_time;
                           
        if (timestamp < min_time) {
            min_time = timestamp;
            victim_index = i;
        }
    }

    // Replace Victim
    CacheLine *victim = &set->lines[victim_index];
    victim->tag = tag;
    victim->insertion_time = lvl->global_time;
    victim->lru_time = lvl->global_time;
    
    return 0; // MISS
}


void cache_hierarchy_access(CacheLevel *L1, CacheLevel *L2, CacheLevel *L3, uint64_t address) {

    if (cache_level_access(L1, address)) {
        return;
    }

    if (cache_level_access(L2, address)) {

        return;
    }

    if (cache_level_access(L3, address)) {
        return;
    }

}

/* =========================
   Reporting
   ========================= */
void cache_print_stats(const char *name, CacheLevel *cache) {
    uint64_t total = cache->hits + cache->misses;
    double rate = total ? (double)cache->hits / total * 100.0 : 0.0;

    printf("=== %s Statistics ===\n", name);
    printf("Policy      : %s\n", cache->policy == CACHE_LRU ? "LRU" : "FIFO");
    printf("Hits        : %llu\n", (unsigned long long)cache->hits);
    printf("Misses      : %llu\n", (unsigned long long)cache->misses);
    printf("Total       : %llu\n", (unsigned long long)total);
    printf("Hit Rate    : %.2f%%\n\n", rate);
}