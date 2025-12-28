#ifndef MEMORY_DUMP_H
#define MEMORY_DUMP_H

/*
 * Prints the current layout of the heap.
 * Shows FREE and USED blocks in address order.
 * Does NOT modify allocator state.
 */
void dump_memory(void);

#endif