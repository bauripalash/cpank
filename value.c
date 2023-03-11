#include "include/value.h"
#include "include/mem.h"
#include "include/obj.h"
#include <stdbool.h>
#include <wchar.h>

void print_val_type(ValType vt) {
  switch (vt) {
  case V_NIL:
    wprintf(L"NIL");
  case V_BOOL:
    wprintf(L"BOOL");
  case V_NUM:
    wprintf(L"NUM");
  case V_OBJ:
    wprintf(L"OBJ");
  }
}

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

void print_val(Value val) {
  // print_val_type(val.type);
  switch (val.type) {
  case V_NUM:
    wprintf(L"%g", get_as_number(val));
    break;
  case V_BOOL:
    wprintf(L"%s", get_as_bool(val) ? "true" : "false");
    break;
  case V_NIL:
    wprintf(L"nil");
    break;
  case V_OBJ:
    print_obj(val);
    break;
  }
}

bool is_bool(Value val) { return val.type == V_BOOL; }

bool is_num(Value val) { return val.type == V_NUM; }

bool is_nil(Value val) { return val.type == V_NIL; }

bool is_obj(Value val) { return val.type == V_OBJ; }

bool get_as_bool(Value val) { return val.as.boolean; }

double get_as_number(Value val) { return val.as.num; }

bool is_falsey(Value val) {
  return is_nil(val) || (is_bool(val) && !get_as_bool(val));
}

bool is_equal(Value left, Value right) {
  if (left.type != right.type) {
    return false;
  }

  switch (left.type) {
  case V_BOOL:
    return get_as_bool(left) == get_as_bool(right);
  case V_NIL:
    return true;
  case V_NUM:
    return get_as_number(left) == get_as_number(right);
  case V_OBJ: {
    return get_as_obj(left) == get_as_obj(right);
    // ObjString *l_string = get_as_string(left);
    // ObjString *r_string = get_as_string(right);
    // return l_string->len == r_string->len &&
    //        wmemcmp(l_string->chars, r_string->chars, l_string->len) == 0;
  }
  default:
    return false;
  }
}

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
