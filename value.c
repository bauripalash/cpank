#include "include/value.h"
#include "include/mem.h"
#include <wchar.h>

void init_valarr(Valarr *array) {
  array->values = NULL;
  array->cap = 0;
  array->len = 0;
}

void write_valarr(Valarr *array, Value value) {
  if (array->cap < array->len + 1) {
    int old_cap = array->cap;
    array->cap = GROW_CAP(old_cap);
    array->values = GROW_ARR(Value, array->values, old_cap, array->cap);
  }

  array->values[array->len] = value;
  array->len++;
}

void free_valarr(Valarr *array) {
  FREE_ARR(Value, array->values, array->cap);
  init_valarr(array);
}

void print_val(Value val) { wprintf(L"%g", val); }
