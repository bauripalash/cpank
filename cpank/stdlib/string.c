#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>

#include "../include/common.h"
#include "../include/mem.h"
#include "../include/stdlib.h"
#include "../include/utils.h"
#include "../include/value.h"
#include "../include/vm.h"



Value _str_split_delim(PankVm* vm, int argc, Value* args) {
    if (argc != 2) {
        return make_error(
            vm, U"string split(a, b) function takes only 2 arguments");
    }
    if (!is_str_obj(args[0]) && !is_str_obj(args[1])) {
        return make_error(vm, U"both arguments must be string for split(a ,b)");
    }
    ObjString* rawstr = get_as_string(args[0]);
    char* char_str = c_to_c(rawstr->chars, rawstr->len);
    ObjString* delim = get_as_string(args[1]);
    char* char_delim = c_to_c(delim->chars, delim->len);

    free(char_str);
    free(char_delim);
    cp_println(L"split(a,b) -> TODO:");

    return make_nil;
}

void push_stdlib_string(PankVm* vm) {
    SL sls[] = {msl(U"split", _str_split_delim)};

    _push_stdlib(vm, U"str", sls, 1);
}
