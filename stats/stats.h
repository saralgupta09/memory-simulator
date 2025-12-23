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

#endif

