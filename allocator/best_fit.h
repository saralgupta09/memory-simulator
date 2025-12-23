#ifndef BEST_FIT_H
#define BEST_FIT_H

#include <stddef.h>

void* best_fit_malloc(size_t size);
void  best_fit_free(void* ptr);

#endif
