#include "include/obj.h"
#include "include/mem.h"
#include "include/value.h"
#include "include/vm.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <wchar.h>

#define ALLOCATE_OBJ(type, otype) (type *)alloc_obj(sizeof(type), otype)

Obj *alloc_obj(size_t size, ObjType type) {
  Obj *obj = (Obj *)rallc(NULL, 0, size);
  obj->type = type;
  obj->next = vm.objs;
  vm.objs = obj;
  return obj;
}

ObjType get_obj_type(Value val) { return get_as_obj(val)->type; }

bool is_obj_type(Value val, ObjType ot) {
  return is_obj(val) && get_as_obj(val)->type == ot;
}

bool is_str_obj(Value val) { return is_obj_type(val, OBJ_STR); }

ObjString *get_as_string(Value val) { return (ObjString *)get_as_obj(val); }

wchar_t *get_as_native_string(Value val) {
  Obj *o = get_as_obj(val);
  ObjString *os = (ObjString *)(o);
  return os->chars;
}

ObjString *allocate_str(wchar_t *chars, int len) {
  ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STR);
  string->len = len;
  string->chars = chars;
  return string;
}

ObjString *copy_string(wchar_t *chars, int len) {
  wchar_t *heap_chars = ALLOC(wchar_t, len + 1);

  wmemcpy(heap_chars, chars, len);

  heap_chars[len] = '\0';

  return allocate_str(heap_chars, len);
}

ObjString *take_string(wchar_t *chars, int len) {
  return allocate_str(chars, len);
}

void print_obj(Value val) {
  switch (get_obj_type(val)) {
  case OBJ_STR:
    wprintf(L"%ls", get_as_native_string(val));
    break;
  }
}
