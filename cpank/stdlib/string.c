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
    ObjString* delim = get_as_string(args[1]);
    ObjArray* arr = new_array(vm);
    push(vm, make_obj_val(arr));
    int len = 0;
    char32_t** tokens = split32(rawstr->chars, delim->chars, &len);

    for (int i = 0; i < len; i++) {
        Value o = make_obj_val(copy_string(vm, tokens[i], strlen16(tokens[i])));

        push(vm, o);
        write_valarr(vm, &arr->items, o);
        free(tokens[i]);
    }
    arr->len = arr->items.len;
    free(tokens);
    pop(vm);
    for (int i = 0; i < len; i++) {
        pop(vm);
    }

    return make_obj_val(arr);
}

void push_stdlib_string(PankVm* vm) {
    SL sls[] = {msl(U"split", _str_split_delim)};

    _push_stdlib(vm, U"str", sls, 1);
}
