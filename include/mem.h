#ifndef cpank_mem_h
#define cpank_mem_h

#include "common.h"

#define ALLOC(type, count) (type *)rallc(NULL, 0, sizeof(type) * (count))

#define GROW_CAP(cap) ((cap) < 8 ? 8 : (cap)*2)

#define GROW_ARR(type, ptr, oc, nc)                                            \
  (type *)rallc(ptr, sizeof(type) * (oc), sizeof(type) * (nc))

#define FREE_ARR(type, ptr, oc) rallc(ptr, sizeof(type) * (oc), 0)

void *rallc(void *ptr, size_t osize, size_t nsize);

#endif
