#include "include/mem.h"

#include <locale.h>
#include <stdbool.h>
#include <stddef.h>
// #include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "include/common.h"
#include "include/compiler.h"
#include "include/instruction.h"
#include "include/obj.h"
#include "include/utils.h"
#include "include/value.h"
#include "include/vm.h"

GcConfig gcon;

// #define DEBUG_LOG_GC

// #define NOGC

 #define DEBUG_STRES_GC
#ifdef DEBUG_LOG_GC
#include "include/debug.h"
#endif

#define GC_HEAD_GROW_FACT 2

void *rallc(PankVm *vm, void *ptr, size_t os, size_t ns) {
    vm->bts_allocated += ns - os;
    if (ns > os) {
#ifdef DEBUG_STRES_GC

        collect_garbage(vm);
#endif
        if (vm->bts_allocated > vm->next_gc) {
            collect_garbage(vm);
        }
    }

    if (ns == 0) {
        free(ptr);
        return NULL;
    }

    void *result = realloc(ptr, ns);
    if (result == NULL) {
        // fprintf(stderr, "Failed to allocate memory");
        cp_err_println(L"Fail to allocate memory");
        exit(1);
    }
    return result;
}

void free_single_obj(PankVm *vm, Obj *obj) {
#ifdef DEBUG_LOG_GC
    cp_color_print('b', L"[GC] (%p) Freeing : T(%ls) : V(", (void *)obj,
                   get_obj_type_as_string(obj->type));
    print_obj(make_obj_val(obj));
    cp_color_println('b', L")");
#endif
    switch (obj->type) {
        case OBJ_STR: {
            ObjString *str = (ObjString *)obj;
            FREE_ARR(vm, wchar_t, str->chars, str->len + 1);
            FREE(vm, ObjString, obj);
            break;
        }

        case OBJ_FUNC: {
            ObjFunc *func = (ObjFunc *)obj;
            free_ins(vm, &func->ins);
            FREE(vm, ObjFunc, obj);
            break;
        }
        case OBJ_NATIVE: {
            FREE(vm, ObjNative, obj);
            break;
        }
        case OBJ_CLOUSRE: {
            ObjClosure *cls = (ObjClosure *)obj;
            FREE_ARR(vm, ObjUpVal *, cls->upv, cls->upv_count);
            FREE(vm, ObjClosure, obj);
            break;
        }
        case OBJ_UPVAL: {
            FREE(vm, ObjUpVal, obj);
            break;
        }
        case OBJ_MOD: {
            FREE(vm, ObjMod, obj);
            break;
        }
        case OBJ_ERR: {
            ObjErr *err = (ObjErr *)obj;
            FREE_ARR(vm, wchar_t, err->errmsg, err->len + 1);

            FREE(vm, ObjErr, obj);
            break;
        }
        case OBJ_ARRAY: {
            ObjArray *array = (ObjArray *)obj;
            free_valarr(vm, &array->items);
            FREE(vm, ObjArray, obj);
            break;
        }
    }
}

void free_objs(PankVm *vm) {
    Obj *object = vm->objs;
    while (object != NULL) {
        Obj *next_obj = object->next;
        free_single_obj(vm, object);
        object = next_obj;
    }

    free(vm->gray_stack);
}

void mark_array(PankVm *vm, Valarr *arr) {
    for (int i = 0; i < arr->len; i++) {
        mark_val(vm, arr->values[i]);
    }
}

void blacken_obj(PankVm *vm, Obj *obj) {
#ifdef DEBUG_LOG_GC
    // setlocale(LC_CTYPE, "");
    cp_color_print('g', L"[GC] (%p) Blacken : T(%ls) : V(", (void *)obj,
                   get_obj_type_as_string(obj->type));

    print_val(make_obj_val(obj));
    cp_color_println('g', L")");
#endif
    switch (obj->type) {
        case OBJ_NATIVE:
            break;
        case OBJ_STR:
            break;

        case OBJ_UPVAL:
            mark_val(vm, ((ObjUpVal *)obj)->closed);
            break;
        case OBJ_CLOUSRE: {
            ObjClosure *cls = (ObjClosure *)obj;
            mark_obj(vm, (Obj *)cls->func);
            for (int i = 0; i < cls->upv_count; i++) {
                mark_obj(vm, (Obj *)cls->upv[i]);
            }
            break;
        }
        case OBJ_FUNC: {
            ObjFunc *func = (ObjFunc *)obj;
            mark_obj(vm, (Obj *)func->name);
            mark_array(vm, &func->ins.consts);
            break;
        }
        case OBJ_MOD: {
            ObjMod *md = (ObjMod *)obj;

            mark_obj(vm, (Obj *)md->name);
            break;
        }
        case OBJ_ERR: {
            ObjErr *err = (ObjErr *)obj;
            err->obj.is_marked = true;
            // mark_obj((Obj *)err->msg);

            break;
        }
        case OBJ_ARRAY: {
            ObjArray *array = (ObjArray *)obj;
            array->obj.is_marked = true;
            for (int i = 0; i < array->len; i++) {
                mark_val(vm, array->items.values[i]);
            }
        }
    }
}

void mark_roots(PankVm *vm) {
#ifdef DEBUG_LOG_GC
    cp_println(L"[GC] Marking Stack Slots");
#endif
    for (Value *slot = vm->stack; slot < vm->stack_top; slot++) {
        mark_val(vm, *slot);
    }
#ifdef DEBUG_LOG_GC
    cp_println(L"[GC] Finished Stack Slots Marking");
#endif
#ifdef DEBUG_LOG_GC
    cp_println(L"[GC] Marking Frame Closures");
#endif
    for (int i = 0; i < vm->mod_count; i++) {
        Module *mod = &vm->modules[i];
        for (int j = 0; j < mod->frame_count; j++) {
            mark_obj(vm, (Obj *)mod->frames[j].closure);
        }
    }

#ifdef DEBUG_LOG_GC
    cp_println(L"[GC] Finished Marking Frame Closure");
#endif

#ifdef DEBUG_LOG_GC
    cp_println(L"[GC] Marking UpValues");
#endif
    for (int i = 0; i < vm->mod_count; i++) {
        Module *mod = &vm->modules[i];
        for (ObjUpVal *upv = mod->open_upvs; upv != NULL; upv = upv->next) {
            mark_obj(vm, (Obj *)upv);
        }
    }

#ifdef DEBUG_LOG_GC
    cp_println(L"[GC] Finished Marking UpValues");
#endif

#ifdef DEBUG_LOG_GC
    cp_println(L"[GC] Marking Module Globals");
#endif

    // mark_table(&vm.globals);
    // for (int i = 0; i < vm.mod_count; i++) {
    // Module *mod = &vm.modules[i];
    // if (vm.current_mod->globals.len > 0) {

    //  mark_table(&vm.current_mod->globals);
    //}
    //

    for (int i = 0; i < vm->mod_count; i++) {
        Module *mod = &vm->modules[i];

        mark_table(vm, &mod->globals);
    }
#ifdef DEBUG_LOG_GC
    cp_println(L"[GC] Finished Marking Module Globals");
    cp_println(L"[GC] Marking Imported StdLibs");
#endif
    for (int i = 0; i < vm->stdlib_count; i++) {
        StdlibMod *sm = &vm->stdlibs[i];
        if (sm->items.cap > 0) {
            if (sm->items.len > 0) {
                mark_table(vm, &sm->items);
            }
        }
    }
    // cp_color_println('r', L"------------");
#ifdef DEBUG_LOG_GC
    cp_println(L"[GC] Finished Marking Imported StdLibs");
#endif

    //}
    //
#ifdef DEBUG_LOG_GC
    cp_println(L"[GC] Marking Builtins");
#endif
    mark_table(vm, &vm->builtins);

    // mark_table(&vm.current_mod->globals);
#ifdef DEBUG_LOG_GC
    cp_println(L"[GC] Finished Marking Builtins");
#endif
#ifdef DEBUG_LOG_GC
    cp_println(L"[GC] Marking Compiler Roots");
#endif
    if (vm->compiler != NULL) {
        mark_compiler_roots(vm, vm->compiler);
    }
#ifdef DEBUG_LOG_GC
    cp_println(L"[GC] Finished Marking Compiler Roots");
#endif
}

void trace_refs(PankVm *vm) {
    while (vm->gray_count > 0) {
        Obj *obj = vm->gray_stack[--vm->gray_count];
        blacken_obj(vm, obj);
    }
}

void sweep(PankVm *vm) {
    Obj *prev = NULL;
    Obj *obj = vm->objs;
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
                vm->objs = obj;
            }

            free_single_obj(vm, unreached);
        }
    }
}

void mark_val(PankVm *vm, Value val) {
    if (is_obj(val)) {
        mark_obj(vm, get_as_obj(val));
    }
}

void mark_obj(PankVm *vm, Obj *obj) {
    if (obj == NULL) {
        return;
    }

    if (obj->is_marked) {
        return;
    }

#ifdef DEBUG_LOG_GC
    cp_color_print('p', L"[GC] (%p) Marking : T(%ls) : V(", (void *)obj,
                   get_obj_type_as_string(obj->type));
    print_obj(make_obj_val(obj));
    cp_color_println('p', L")");
#endif

    obj->is_marked = true;
    if (vm->gray_cap < vm->gray_count + 1) {
        // wprintf(L"++ growing gstack cap \n");
        vm->gray_cap = GROW_CAP(vm->gray_cap);
        vm->gray_stack =
            (Obj **)realloc(vm->gray_stack, sizeof(Obj *) * vm->gray_cap);
        if (vm->gray_stack == NULL) {
            exit(1);
        }
    }

    vm->gray_stack[vm->gray_count++] = obj;
}

void collect_garbage(PankVm *vm) {
#ifndef NOGC
    // setlocale(LC_CTYPE, "");
#ifdef DEBUG_LOG_GC
    cp_color_println('y', L"[GC] === GC START ====");
    size_t before = vm->bts_allocated;
#endif

#ifdef DEBUG_LOG_GC
    // wprintf(L"gc marking roots -> \n");
    cp_println(L"[GC] -- Marking Roots --");
#endif

    mark_roots(vm);

#ifdef DEBUG_LOG_GC
    // wprintf(L"finished marking roots -> \n");
    cp_println(L"[GC] -- Finished Marking Roots --");

    cp_println(L"[GC] -- Tracing Refs --");
#endif
    trace_refs(vm);

#ifdef DEBUG_LOG_GC
    cp_println(L"[GC] -- Finished Tracing Refs --");
#endif

#ifdef DEBUG_LOG_GC
    cp_println(L"[GC] -- Removing White Strings --");
#endif

    table_remove_white(&vm->strings);

#ifdef DEBUG_LOG_GC
    cp_println(L"[GC] -- Finished Removing White Strings --");
#endif

#ifdef DEBUG_LOG_GC
    cp_println(L"[GC] -- Sweeping --");
#endif

    sweep(vm);

#ifdef DEBUG_LOG_GC
    cp_println(L"[GC] -- Finished Sweeping --");
#endif

    vm->next_gc = vm->bts_allocated * GC_HEAD_GROW_FACT;

#ifdef DEBUG_LOG_GC

    cp_color_println('y', L"[GC] === GC END ====");
    // wprintf(L"-- gc end\n");
    cp_println(L"[GC] collected %zu bytes (from %zu to %zu) next at %zu",
               before - vm->bts_allocated, before, vm->bts_allocated,
               vm->next_gc);
#endif
#endif
}
