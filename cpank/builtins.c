/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <uchar.h>

#include "include/obj.h"
#include "include/value.h"
#include "include/vm.h"

Value show_ntv_fn(PankVm *vm, int argc, Value *args) {
    bool n = vm->need_buffer;  // Should we write to vm buffer?
    for (int i = 0; i < argc; i++) {
        if (n) {
            char32_t *v = value_to_string(vm, args[i]);

            write_pbuffer(&vm->buffer, "%ls", v);
            free(v);
        } else {
            print_val(args[i]);
        }
    }

    return make_nil;
}

// This builtin function returns the current time in seconds since
// the program started to execute.
Value clock_ntv_fn(PankVm *vm, int argc, Value *args) {
    return make_num((double)clock() / CLOCKS_PER_SEC);
}

// This builtin function creates a new Big Number.
// ( `Deprecated`, use the `new` function from `big` standard library.)
Value bignew_ntv_fn(PankVm *vm, int argc, Value *args) {
    if (argc != 1) {
        return make_error(vm,
                          U"bignew(value) function takes only one argument");
    }
    Value raw_val = args[0];
    if (is_str_obj(raw_val)) {
        char32_t *s = get_as_native_string(args[0]);
        return make_obj_val(new_bignum_with_str(vm, s));
    } else if (is_num(raw_val)) {
        double d = get_as_number(raw_val);
        return make_obj_val(new_bignum_with_double(vm, d));
    } else {
        return make_error(vm,
                          U"bignew(value) function only takes strings and "
                          U"numbers  as argument");
    }
}

// This builtin function returns `true` if the 1st and 2nd argument are equal.
// For now it returns boolean, but in future it would create an error.
Value asserteq_ntv_fn(PankVm *vm, int argc, Value *args) {
    if (argc != 2) {
        return make_error(vm, U"asserteq(a, b) takes only 2 arguments");
    }

    if (!is_equal(args[0], args[1])) {
        return make_bool(false);
    }

    return make_bool(true);
}

// This builtin function returns the type of the first argument.
// The result will be in `English`
Value type_ntv_fn(PankVm *vm, int argc, Value *args) {
    if (argc != 1) {
        return make_error(
            vm, U"type(a) builtin function only takes a single argument");
    }

    return make_str(vm, get_val_type_str(args[0], false));
}

// This builtin function returns the type of the first argument.
// The result will be in `Bengali`
Value bn_type_ntv_fn(PankVm *vm, int argc, Value *args) {
    if (argc != 1) {
        return make_error(
            vm, U"type(a) builtin function only takes a single argument");
    }

    return make_str(vm, get_val_type_str(args[0], true));
}

// This builtin function returns the `length` of the first argument.
// This function only works on a subset of types available in Pankti.
// * String -> The number of characters.
// * Array  -> The number of elements.
// * Map    -> The number of key-value pairs.
//
//
// cppcheck-suppress constParameter
Value len_ntv_fn(PankVm *vm, int argc, Value *args) {
    if (argc != 1) {
        return make_error(
            vm, U"len(a) builtin function takes only a single argument");
    }
    Value a = args[0];
    if (is_num(a)) {
        return make_error(
            vm, U"oops! I don't know how to calculate length of numbers");
    } else if (is_bool(a)) {
        return make_error(
            vm, U"oops! I don't know how to calculate length of bools");
    } else if (is_nil(a)) {
        return make_error(vm,
                          U"oops! I don't know how to calculate length of nil");

    } else if (is_str_obj(a)) {
        return make_num((double)((ObjString *)get_as_obj(a))->len);
    } else if (is_array_obj(a)) {
        return make_num((double)((ObjArray *)get_as_obj(a))->items.len);
    } else if (is_map_obj(a)) {
        return make_num((double)((ObjHashMap *)get_as_obj(a))->count);
    } else if (is_err_obj(a)) {
        return make_error(vm,
                          U"I don't know how to calculate length of errors");
    }
    return make_num(0);
}
