#include <stdio.h>
#include <stddef.h>

#include "stats.h"
#include "../allocator/allocator_internal.h"

/* =========================
   Global allocation counters
   ========================= */
size_t total_alloc_requests = 0;
size_t successful_allocs    = 0;
size_t failed_allocs        = 0;

/* =========================
   Existing collected stats
   ========================= */
static size_t used_memory = 0;
static size_t free_memory = 0;
static size_t used_blocks = 0;
static size_t free_blocks = 0;
static size_t largest_free_block = 0;

/* =========================
   Reset stats
   ========================= */
void stats_reset(void)
{
    total_alloc_requests = 0;
    successful_allocs    = 0;
    failed_allocs        = 0;

    used_memory = 0;
    free_memory = 0;
    used_blocks = 0;
    free_blocks = 0;
    largest_free_block = 0;
}

/* =========================
   Walk heap and collect base stats
   ========================= */
void stats_collect(void)
{
    used_memory = 0;
    free_memory = 0;
    used_blocks = 0;
    free_blocks = 0;
    largest_free_block = 0;

    if (!heap || heap_size == 0)
        return;

    char* curr = (char*)heap;
    char* heap_end = curr + heap_size;

    while (curr < heap_end)
    {
        metadata_t* block = (metadata_t*)curr;

        if (block->size == 0)
            break;  /* safety */

        if (block->in_use)
        {
            used_blocks++;
            used_memory += block->size;
        }
        else
        {
            free_blocks++;
            free_memory += block->size;

            if (block->size > largest_free_block)
                largest_free_block = block->size;
        }

        curr += block->size;
    }
}

/* =========================
   Print base stats
   ========================= */
void stats_print(void)
{
    double utilization = 0.0;
    double external_frag = 0.0;
    double success_rate = 0.0;

    if (heap_size > 0)
        utilization = (double)used_memory / heap_size;

    if (free_memory > 0)
        external_frag = 1.0 - ((double)largest_free_block / free_memory);

    if (total_alloc_requests > 0)
        success_rate = ((double)successful_allocs / total_alloc_requests) * 100.0;

    printf("\n---------- SUMMARY ----------\n");
    printf("Total heap size        : %zu bytes\n", heap_size);
    printf("Used memory            : %zu bytes\n", used_memory);
    printf("Free memory            : %zu bytes\n", free_memory);
    printf("Used blocks            : %zu\n", used_blocks);
    printf("Free blocks            : %zu\n", free_blocks);
    printf("Memory utilization     : %.2f%%\n", utilization * 100.0);
    printf("External fragmentation : %.3f\n", external_frag);
    printf("Allocation requests    : %zu\n", total_alloc_requests);
    printf("Successful allocs      : %zu\n", successful_allocs);
    printf("Failed allocs          : %zu\n", failed_allocs);
    printf("Success rate           : %.2f%%\n", success_rate);
    printf("-----------------------------\n");
}

/* =========================
   Fragmentation stats (STEP 3)
   ========================= */
void frag_stats_collect(frag_stats_t *fs)
{
    fs->total_used = 0;
    fs->total_free = 0;
    fs->internal_frag = 0;
    fs->largest_free_block = 0;

    if (!heap || heap_size == 0)
        return;

    char* curr = (char*)heap;
    char* heap_end = curr + heap_size;

    while (curr < heap_end)
    {
        metadata_t* block = (metadata_t*)curr;

        if (block->size == 0)
            break;

        if (block->in_use)
        {
            size_t payload_size = block->size - sizeof(metadata_t);
            fs->total_used += block->size;

            if (payload_size > block->requested_size)
                fs->internal_frag +=
                    (payload_size - block->requested_size);
        }
        else
        {
            fs->total_free += block->size;

            if (block->size > fs->largest_free_block)
                fs->largest_free_block = block->size;
        }

        curr += block->size;
    }
}

/* =========================
   Print fragmentation stats
   ========================= */
void frag_stats_print(const frag_stats_t *fs)
{
    double external_frag = 0.0;

    if (fs->total_free > 0)
        external_frag =
            1.0 - ((double)fs->largest_free_block / fs->total_free);

    printf("\n------ FRAGMENTATION STATS ------\n");
    printf("Total used memory      : %zu bytes\n", fs->total_used);
    printf("Total free memory      : %zu bytes\n", fs->total_free);
    printf("Internal fragmentation : %zu bytes\n", fs->internal_frag);
    printf("Largest free block     : %zu bytes\n", fs->largest_free_block);
    printf("External fragmentation : %.3f\n", external_frag);
    printf("---------------------------------\n");
}
