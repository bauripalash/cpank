#include "include/value.h"
#include "include/mem.h"
#include <stdbool.h>
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

void print_val(Value val) { wprintf(L"%g", get_as_number(val)); }

bool is_bool(Value val) { return val.type == V_BOOL; }

bool is_num(Value val) { return val.type == V_NUM; }

bool is_nil(Value val) { return val.type == V_NIL; }

bool get_as_bool(Value val) { return val.as.boolean; }

double get_as_number(Value val) { return val.as.num; }

Value make_num(double num) {
  Value v;
  v.type = V_NUM;
  v.as.num = num;
  return v;
}

Value make_bool(bool value) {
  Value v;
  v.type = V_BOOL;
  v.as.boolean = value;
  return v;
}

Value make_nil() {
  Value v;
  v.type = V_NIL;
  v.as.num = 0;
  return v;
}

Value make_neg(Value value) {
  Value v;
  v.type = value.type;
  v.as.num = -value.as.num;
  return v;
}
