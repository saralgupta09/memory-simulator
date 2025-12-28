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


/* =========================
   Global cache levels
   (visible to other files)
   ========================= */

int main(int argc, char **argv)
{

    if (argc > 1 && strcmp(argv[1], "--cli") == 0) {
        cli_run();
        return 0;
    }


    allocator_init(2048);  


    void* a = my_malloc(100);
    void* b = my_malloc(200);

    dump_memory();

    my_free(a);
    dump_memory();

    my_free(b);
    dump_memory();

    return 0;
}
