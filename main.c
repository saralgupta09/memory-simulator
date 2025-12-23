#include <stdio.h>
#include "allocator/my_malloc.h"
#include "observability/memory_dump.h"
#include "allocator/allocator.h"

void run_test(alloc_strategy_t strategy, const char* name)
{
    set_allocator_strategy(strategy);

    printf("\n=== %s ===\n", name);
    dump_memory();
    void* a = my_malloc(100);
    void* b = my_malloc(300);
    void* c = my_malloc(200);
    void* d = my_malloc(180);

    dump_memory();

    my_free(b);
    my_free(d);

    dump_memory();
}

int main(void)
{
    run_test(ALLOC_FIRST_FIT, "FIRST FIT");
    run_test(ALLOC_BEST_FIT,  "BEST FIT");
    run_test(ALLOC_WORST_FIT, "WORST FIT");

    return 0;
}
