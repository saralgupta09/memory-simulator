#ifndef STATS_H
#define STATS_H

#include <stddef.h>

/* =========================
   Existing allocation stats
   ========================= */

void stats_reset(void);
void stats_collect(void);
void stats_print(void);

/* allocation counters */
extern size_t total_alloc_requests;
extern size_t successful_allocs;
extern size_t failed_allocs;

/* =========================
   Fragmentation statistics
   ========================= */

typedef struct {
    size_t total_used;          /* total bytes used (including metadata) */
    size_t total_free;          /* total free bytes */
    size_t internal_frag;       /* wasted bytes inside allocated blocks */
    size_t largest_free_block;  /* largest free block size */
} frag_stats_t;

/* collect fragmentation info by walking heap */
void frag_stats_collect(frag_stats_t *fs);

/* print fragmentation info */
void frag_stats_print(const frag_stats_t *fs);

#endif
