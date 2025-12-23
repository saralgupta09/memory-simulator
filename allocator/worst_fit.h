#ifndef WORST_FIT_H
#define WORST_FIT_H

#include <stddef.h>

void* worst_fit_malloc(size_t size);
void  worst_fit_free(void* ptr);

#endif
