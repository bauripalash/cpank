#include "include/obj.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "include/common.h"
#include "include/instruction.h"
#include "include/mem.h"
#include "include/utils.h"
#include "include/value.h"
#include "include/vm.h"

#define ALLOCATE_OBJ(vm, type, otype) (type *)alloc_obj(vm, sizeof(type), otype)

Obj *alloc_obj(PankVm *vm, size_t size, ObjType type) {
    Obj *obj = (Obj *)rallc(vm, NULL, 0, size);
    obj->type = type;
    obj->next = vm->objs;
    obj->is_marked = false;
    vm->objs = obj;
#ifdef DEBUG_LOG_GC
    wprintf(L"%p allocate %zu for %d\n", (void *)obj, size, type);
#endif
    return obj;
}

ObjUpVal *new_up_val(PankVm *vm, Value *val) {
    ObjUpVal *upv = ALLOCATE_OBJ(vm, ObjUpVal, OBJ_UPVAL);
    upv->location = val;
    upv->next = NULL;
    upv->closed = make_nil();
    return upv;
}

ObjType get_obj_type(Value val) { return get_as_obj(val)->type; }

bool is_obj_type(Value val, ObjType ot) {
    return is_obj(val) && get_as_obj(val)->type == ot;
}

bool is_str_obj(Value val) { return is_obj_type(val, OBJ_STR); }
bool is_func_obj(Value val) { return is_obj_type(val, OBJ_FUNC); }
bool is_native_obj(Value val) { return is_obj_type(val, OBJ_NATIVE); }
bool is_closure_obj(Value val) { return is_obj_type(val, OBJ_CLOUSRE); }
bool is_mod_obj(Value val) { return is_obj_type(val, OBJ_MOD); }
bool is_err_obj(Value val) { return is_obj_type(val, OBJ_ERR); }
bool is_array_obj(Value val) { return is_obj_type(val, OBJ_ARRAY); }

ObjString *get_as_string(Value val) { return (ObjString *)get_as_obj(val); }
ObjFunc *get_as_func(Value val) { return (ObjFunc *)get_as_obj(val); }
wchar_t *get_as_native_string(Value val) {
    Obj *o = get_as_obj(val);
    ObjString *os = (ObjString *)(o);
    return os->chars;
}

NativeFn get_as_native(Value val) {
    return ((ObjNative *)get_as_obj(val))->func;
}

ObjClosure *get_as_closure(Value val) { return (ObjClosure *)get_as_obj(val); }

ObjMod *get_as_mod(Value val) { return (ObjMod *)get_as_obj(val); }

ObjErr *get_as_err(Value val) { return (ObjErr *)get_as_obj(val); }
ObjString *allocate_str(PankVm *vm, wchar_t *chars, int len, uint32_t hash) {
    ObjString *string = ALLOCATE_OBJ(vm, ObjString, OBJ_STR);
    string->len = len;
    string->chars = chars;
    string->hash = hash;

    push(vm, make_obj_val(string));

    table_set(vm, &vm->strings, string, make_nil());
    pop(vm);
    return string;
}

ObjArray *get_as_array(Value val) { return (ObjArray *)get_as_obj(val); }

wchar_t *get_obj_type_as_string(ObjType o) {
    switch (o) {
        case OBJ_STR:
            return L"OBJ_STR";
        case OBJ_FUNC:
            return L"OBJ_FUNC";
        case OBJ_CLOUSRE:
            return L"OBJ_CLOUSRE";
        case OBJ_UPVAL:
            return L"OBJ_UPVAL";
        case OBJ_NATIVE:
            return L"OBJ_NATIVE";
        case OBJ_MOD:
            return L"OBJ_MOD";
        case OBJ_ERR:
            return L"OBJ_ERR";
        case OBJ_ARRAY:
            return L"OBJ_ARRAY";
    }

    return L"OBJ_UNKNOWN";
}

uint32_t get_hash(const wchar_t *key, int len) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < len; i++) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

ObjString *copy_string(PankVm *vm, wchar_t *chars, int len) {
    uint32_t hash = get_hash(chars, len);
    ObjString *interned = table_find_str(&vm->strings, chars, len, hash);
    if (interned != NULL) {
        return interned;
    }
    wchar_t *heap_chars = ALLOC(vm, wchar_t, len + 1);

    wmemcpy(heap_chars, chars, len);

    heap_chars[len] = '\0';

    return allocate_str(vm, heap_chars, len, hash);
}

ObjString *take_string(PankVm *vm, wchar_t *chars, int len) {
    uint32_t hash = get_hash(chars, len);

    ObjString *interned = table_find_str(&vm->strings, chars, len, hash);

    if (interned != NULL) {
        FREE_ARR(vm, wchar_t, chars, len + 1);
        return interned;
    }

    return allocate_str(vm, chars, len, hash);
}

void print_function(ObjFunc *func) { cp_print(L"<fn %ls>", func->name->chars); }

void print_obj(Value val) {
    switch (get_obj_type(val)) {
        case OBJ_STR:
            cp_print(L"%ls", get_as_native_string(val));
            // wprintf(L"str");
            break;
        case OBJ_FUNC: {
            ObjFunc *f = get_as_func(val);

            if (f != NULL && f->name != NULL) {
                print_function(get_as_func(val));
            } else {
                cp_print(L"<fn <%p>>", f);
            }
            break;
        }
        case OBJ_NATIVE:
            cp_print(L"<native_fn>");
            break;
        case OBJ_CLOUSRE: {
            ObjClosure *cls = get_as_closure(val);
            if (cls != NULL && cls->func != NULL && cls->func->name != NULL) {
                print_function(cls->func);
            } else {
                cp_print(L"<closure <%p>>", cls);
            }
            break;
        }
        case OBJ_UPVAL:
            cp_print(L"upval");
            break;
        case OBJ_MOD: {
            ObjMod *mod = get_as_mod(val);
            cp_print(L"<mod %ls>", mod->name->chars);
            break;
        }
        case OBJ_ERR: {
            ObjErr *err = get_as_err(val);
            cp_print(L"Error ");
            cp_print(L"%ls", err->errmsg);
            break;
        }
        case OBJ_ARRAY: {
            ObjArray *array = get_as_array(val);
            cp_print(L"[");
            for (int i = 0; i < array->len; i++) {
                Value val = array->items.values[i];
                print_val(val);
                cp_print(L", ");
            }
            cp_print(L"]");
            break;
        }
        default: {
            cp_print(L"OBJ_UNKNOWN");
            break;
        }
    }
}

ObjFunc *new_func(PankVm *vm) {
    ObjFunc *func = ALLOCATE_OBJ(vm, ObjFunc, OBJ_FUNC);
    func->arity = 0;
    func->name = NULL;
    func->up_count = 0;
    init_instruction(&func->ins);
    return func;
}

ObjArray *new_array(PankVm *vm) {
    ObjArray *array = ALLOCATE_OBJ(vm, ObjArray, OBJ_ARRAY);
    array->len = 0;
    init_valarr(&array->items);
    return array;
}

ObjNative *new_native(PankVm *vm, NativeFn fn) {
    ObjNative *native = ALLOCATE_OBJ(vm, ObjNative, OBJ_NATIVE);
    native->func = fn;
    return native;
}

ObjClosure *new_closure(PankVm *vm, ObjFunc *func, uint32_t global_owner) {
    ObjUpVal **upvs = ALLOC(vm, ObjUpVal *, func->up_count);
    for (int i = 0; i < func->up_count; i++) {
        upvs[i] = NULL;
    }
    ObjClosure *cls = ALLOCATE_OBJ(vm, ObjClosure, OBJ_CLOUSRE);
    cls->func = func;
    cls->upv = upvs;
    cls->global_owner = global_owner;
    cls->globals = &get_mod_by_hash(vm, global_owner)->globals;
    cls->upv_count = func->up_count;
    return cls;
}

ObjMod *new_mod(PankVm *vm, wchar_t *name) {
    ObjMod *mod = ALLOCATE_OBJ(vm, ObjMod, OBJ_MOD);
    mod->name = copy_string(vm, name, wcslen(name));
    return mod;
}

ObjErr *new_err_obj(PankVm *vm, wchar_t *errmsg) {
    ObjErr *err = ALLOCATE_OBJ(vm, ObjErr, OBJ_ERR);
    err->errmsg = malloc(sizeof(wchar_t) * (wcslen(errmsg) + 1));
    wmemset(err->errmsg, 0, wcslen(errmsg) + 1);

    wmemcpy(err->errmsg, errmsg, wcslen(errmsg) + 1);
    err->len = wcslen(errmsg) + 1;
    // copy_string(errmsg, wcslen(errmsg));
    return err;
}

Value make_error(PankVm *vm, wchar_t *errmsg) {
    return make_obj_val(new_err_obj(vm, errmsg));
}
