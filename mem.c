#include "include/mem.h"
#include "include/instruction.h"
#include "include/obj.h"
#include "include/value.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "include/vm.h"

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

void free_single_obj(Obj *obj) {
  switch (obj->type) {
  case OBJ_STR: {
    ObjString *str = (ObjString *)obj;
    FREE_ARR(wchar_t, str->chars, str->len + 1);
    FREE(ObjString, obj);
    break;
  }
  case OBJ_FUNC: {
    ObjFunc *func = (ObjFunc *)obj;
    free_ins(&func->ins);
    FREE(ObjFunc, obj);
    break;
  }
  case OBJ_NATIVE: {
    FREE(ObjNative, obj);
    break;
  }
  }
}

void free_objs() {
  Obj *object = vm.objs;
  while (object != NULL) {
    Obj *next = object->next;
    free_single_obj(object);
    object = next;
  }
}
