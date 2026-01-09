#ifndef STATS_H
#define STATS_H

#include <stddef.h>

void stats_reset(void);
void stats_collect(void);
void stats_print(void);

/* allocation counters */
extern size_t total_alloc_requests;
extern size_t successful_allocs;
extern size_t failed_allocs;

typedef struct {
    size_t total_used;              // total allocated bytes (incl metadata)
    size_t total_free;              // total free bytes
    size_t internal_frag;           // payload slack only
    size_t largest_free_block;

    size_t allocator_overhead;      // NEW: metadata overhead
    size_t allocated_blocks;        // NEW: count of used blocks
} frag_stats_t;

void frag_stats_collect(frag_stats_t *fs);

void frag_stats_print(const frag_stats_t *fs);

#endif
