#ifndef FIRST_FIT_H
#define FIRST_FIT_H

#include <stddef.h>

void* first_fit_malloc(size_t size);
void  first_fit_free(void* ptr);

#endif
