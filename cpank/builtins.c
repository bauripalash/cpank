/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#include <stdbool.h>
#include <time.h>

#include "include/obj.h"
#include "include/value.h"
#include "include/vm.h"
Value clock_ntv_fn(PankVm *vm, int argc, Value *args) {
    return make_num((double)clock() / CLOCKS_PER_SEC);
}

Value asserteq_ntv_fn(PankVm *vm, int argc, Value *args) {
    if (argc != 2) {
        return make_error(vm, U"asserteq(a, b) takes only 2 arguments");
    }

    if (!is_equal(args[0], args[1])) {
        return make_bool(false);
    }

    return make_bool(true);
}

Value type_ntv_fn(PankVm *vm, int argc, Value *args) {
    if (argc != 1) {
        return make_error(
            vm, U"type(a) builtin function only takes a single argument");
    }

    return make_str(vm, get_val_type_str(args[0], false));
}

Value bn_type_ntv_fn(PankVm *vm, int argc, Value *args) {
    if (argc != 1) {
        return make_error(
            vm, U"type(a) builtin function only takes a single argument");
    }

    return make_str(vm, get_val_type_str(args[0], true));
}

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
        return make_error(vm, U"I dont know how to calculate length of errors");
    }
    return make_num(0);
}
