#include <stdbool.h>

#include "../include/stdlib.h"
#include "../include/utils.h"

Value _file_exists(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, U"exists(a) only takes single argument");
    }
    if (!is_str_obj(args[0])) {
        return make_error(vm, U"argument to exists(a) must be a string");
    }

    ObjString* str = (ObjString*)get_as_obj(args[0]);
    char* fp = c_to_c(str->chars, str->len);

    if (does_file_exist(fp)) {
        return make_bool(true);
    } else {
        return make_bool(false);
    }
}



void push_stdlib_file(PankVm* vm) {
    SL sls[] = {
        msl(U"exists", _file_exists),
    };

    _push_stdlib(vm, U"file", sls, 1);
}
