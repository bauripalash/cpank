#include "include/mem.h"
#include "include/common.h"
#include "include/compiler.h"
#include "include/instruction.h"
#include "include/obj.h"
#include "include/value.h"
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "include/vm.h"

#ifdef DEBUG_LOG_GC
#include "include/debug.h"
#endif

void *rallc(void *ptr, size_t os, size_t ns) {

  if (ns > os) {
#ifdef DEBUG_STRES_GC
    // collect_garbage();
#endif
  }

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
#ifdef DEBUG_LOG_GC
  wprintf(L"%p free type %d\n", (void *)obj, obj->type);
#endif
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
  case OBJ_CLOUSRE: {
    ObjClosure *cls = (ObjClosure *)obj;
    FREE_ARR(ObjUpVal *, cls->upv, cls->upv_count);
    FREE(ObjClosure, obj);
    break;
  }
  case OBJ_UPVAL: {
    FREE(ObjUpVal, obj);
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

  free(vm.gray_stack);
}

void mark_array(Valarr *arr) {
  for (int i = 0; i < arr->len; i++) {
    mark_val(arr->values[i]);
  }
}

void blacken_obj(Obj *obj) {
#ifdef DEBUG_LOG_GC
  setlocale(LC_CTYPE, "");
  wprintf(L"%p blacken ", (void *)obj);
  print_val(make_obj_val(obj));
  wprintf(L"\n");
#endif
  switch (obj->type) {
  case OBJ_NATIVE:
  case OBJ_STR:
    break;
  case OBJ_UPVAL:
    mark_val(((ObjUpVal *)obj)->closed);
    break;
  case OBJ_CLOUSRE: {
    ObjClosure *cls = (ObjClosure *)obj;
    mark_obj((Obj *)cls->func);
    for (int i = 0; i < cls->upv_count; i++) {
      mark_obj((Obj *)cls->upv[i]);
    }
    break;
  }
  case OBJ_FUNC: {
    ObjFunc *func = (ObjFunc *)obj;
    mark_obj((Obj *)func->name);
    mark_array(&func->ins.consts);
    break;
  }
  }
}

void mark_roots() {
  for (Value *slot = vm.stack; slot < vm.stack_top; slot++) {
    mark_val(*slot);
  }

  for (int i = 0; i < vm.frame_count; i++) {
    mark_obj((Obj *)vm.frames[i].closure);
  }

  for (ObjUpVal *upv = vm.open_upvs; upv != NULL; upv = upv->next) {
    mark_obj((Obj *)upv);
  }

  mark_table(&vm.globals);
  mark_compiler_roots();
}

void trace_refs() {
  while (vm.gray_count > 0) {
    Obj *obj = vm.gray_stack[--vm.gray_count];
    blacken_obj(obj);
  }
}

void mark_val(Value val) {
  if (is_obj(val)) {
    mark_obj(get_as_obj(val));
  }
}

void mark_obj(Obj *obj) {
  if (obj == NULL) {
    return;
  }

  if (obj->is_marked) {
    return;
  }

#ifdef DEBUG_LOG_GC
  setlocale(LC_CTYPE, "");
  wprintf(L"%p mark ", (void *)obj);
  print_val(make_obj_val(obj));
  wprintf(L"\n");
#endif

  obj->is_marked = true;
  if (vm.gray_cap < vm.gray_count + 1) {
    vm.gray_cap = GROW_CAP(vm.gray_cap);
    vm.gray_stack = (Obj **)realloc(vm.gray_stack, sizeof(Obj *) * vm.gray_cap);
    if (vm.gray_stack == NULL) {
      exit(1);
    }
  }
}

void collect_garbage() {
  setlocale(LC_CTYPE, "");
#ifdef DEBUG_LOG_GC
  wprintf(L"-- gc start\n");
#endif

  mark_roots();
  trace_refs();

#ifdef DEBUG_LOG_GC
  wprintf(L"-- gc end\n");
#endif
}
