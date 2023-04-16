#include <stdbool.h>
#include <stdlib.h>

#include "../include/openfile.h"
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
        free(fp);
        return make_bool(true);
    } else {
        free(fp);
        return make_bool(false);
    }
}

Value _file_read_file_as_string(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, U"readfilestr(path) takes only single argument");
    }

    if (!is_str_obj(args[0])) {
        return make_error(vm, U"argument to readfilestr(a) must be a string");
    }
    ObjString* str = (ObjString*)get_as_obj(args[0]);

    WSrcfile fws = wread_file(str->chars);

    if (fws.errcode != 0) {
        return make_str(vm, U"");
    }

    Value result = make_str(vm, fws.source);
    free(fws.source);
    return result;
}

void push_stdlib_file(PankVm* vm) {
    SL sls[] = {
        msl(U"exists", _file_exists),
        msl(U"readfile", _file_read_file_as_string),
    };

    _push_stdlib(vm, U"file", sls, 2);
}
