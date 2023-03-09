#include "include/mem.h"
#include <stdio.h>
#include <stdlib.h>

void *rallc(void *ptr, size_t os, size_t ns) {
  if (ns == 0) {
    free(ptr);
    return NULL;
  }

  void *result = realloc(ptr, ns);
  if (result == NULL) {
    fprintf(stderr, "Failed to allocate memory");
    exit(1);
  }
  return result;
}
