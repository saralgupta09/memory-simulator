#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"
#include "../allocator/allocator.h"
#include "../allocator/my_malloc.h"
#include "../observability/memory_dump.h"
#include "../stats/stats.h"

#define MAX_CLI_ALLOCS 1024

typedef struct {
    int   id;
    void *ptr;
    int   in_use;
} cli_alloc_t;

static cli_alloc_t alloc_table[MAX_CLI_ALLOCS];
static int next_id = 1;

/* =========================
   Helpers
   ========================= */

static void print_prompt(void)
{
    printf("> ");
    fflush(stdout);
}

static void reset_alloc_table(void)
{
    for (int i = 0; i < MAX_CLI_ALLOCS; i++) {
        alloc_table[i].in_use = 0;
        alloc_table[i].ptr = NULL;
        alloc_table[i].id = 0;
    }
    next_id = 1;
}

static int store_alloc(void *ptr)
{
    for (int i = 0; i < MAX_CLI_ALLOCS; i++) {
        if (!alloc_table[i].in_use) {
            alloc_table[i].in_use = 1;
            alloc_table[i].ptr = ptr;
            alloc_table[i].id = next_id++;
            return alloc_table[i].id;
        }
    }
    return -1;
}

static void* find_alloc(int id)
{
    for (int i = 0; i < MAX_CLI_ALLOCS; i++) {
        if (alloc_table[i].in_use && alloc_table[i].id == id)
            return alloc_table[i].ptr;
    }
    return NULL;
}

static void clear_alloc(int id)
{
    for (int i = 0; i < MAX_CLI_ALLOCS; i++) {
        if (alloc_table[i].in_use && alloc_table[i].id == id) {
            alloc_table[i].in_use = 0;
            alloc_table[i].ptr = NULL;
            return;
        }
    }
}

/* =========================
   CLI main loop
   ========================= */

void cli_run(void)
{
    char line[256];

    printf("Memory Simulator (memsim)\n");

    while (1) {
        print_prompt();

        if (!fgets(line, sizeof(line), stdin))
            break;

        char *cmd = strtok(line, " \n");
        if (!cmd)
            continue;

        /* ---- init memory ---- */
        if (strcmp(cmd, "init") == 0) {
            char *sub = strtok(NULL, " \n");
            char *sz  = strtok(NULL, " \n");

            if (!sub || !sz || strcmp(sub, "memory") != 0) {
                printf("Usage: init memory <size>\n");
                continue;
            }

            size_t size = (size_t)atoi(sz);
            allocator_init(size);
            reset_alloc_table();

            printf("Memory initialized with size %zu\n", size);
        }

        /* ---- strategy ---- */
        else if (strcmp(cmd, "strategy") == 0) {
            char *arg = strtok(NULL, " \n");
            if (!arg) {
                printf("Usage: strategy <first|best|worst>\n");
                continue;
            }

            if (strcmp(arg, "first") == 0) {
                set_allocator_strategy(ALLOC_FIRST_FIT);
                printf("Strategy set to FIRST FIT\n");
            }
            else if (strcmp(arg, "best") == 0) {
                set_allocator_strategy(ALLOC_BEST_FIT);
                printf("Strategy set to BEST FIT\n");
            }
            else if (strcmp(arg, "worst") == 0) {
                set_allocator_strategy(ALLOC_WORST_FIT);
                printf("Strategy set to WORST FIT\n");
            }
            else {
                printf("Unknown strategy\n");
            }
        }

        /* ---- malloc ---- */
        else if (strcmp(cmd, "malloc") == 0) {
            char *sz = strtok(NULL, " \n");
            if (!sz) {
                printf("Usage: malloc <size>\n");
                continue;
            }

            size_t size = (size_t)atoi(sz);
            void *ptr = my_malloc(size);

            if (!ptr) {
                printf("Allocation failed\n");
                continue;
            }

            int id = store_alloc(ptr);
            if (id < 0) {
                printf("CLI allocation table full\n");
                continue;
            }

            printf("Allocated block id=%d at address=%p\n", id, ptr);
        }

        /* ---- free ---- */
        else if (strcmp(cmd, "free") == 0) {
            char *idstr = strtok(NULL, " \n");
            if (!idstr) {
                printf("Usage: free <id>\n");
                continue;
            }

            int id = atoi(idstr);
            void *ptr = find_alloc(id);

            if (!ptr) {
                printf("Invalid block id\n");
                continue;
            }

            my_free(ptr);
            clear_alloc(id);
            printf("Block %d freed\n", id);
        }

        /* ---- dump memory ---- */
        else if (strcmp(cmd, "dump") == 0) {
            char *sub = strtok(NULL, " \n");
            if (!sub || strcmp(sub, "memory") != 0) {
                printf("Usage: dump memory\n");
                continue;
            }
            dump_memory();
        }

        /* ---- stats ---- */
        else if (strcmp(cmd, "stats") == 0) {
            stats_collect();
            stats_print();
        }

        /* ---- fragmentation stats ---- */
        else if (strcmp(cmd, "frag") == 0) {
            frag_stats_t fs;
            frag_stats_collect(&fs);
            frag_stats_print(&fs);
        }

        /* ---- exit ---- */
        else if (strcmp(cmd, "exit") == 0) {
            break;
        }

        else {
            printf("Unknown command\n");
        }
    }
}
