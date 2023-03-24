#include "include/mem.h"
#include "include/common.h"
#include "include/compiler.h"
#include "include/instruction.h"
#include "include/obj.h"
#include "include/value.h"
#include <locale.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "include/vm.h"

GcConfig gcon;

// #define DEBUG_LOG_GC

// #define NOGC

// #define DEBUG_STRES_GC
#ifdef DEBUG_LOG_GC
#include "include/debug.h"
#endif

#define GC_HEAD_GROW_FACT 2

void *rallc(void *ptr, size_t os, size_t ns) {
  vm.bts_allocated += ns - os;
  if (ns > os) {
#ifdef DEBUG_STRES_GC

    collect_garbage();
#endif
    if (vm.bts_allocated > vm.next_gc) {

      collect_garbage();
    }
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
  case OBJ_MOD: {

    // FREE_ARR(wchar_t, mod->name->chars, mod->name->len+1);
    FREE(ObjMod, obj);
    // free_single_obj((Obj*)mod->name);
    //  free(mod->name->chars);
    //  FREE(ObjString, mod->name);
    break;
  }
  }
}

void free_objs() {
  Obj *object = vm.objs;
  while (object != NULL) {
    Obj *next_obj = object->next;
    free_single_obj(object);
    object = next_obj;
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
  // setlocale(LC_CTYPE, "");
  wprintf(L"%p blacken -> %ls", (void *)obj, get_obj_type_as_string(obj->type));

  print_val(make_obj_val(obj));
  wprintf(L"\n");
#endif
  switch (obj->type) {
  case OBJ_NATIVE:
    break;
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
  case OBJ_MOD: {
    ObjMod *md = (ObjMod *)obj;
    mark_obj((Obj *)md->name);
    break;
  }
  }
}

void mark_roots() {
#ifdef DEBUG_LOG_GC
  wprintf(L"marking stack slots -> \n");
#endif
  for (Value *slot = vm.stack; slot < vm.stack_top; slot++) {
    mark_val(*slot);
  }
#ifdef DEBUG_LOG_GC
  wprintf(L"finished marking slots -> \n");
#endif
#ifdef DEBUG_LOG_GC
  wprintf(L"marking roots - frame closures -> \n");
#endif
  for (int i = 0; i < vm.mod_count; i++) {
    Module *mod = &vm.modules[i];
    for (int i = 0; i < mod->frame_count; i++) {
      mark_obj((Obj *)mod->frames[i].closure);
    }
  }

#ifdef DEBUG_LOG_GCC
  wprintf(L"finished marking frame closures -> \n");
#endif

#ifdef DEBUG_LOG_GCC
  wprintf(L"gc marking open upvalues -> \n");
#endif
  // for (int i = 0; i < vm.mod_count; i++) {
  // Module *mod = &vm.modules[i];
  for (ObjUpVal *upv = vm.current_mod->open_upvs; upv != NULL;
       upv = upv->next) {
    mark_obj((Obj *)upv);
  }
  //}

#ifdef DEBUG_LOG_GC
  wprintf(L"finished marking open upvalues -> \n");
#endif

#ifdef DEBUG_LOG_GC
  wprintf(L"gc marking table -> gloals -> \n");
#endif

  // mark_table(&vm.globals);
  // for (int i = 0; i < vm.mod_count; i++) {
  // Module *mod = &vm.modules[i];
  if (vm.current_mod->globals.len > 0) {

    mark_table(&vm.current_mod->globals);
  }
  //}

  // mark_table(&vm.current_mod->globals);
#ifdef DEBUG_LOG_GC
  wprintf(L"finished marking global table -> \n");
#endif
#ifdef DEBUG_LOG_GC
  wprintf(L"marking compiler roots -> \n");
#endif
  mark_compiler_roots();
#ifdef DEBUG_LOG_GC
  wprintf(L"finished marking compiler roots -> \n");
#endif
}

void trace_refs() {
  while (vm.gray_count > 0) {
    Obj *obj = vm.gray_stack[--vm.gray_count];
    blacken_obj(obj);
  }
}

void sweep() {
  Obj *prev = NULL;
  Obj *obj = vm.objs;
  while (obj != NULL) {
    if (obj->is_marked) {
      obj->is_marked = false;
      prev = obj;
      obj = obj->next;
    } else {
      Obj *unreached = obj;
      obj = obj->next;
      if (prev != NULL) {
        prev->next = obj;
      } else {
        vm.objs = obj;
      }

      free_single_obj(unreached);
    }
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
  // setlocale(LC_CTYPE, "");
  wprintf(L"%p mark  -> ", (void *)obj);
  //  Value v = make_obj_val(obj);

  wprintf(L"%ls\n", get_obj_type_as_string(obj->type));
  // ObjFunc * f = (ObjFunc *)obj;
  // dissm_ins_chunk(&f->ins, L"debug");
  // print_val(make_obj_val(obj));
  // print_obj(make_obj_val(obj));
  wprintf(L"\n");
#endif

  obj->is_marked = true;
  if (vm.gray_cap < vm.gray_count + 1) {
    // wprintf(L"++ growing gstack cap \n");
    vm.gray_cap = GROW_CAP(vm.gray_cap);
    vm.gray_stack = (Obj **)realloc(vm.gray_stack, sizeof(Obj *) * vm.gray_cap);
    if (vm.gray_stack == NULL) {
      exit(1);
    }
  }

  vm.gray_stack[vm.gray_count++] = obj;
}

void collect_garbage() {
#ifndef NOGC
  // setlocale(LC_CTYPE, "");
#ifdef DEBUG_LOG_GC
  wprintf(L"-- gc start\n");
  size_t before = vm.bts_allocated;
#endif

#ifdef DEBUG_LOG_GC
  wprintf(L"gc marking roots -> \n");
#endif

  mark_roots();

#ifdef DEBUG_LOG_GC
  wprintf(L"finished marking roots -> \n");
#endif
  trace_refs();
  table_remove_white(&vm.strings);

  sweep();

  vm.next_gc = vm.bts_allocated * GC_HEAD_GROW_FACT;

#ifdef DEBUG_LOG_GC
  wprintf(L"-- gc end\n");
  wprintf(L"  collected %zu bytes (from %zu to %zu) next at %zu",
          before - vm.bts_allocated, before, vm.bts_allocated, vm.next_gc);
#endif
#endif
}
