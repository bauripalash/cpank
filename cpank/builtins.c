#include <stdbool.h>
#include <time.h>

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
