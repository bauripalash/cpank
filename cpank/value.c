#include "include/value.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <uchar.h>
#include <wchar.h>

#include "include/bn.h"
#include "include/mem.h"
#include "include/obj.h"
#include "include/utils.h"

#define NUMBER_NAME U"number"
#define NIL_NAME    U"nil"
#define BOOL_NAME   U"bool"

void print_val_type(ValType vt) {
    switch (vt) {
        case V_NIL:
            cp_print(L"NIL");
            break;
        case V_BOOL:
            cp_print(L"BOOL");
            break;
        case V_NUM:
            cp_print(L"NUM");
            break;
        case V_OBJ:
            cp_print(L"OBJ");
            break;
    }
}

char32_t *get_val_type_str(Value val, bool isbn) {
    if (is_num(val)) {
        return isbn ? NUMBER_NAME_BN : NUMBER_NAME;
    } else if (is_nil(val)) {
        return isbn ? NIL_NAME_BN : NIL_NAME;
    } else if (is_bool(val)) {
        return isbn ? BOOL_NAME_BN : BOOL_NAME;
    } else if (is_obj(val)) {
        return get_obj_type_str(val, isbn);
    }

    return U"unknown";
}

char32_t *value_to_string(PankVm *vm, Value val) {
    if (is_num(val)) {
        double num = get_as_number(val);
        if (is_int(num)) {
            int inum = (int)num;
            int length = snprintf(NULL, 0, "%d", inum);
            char *str = (char *)malloc(length + 1);
            snprintf(str, length + 1, "%d", inum);
            char32_t *res = chto16(str);
            free(str);
            return res;
        } else {
            int length = snprintf(NULL, 0, "%lg", num);
            char *str = malloc(length + 1);
            snprintf(str, length + 1, "%lg", num);
            char32_t *res = chto16(str);
            free(str);
            return res;
        }
    } else if (is_nil(val)) {
        return chto16("nil");
    } else if (is_bool(val)) {
        bool v = get_as_bool(val);
        return v ? chto16("true") : chto16("false");
    } else if (is_obj(val)) {
        return obj_to_string(vm, val);
    }
    return chto16("");
}

void init_valarr(Valarr *array) {
    array->values = NULL;
    array->cap = 0;
    array->len = 0;
}

void write_valarr(PankVm *vm, Valarr *array, Value value) {
    if (array->cap < array->len + 1) {
        int old_cap = array->cap;
        array->cap = GROW_CAP(old_cap);
        array->values = GROW_ARR(vm, Value, array->values, old_cap, array->cap);
    }

    array->values[array->len] = value;
    array->len++;
}

void pop_valarr(PankVm *vm, Valarr *array) { array->len--; }

void free_valarr(PankVm *vm, Valarr *array) {
    FREE_ARR(vm, Value, array->values, array->cap);
    init_valarr(array);
}

void print_val(Value val) {
    //  print_val_type(val.type);
#ifdef NAN_BOXING
    if (is_bool(val)) {
 #ifdef IS_WIN
        cp_print(L"%S", get_as_bool(val) ? "true" : "false");
 #else
        cp_print(L"%s", get_as_bool(val) ? "true" : "false");
 #endif
    } else if (is_nil(val)) {
        cp_print(L"nil");
    } else if (is_num(val)) {
        double num = get_as_number(val);
        if (is_int(num)) {
            cp_print(L"%d", (int)num);
        } else {
            cp_print(L"%g", num);
        }
    } else if (is_obj(val)) {
        print_obj(val);
    }
#else
    switch (val.type) {
        case V_NUM:
            cp_print(L"%g", get_as_number(val));
            break;
        case V_BOOL:
 #ifdef IS_WIN
            cp_print(L"%S", get_as_bool(val) ? "true" : "false");
 #else
            cp_print(L"%s", get_as_bool(val) ? "true" : "false");
 #endif
            break;
        case V_NIL:
            cp_print(L"nil");
            break;
        case V_OBJ:
            print_obj(val);
            break;
    }
#endif
}

// bool is_bool(Value val) { return val.type == V_BOOL; }

// bool is_num(Value val) { return val.type == V_NUM; }

// bool is_nil(Value val) { return val.type == V_NIL; }

// bool is_obj(Value val) { return val.type == V_OBJ; }

// bool get_as_bool(Value val) { return val.as.boolean; }

// double get_as_number(Value val) { return val.as.num; }

bool is_falsey(Value val) {
    return is_nil(val) || (is_bool(val) && !get_as_bool(val));
}

bool is_equal(Value left, Value right) {
#ifdef NAN_BOXING
    if (is_num(left) && is_num(right)) {
        return get_as_number(left) == get_as_number(right);
    } else if (is_obj(left) && is_obj(right)) {
        return is_obj_equal(get_as_obj(left), get_as_obj(right));
    } else {
        return left == right;
    }
#else
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
            return is_obj_equal(get_as_obj(left), get_as_obj(right));
            // ObjString *l_string = get_as_string(left);
            // ObjString *r_string = get_as_string(right);
            // return l_string->len == r_string->len &&
            //        wmemcmp(l_string->chars, r_string->chars, l_string->len)
            //        == 0;
        }
        default:
            return false;
    }
#endif
}

/*
Value make_nil(void) {
    Value v;
    v.type = V_NIL;
    v.as.num = 0;
    return v;
}*/

/*Value make_neg(Value value) {
    Value v;
    v.type = value.type;
    v.as.num = -value.as.num;
    return v;
}*/
