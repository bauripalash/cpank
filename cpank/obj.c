/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#include "include/obj.h"

#include <gmp.h>
#include <locale.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>
#include <wchar.h>

#include "ext/xxhash/xxhash.h"
#include "include/bn.h"
#include "include/common.h"
#include "include/helper/comp.h"
#include "include/helper/os.h"
#include "include/instruction.h"
#include "include/lexer.h"
#include "include/mem.h"
#include "include/openfile.h"
#include "include/utils.h"
#include "include/value.h"
#include "include/vm.h"

#define MOD_NAME     U"module"
#define STR_NAME     U"string"
#define ERR_NAME     U"error"
#define FUNC_NAME    U"function"
#define HMAP_NAME    U"hashmap"
#define CLOSURE_NAME U"closure"
#define ARRAY_NAME   U"array"
#define NATIVE_NAME  U"native"
#define UPVAL_NAME   U"upvalue"
#define BIGNUM_NAME  U"bignum"

char32_t *get_obj_type_str(Value val, bool isbn) {
    ObjType tp = get_obj_type(val);
    switch (tp) {
        case OBJ_MOD:
            return isbn ? MOD_NAME_BN : MOD_NAME;
        case OBJ_STR:
            return isbn ? STR_NAME_BN : STR_NAME;
        case OBJ_ERR:
            return isbn ? ERR_NAME_BN : ERR_NAME;
        case OBJ_FUNC:
            return isbn ? FUNC_NAME_BN : FUNC_NAME;
        case OBJ_HMAP:
            return isbn ? HMAP_NAME_BN : HMAP_NAME;
        case OBJ_CLOUSRE:
            return isbn ? CLOSURE_NAME_BN : CLOSURE_NAME;
        case OBJ_NATIVE:
            return isbn ? NATIVE_NAME_BN : NATIVE_NAME;
        case OBJ_ARRAY:
            return isbn ? ARRAY_NAME_BN : ARRAY_NAME;
        case OBJ_UPVAL:
            return isbn ? UPVAL_NAME_BN : UPVAL_NAME;
        case OBJ_BIGNUM:
            return BIGNUM_NAME;  // TODO : Bengali name
    }

    return U"OBJ_UNKNOWN";
}

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
    upv->closed = make_nil;
    return upv;
}

ObjType get_obj_type(Value val) { return get_as_obj(val)->type; }

bool is_obj_type(Value val, ObjType ot) {
    return is_obj(val) && get_as_obj(val)->type == ot;
}

bool is_str_obj(Value val) { return is_obj_type(val, OBJ_STR); }
bool is_bignum_obj(Value value) { return is_obj_type(value, OBJ_BIGNUM); }

// cppcheck-suppress unusedFunction
bool is_func_obj(Value val) { return is_obj_type(val, OBJ_FUNC); }

// cppcheck-suppress unusedFunction
bool is_native_obj(Value val) { return is_obj_type(val, OBJ_NATIVE); }

// cppcheck-suppress unusedFunction
bool is_closure_obj(Value val) { return is_obj_type(val, OBJ_CLOUSRE); }

bool is_map_obj(Value val) { return is_obj_type(val, OBJ_HMAP); }
bool is_mod_obj(Value val) { return is_obj_type(val, OBJ_MOD); }
bool is_err_obj(Value val) { return is_obj_type(val, OBJ_ERR); }
bool is_array_obj(Value val) { return is_obj_type(val, OBJ_ARRAY); }

ObjString *get_as_string(Value val) { return (ObjString *)get_as_obj(val); }
ObjFunc *get_as_func(Value val) { return (ObjFunc *)get_as_obj(val); }
char32_t *get_as_native_string(Value val) {
    Obj *o = get_as_obj(val);
    ObjString *os = (ObjString *)(o);
    return os->chars;
}
char32_t *duplicate_string(ObjString *str) {
    char32_t *dupstr = (char32_t *)calloc((str->len + 1), sizeof(char32_t));
    copy_c32(dupstr, str->chars, str->len + 1);
    return dupstr;
}

NativeFn get_as_native(Value val) {
    return ((ObjNative *)get_as_obj(val))->func;
}

ObjClosure *get_as_closure(Value val) { return (ObjClosure *)get_as_obj(val); }

ObjMod *get_as_mod(Value val) { return (ObjMod *)get_as_obj(val); }

ObjHashMap *get_as_hmap(Value val) { return (ObjHashMap *)get_as_obj(val); }
ObjErr *get_as_err(Value val) { return (ObjErr *)get_as_obj(val); }

ObjBigNum *get_as_bignum(Value val) { return (ObjBigNum *)get_as_obj(val); }

ObjString *allocate_str(PankVm *vm, char32_t *chars, int len, uint64_t hash) {
    ObjString *string = ALLOCATE_OBJ(vm, ObjString, OBJ_STR);
    string->len = len;
    string->chars = chars;
    string->hash = hash;

    push(vm, make_obj_val(string));

    table_set(vm, &vm->strings, string, make_nil);
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
        case OBJ_HMAP:
            return L"OBJ_HMAP";
        case OBJ_BIGNUM:
            return L"OBJ_BIGNUM";
    }

    return L"OBJ_UNKNOWN";
}

uint64_t get_hash(const char32_t *key, int len) {
    /*uint32_t hash = 2166136261u;
    for (int i = 0; i < len; i++) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;*/

    XXH64_hash_t hash = XXH3_64bits(key, sizeof(char32_t) * len);
    return (uint64_t)hash;
}

ObjString *copy_string(PankVm *vm, char32_t *chars, int len) {
    uint64_t hash = get_hash(chars, len);
    ObjString *interned = table_find_str(&vm->strings, chars, len, hash);
    if (interned != NULL) {
        return interned;
    }
    char32_t *heap_chars = ALLOC(vm, char32_t, len + 1);
    // char32_t * heap_chars = (char32_t*)malloc(sizeof(char32_t) * (len + 1));

    // wmemcpy(heap_chars, chars, len);
    // copy_c32(heap_chars, chars, len);
    memcpy(heap_chars, chars, (sizeof(char32_t)) * (len));

    heap_chars[len] = '\0';

    // cp_println(L"COPY_STRING -> %S -> %S - >%d\n" , heap_chars , chars ,
    // len);
    return allocate_str(vm, heap_chars, len, hash);
}

ObjString *take_string(PankVm *vm, char32_t *chars, int len) {
    uint32_t hash = get_hash(chars, len);

    ObjString *interned = table_find_str(&vm->strings, chars, len, hash);

    if (interned != NULL) {
        FREE_ARR(vm, char32_t, chars, len + 1);
        return interned;
    }

    return allocate_str(vm, chars, len, hash);
}

void print_function(ObjFunc *func) {
#ifdef PANK_OS_WIN
    char *fname_str = c_to_c(func->name->chars, 0);
    cp_print(L"<fn %S >", fname_str);
    free(fname_str);
#else
    cp_print(L"<fn %ls>", func->name->chars);
#endif
}

bool is_obj_equal(Obj *a, Obj *b) {
    if (a->type != b->type) {
        return false;
    }
    switch (a->type) {
        case OBJ_STR:
            return ((ObjString *)a)->hash == ((ObjString *)b)->hash;
        default:
            return false;
    }
}

char32_t *obj_to_string(PankVm *vm, Value val) {
    switch (get_obj_type(val)) {
        // TODO: BigNum
        case OBJ_STR:
            return duplicate_string((ObjString *)get_as_obj(val));

        case OBJ_ERR: {
            ObjErr *er = get_as_err(val);
            return char_to_32(er->errmsg);
        }

        case OBJ_UPVAL:
            return char_to_32("upvalue");
        case OBJ_NATIVE: {
            ObjNative *n = (ObjNative *)get_as_obj(val);

            if (n->name != NULL && n->name_len > 0) {
                setlocale(LC_ALL, "bn_IN.UTF-8");
                char *nm = c_to_c(n->name, n->name_len + 1);
                int len = snprintf(NULL, 0, "<native fn %s>", nm);
                char *cres = (char *)malloc(sizeof(char) * (len + 1));
                snprintf(cres, len + 1, "<native fn %s>", nm);
                char32_t *result = char_to_32(cres);
                free(cres);
                free(nm);
                return result;

            } else {
                return char_to_32("<native func>");
            }
        }
        case OBJ_FUNC: {
            ObjFunc *f = (ObjFunc *)get_as_obj(val);
            if (f->name != NULL) {
                setlocale(LC_ALL, "bn_IN.UTF-8");
                char *nm = c_to_c(f->name->chars, f->name->len);
                int len = snprintf(NULL, 0, "<func %s>", nm);
                char *cres = (char *)malloc(sizeof(char) * (len + 1));
                snprintf(cres, len + 1, "<func %s>", nm);
                char32_t *result = char_to_32(cres);
                free(cres);
                free(nm);
                return result;
            } else {
                return char_to_32("<func>");
            }
        }
        case OBJ_CLOUSRE: {
            ObjClosure *cl = (ObjClosure *)get_as_obj(val);
            ObjFunc *f = cl->func;
            if (f->name != NULL) {
                setlocale(LC_ALL, "bn_IN.UTF-8");
                char *nm = c_to_c(f->name->chars, f->name->len);
                int len = snprintf(NULL, 0, "<func %s>", nm);
                char *cres = (char *)malloc(sizeof(char) * (len + 1));
                snprintf(cres, len + 1, "<func %s>", nm);
                char32_t *result = char_to_32(cres);
                free(cres);
                free(nm);
                return result;
            } else {
                return char_to_32("<func>");
            }
        }
        case OBJ_MOD: {
            ObjMod *mod = (ObjMod *)get_as_obj(val);
            char *nm = c_to_c(mod->name->chars, mod->name->len);
            int len = snprintf(NULL, 0, "<module %s>", nm);
            char *cres = (char *)malloc(sizeof(char) * (len + 1));
            snprintf(cres, len + 1, "<module %s>", nm);
            char32_t *result = char_to_32(cres);
            free(cres);
            free(nm);
            return result;
        }
        case OBJ_HMAP: {
            setlocale(LC_ALL, "bn_IN.UTF-8");
            ObjHashMap *hmap = get_as_hmap(val);
            char32_t *sep = U": ";
            int sep_len = 2;
            char32_t *comma = U", ";
            int comma_len = 2;
            char32_t lbrack[] = U"{ ";
            int lbrack_len = 2;
            char32_t rbrack[] = U"}";
            int rbrack_len = 1;
            int total_len = 0;
            char32_t **result =
                (char32_t **)calloc(hmap->count, sizeof(char32_t *));

            int len = 0;
            for (int i = 0; i < hmap->cap; i++) {
                HmapItem *hitem = &hmap->items[i];
                if (hitem != NULL && !is_nil(hitem->key)) {
                    int index = len;
                    char32_t *k_str = value_to_string(vm, hitem->key);
                    int klen = strlen32(k_str);
                    char32_t *v_str = value_to_string(vm, hitem->val);
                    int vlen = strlen32(v_str);
                    int needlen = klen + sep_len + vlen + comma_len;
                    result[index] =
                        (char32_t *)malloc((needlen + 1) * sizeof(char32_t));
                    char32_t *ptr = result[index];
                    copy_c32(ptr, k_str, klen);
                    ptr += klen;
                    copy_c32(ptr, sep, sep_len);

                    ptr += sep_len;

                    copy_c32(ptr, v_str, vlen);
                    ptr += vlen;
                    copy_c32(ptr, comma, comma_len);
                    ptr += comma_len;

                    *ptr = U'\0';
                    free(k_str);
                    free(v_str);
                    total_len += needlen;
                    len++;
                }
            }

            char32_t *res = (char32_t *)calloc(
                total_len + lbrack_len + rbrack_len + 1, sizeof(char32_t));

            char32_t *ptr = res;
            copy_c32(ptr, lbrack, lbrack_len);
            ptr += lbrack_len;

            for (int i = 0; i < len; i++) {
                int rlen = strlen32(result[i]);
                copy_c32(ptr, result[i], rlen);
                free(result[i]);
                ptr += rlen;
            }

            copy_c32(ptr, rbrack, rbrack_len);
            ptr += rbrack_len;
            *ptr = U'\0';
            free(result);
            char32_t *rr = res;
            return rr;
        }
        case OBJ_BIGNUM: {
            ObjBigNum *bn = get_as_bignum(val);
            char *str;
            if (bn->isfloat) {
                str = gmp_float_to_str(bn->as.fval);

            } else {
                str = gmp_int_to_str(bn->as.ival);
            }
            if (str == NULL) {
                return U"";
            }

            char32_t *str32 = char_to_32(str);
            if (str32 == NULL) {
                free(str);
                return U"";
            }
            free(str);
            return str32;
        }
        case OBJ_ARRAY: {
            ObjArray *arr = get_as_array(val);
            char32_t *comma = U", ";
            int comma_len = 2;
            char32_t lbrack[] = U"[ ";
            int lbrack_len = 2;
            char32_t rbrack[] = U"]";
            int rbrack_len = 1;
            setlocale(LC_ALL, "bn_IN.UTF-8");
            char32_t **result =
                (char32_t **)calloc(arr->items.len, sizeof(char32_t *));
            int total_len = 0;
            for (int i = 0; i < arr->items.len; i++) {
                result[i] = value_to_string(vm, arr->items.values[i]);
                int ilen = strlen32(result[i]);

                total_len += ilen;
            }

            char32_t *res =
                (char32_t *)calloc((total_len + lbrack_len + rbrack_len +
                                    (total_len * comma_len) + 1),
                                   sizeof(char32_t));
            char32_t *ptr = res;
            copy_c32(ptr, lbrack, lbrack_len);
            ptr += lbrack_len;
            for (int i = 0; i < arr->items.len; i++) {
                int rlen = strlen32(result[i]);
                copy_c32(ptr, result[i], rlen);
                ptr += rlen;
                copy_c32(ptr, comma, comma_len);
                ptr += comma_len;
                free(result[i]);
            }
            copy_c32(ptr, rbrack, rbrack_len);
            ptr += rbrack_len;
            *ptr = U'\0';
            free(result);
            return res;
        }
    }
    return char_to_32("unknown");
}

void print_obj(Value val) {
    switch (get_obj_type(val)) {
        case OBJ_STR: {
#ifdef PANK_OS_WIN
            char *obj_str = c_to_c(get_as_native_string(val), 0);
            cp_print(L"%S", obj_str);
            free(obj_str);
#else
            cp_print(L"%ls", get_as_native_string(val));
#endif
            // wprintf(L"str");
            break;
        }
        case OBJ_FUNC: {
            ObjFunc *f = get_as_func(val);

            if (f != NULL && f->name != NULL) {
                print_function(get_as_func(val));
            } else {
                cp_print(L"<fn <%p>>", f);
            }
            break;
        }
        case OBJ_NATIVE: {
            ObjNative *native = (ObjNative *)get_as_obj(val);
            if (native->name != NULL) {
#ifdef PANK_OS_WIN
                char *nn = c_to_c(native->name, 0);
                cp_print(L"<native func '%S'>", nn);
                free(nn);
#else
                cp_print(L"<native func '%ls'>", native->name);
#endif

            } else {
                cp_print(L"<native func >");
            }
            break;
        }
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
#ifdef PANK_OS_WIN

            char *mod_str = c_to_c(mod->name->chars, 0);
            cp_print(L"< mod %S >", mod_str);
            free(mod_str);
#else
            cp_print(L"<mod %ls>", mod->name->chars);
#endif
            break;
        }
        case OBJ_ERR: {
            ObjErr *err = get_as_err(val);
            cp_print(L"Error ");
#ifdef PANK_OS_WIN
            char *emsg = err->errmsg;  // c_to_c(err->errmsg, 0);
            cp_print(L"%S", emsg);
            // free(emsg);
#else
            cp_print(L"%ls", err->errmsg);
#endif
            break;
        }
        case OBJ_ARRAY: {
            ObjArray *array = get_as_array(val);
            int ln = array->items.len;

            cp_print(L"[ ");
            for (int i = 0; i < ln; i++) {
                Value newval = array->items.values[i];
                print_val(newval);
                if (i + 1 != ln) {
                    cp_print(L", ");
                }
            }
            cp_print(L" ]");
            break;
        }
        case OBJ_HMAP: {
            ObjHashMap *map = get_as_hmap(val);
            int mcap = map->cap;
            int mlen = map->count;
            int cur = 0;
            cp_print(L"{ ");
            for (int i = 0; i < mcap; i++) {
                HmapItem *item = &map->items[i];
                if (item != NULL && !is_nil(item->key)) {
                    print_val(item->key);
                    cp_print(L": ");
                    print_val(item->val);
                    if (cur + 1 != mlen) {
                        cp_print(L" ,");
                    }
                    cur++;
                }
            }
            cp_print(L" }");
            break;
        }
        case OBJ_BIGNUM: {
            ObjBigNum *bn = get_as_bignum(val);

            if (bn->isfloat) {
                char *str = gmp_float_to_str(bn->as.fval);
                if (str == NULL) {
                    cp_print(L"");
                    break;
                } else {
                    cp_print(L"%s", str);

                    // mpfr_free_str(str);
                    free(str);
                }
            } else {
                char *str = gmp_int_to_str(bn->as.ival);
                if (str == NULL) {
                    cp_print(L"");
                    break;
                } else {
                    cp_print(L"%s", str);
                    free(str);
                }
            }

            // cp_print(L"%s -> %d", str, num_zeros);
            // free(str);
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

ObjNative *new_native(PankVm *vm, NativeFn fn, char32_t *name) {
    ObjNative *native = ALLOCATE_OBJ(vm, ObjNative, OBJ_NATIVE);
    native->func = fn;
    size_t namelen = strlen32(name) + 1;
    native->name = (char32_t *)malloc(sizeof(char32_t) * namelen);
    memset(native->name, 0, namelen);
    // memcpy(native->name, name, namelen);
    copy_c32(native->name, name, namelen);
    native->name_len = namelen - 1;
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

ObjMod *new_mod(PankVm *vm, char32_t *name) {
    ObjMod *mod = ALLOCATE_OBJ(vm, ObjMod, OBJ_MOD);
    mod->name = NULL;
    push(vm, make_obj_val(mod));
    mod->name = copy_string(vm, name, strlen32(name));
    // cp_color_println('b' , L"modname -> %ls" , mod->name->chars);
    pop(vm);
    // print_stack(vm);
    return mod;
}

ObjBigNum *new_bignum(PankVm *vm) {
    ObjBigNum *bn = ALLOCATE_OBJ(vm, ObjBigNum, OBJ_BIGNUM);
    push(vm, make_obj_val(bn));
    bn->marker = false;
    bn->isfloat = false;

    mpz_init(bn->as.ival);
    pop(vm);
    return bn;
}

ObjBigNum *new_bignum_float(PankVm *vm) {
    ObjBigNum *bn = ALLOCATE_OBJ(vm, ObjBigNum, OBJ_BIGNUM);
    push(vm, make_obj_val(bn));
    bn->marker = false;
    bn->isfloat = true;

    // mpf_init(bn->as.fval);
    mpfr_init2(bn->as.fval, BIGFLOAT_MINPREC);
    pop(vm);
    return bn;
}

ObjBigNum *new_bignum_with_double(PankVm *vm, double value) {
    ObjBigNum *bn = new_bignum(vm);
    push(vm, make_obj_val(bn));
    if (is_int(value)) {
        mpz_set_d(bn->as.ival, value);
    } else {
        bn->isfloat = true;
        mpz_clear(bn->as.ival);
        mpfr_init2(bn->as.fval, BIGFLOAT_MINPREC);
        mpfr_set_d(bn->as.fval, value, BIGFLOAT_ROUND);
    }
    pop(vm);
    return bn;
}

ObjBigNum *new_bignum_with_mpf(PankVm *vm, mpfr_t value) {
    ObjBigNum *bn = new_bignum_float(vm);
    push(vm, make_obj_val(bn));
    mpfr_set(bn->as.fval, value, BIGFLOAT_ROUND);
    pop(vm);
    return bn;
}

ObjBigNum *new_bignum_with_mpz(PankVm *vm, mpz_t value) {
    ObjBigNum *bn = new_bignum(vm);
    push(vm, make_obj_val(bn));
    mpz_set(bn->as.ival, value);
    pop(vm);
    return bn;
}

ObjBigNum *new_bignum_with_str(PankVm *vm, char32_t *value) {
    ObjBigNum *bn = new_bignum(vm);
    push(vm, make_obj_val(bn));
    char *str = c32_to_char(value, strlen32(value));
    bool isf = false;
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '.') {
            isf = true;
        }
    }

    if (isf) {
        mpz_clear(bn->as.ival);
        mpfr_init2(bn->as.fval, BIGFLOAT_MINPREC);
        // mpf_init(bn->as.fval);
        mpfr_set_str(bn->as.fval, str, 10, BIGFLOAT_ROUND);
        bn->isfloat = true;
    } else {
        mpz_set_str(bn->as.ival, str, 10);
    }
    // mpf_set_str(bn->value , str , 10);
    free(str);
    pop(vm);
    return bn;
}

ObjErr *new_err_obj(PankVm *vm, char32_t *errmsg) {
    ObjErr *err = ALLOCATE_OBJ(vm, ObjErr, OBJ_ERR);

    err->errmsg = c_to_c(
        errmsg,
        0);  //(char32_t *)malloc(sizeof(char32_t) * (strlen32(errmsg) + 1));
    // wmemset(err->errmsg, 0, wcslen(errmsg) + 1);
    //    memset(err->errmsg, 0, strlen32(errmsg) + 1);

    // wmemcpy(err->errmsg, errmsg, wcslen(errmsg) + 1);
    //    memcpy(err->errmsg, errmsg, strlen32(errmsg) + 1);
    err->len = strlen(err->errmsg) + 1;
    // copy_string(errmsg, wcslen(errmsg));
    return err;
}

Value make_error(PankVm *vm, char32_t *errmsg) {
    return make_obj_val(new_err_obj(vm, errmsg));
}

Value make_argc_want_mismatch_error(PankVm *vm, char *funname, int want,
                                    int got) {
    ObjErr *err = ALLOCATE_OBJ(vm, ObjErr, OBJ_ERR);
    err->errmsg = NULL;
    push(vm, make_obj_val(err));
    int len = snprintf(NULL, 0,
                       "function '%s' wants %d argument(s) but got %d instead!",
                       funname, want, got);
    err->errmsg = (char *)calloc(len + 1, sizeof(char));
    snprintf(err->errmsg, len + 1,
             "function '%s' wants %d argument(s) but got %d instead!", funname,
             want, got);
    err->len = len;
    return pop(vm);
}

Value make_bn_argc_want_mismatch_error(PankVm *vm, char *funname, int want,
                                       int got) {
    ObjErr *err = ALLOCATE_OBJ(vm, ObjErr, OBJ_ERR);
    err->errmsg = NULL;
    push(vm, make_obj_val(err));
    int len = snprintf(
        NULL, 0, "কাজ '%s' এর জন্য %d টি চলরাশি দরকার কিন্তু %d টি পাওয়া গেল!",
        funname, want, got);
    err->errmsg = (char *)calloc(len + 1, sizeof(char));
    snprintf(err->errmsg, len + 1,
             "কাজ '%s' এর জন্য %d টি চলরাশি দরকার কিন্তু %d টি পাওয়া গেল!",
             funname, want, got);
    err->len = len;
    return pop(vm);
}

ObjHashMap *new_hmap(PankVm *vm) {
    ObjHashMap *map = ALLOCATE_OBJ(vm, ObjHashMap, OBJ_HMAP);
    map->cap = 0;
    map->count = 0;
    map->items = NULL;
    return map;
}

static uint64_t get_obj_hash(Obj *obj) {
    if (obj->type != OBJ_STR) {
        return 0;
    }

    return ((ObjString *)obj)->hash;
}

#ifdef NAN_BOXING
static uint64_t hash_uint(uint64_t value) {
    // uint64_t x = value;
    uint32_t hash = 0;
    hash = ~value + (value << 18);
    hash = hash ^ (hash >> 31);
    hash = hash * 21;
    hash = hash ^ (hash >> 11);
    hash = hash + (hash << 6);
    hash = hash ^ (hash >> 22);
    return (uint64_t)(hash & 0x3fffffff);
}

#else
static uint32_t hash_uint(uint32_t value) {
    uint32_t hash = 0;
    hash = ~value + (value << 18);
    hash = hash ^ (hash >> 31);
    hash = hash * 21;
    hash = hash ^ (hash >> 11);
    hash = hash + (hash << 6);
    hash = hash ^ (hash >> 22);
    return (uint32_t)(hash & 0x3fffffff);
}
#endif

static uint32_t get_value_hash(Value value) {
    // VM Must check for invalid values!
    //
#ifdef NAN_BOXING
    if (is_obj(value)) {
        return get_obj_hash(get_as_obj(value));
    }
    return hash_uint(value);
#else
    if (is_bool(value)) {
        return hash_uint((uint32_t)get_as_bool(value));
    } else if (is_num(value)) {
        double n = get_as_number(value);
        if (n < 0 || ceil(n) != n) {
            return 0;
        } else {
            return hash_uint((uint32_t)n);
        }
    } else if (is_obj(value)) {
        return get_obj_hash(get_as_obj(value));
    } else if (is_nil(value)) {
        return 0;
    }
    return 0;
#endif
}

bool is_valid_hashmap_key(Value val) {
    if (is_nil(val)) {
        return false;
    } else if (is_obj(val)) {
        Obj *o = get_as_obj(val);
        if (o->type != OBJ_STR) {
            return false;
        }
    }

    return true;
}

static HmapItem *find_value_in_hmap(HmapItem *entries, int cap, Value key) {
    uint64_t key_hash = get_value_hash(key);
    uint8_t index = key_hash & (cap - 1);
    HmapItem *tombstone = NULL;
    for (;;) {
        HmapItem *entry = &entries[index];
        if (is_nil(entry->key)) {
            if (is_nil(entry->val)) {
                return tombstone != NULL ? tombstone : entry;
            } else {
                if (tombstone == NULL) {
                    tombstone = entry;
                }
            }

        } else if (is_equal(entry->key, key)) {
            return entry;
        }

        index = (index + 1) & (cap - 1);
    }
}

bool hmap_get(ObjHashMap *map, Value key, Value *val) {
    if (map->count == 0) {
        return false;
    }
    HmapItem *item = find_value_in_hmap(map->items, map->cap, key);
    if (item == NULL || is_nil(item->key)) {
        return false;
    }
    *val = item->val;

    return true;
}

void adjust_map_cap(PankVm *vm, ObjHashMap *map, int cap) {
    HmapItem *items = ALLOC(vm, HmapItem, cap);
    for (int i = 0; i < cap; i++) {
        items[i].key = make_nil;
        items[i].val = make_nil;
    }

    map->count = 0;
    HmapItem *olditems = map->items;
    int oldcap = map->cap;

    map->count = 0;
    map->items = items;
    map->cap = cap;

    for (int i = 0; i < oldcap; i++) {
        HmapItem *item = &olditems[i];
        if (is_nil(item->key)) {
            continue;
        }
        hmap_set(vm, map, item->key, item->val);
    }

    FREE_ARR(vm, HmapItem, olditems, oldcap);
}

bool hmap_set(PankVm *vm, ObjHashMap *map, Value key, Value val) {
    if (map->count + 1 > map->cap * 0.75) {
        int cap = GROW_CAP(map->cap);
        adjust_map_cap(vm, map, cap);
    }

    uint32_t index = get_value_hash(key) & (map->cap - 1);

    HmapItem *buckt;
    bool is_new_key = false;
    HmapItem item;
    item.key = key;
    item.val = val;
    item.hash = get_value_hash(key);

    for (;;) {
        buckt = &map->items[index];
        if (is_nil(buckt->key)) {
            is_new_key = true;
            break;
        } else {
            if (is_equal(key, buckt->key)) {
                break;
            }
        }

        index = (index + 1) & (map->cap - 1);
    }

    /*cp_print(L"H_1 -> KEY ->> ");
    print_val(item.key);
    cp_print(L" | VAL ->> ");
    print_val(item.val);
    cp_println(L"");*/

    *buckt = item;
    if (is_new_key) {
        map->count++;
    }

    return is_new_key;
}

Value make_str(PankVm *vm, char32_t *str) {
    return make_obj_val(copy_string(vm, str, strlen32(str)));
}
