/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

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
        Value o = make_obj_val(copy_string(vm, tokens[i], strlen32(tokens[i])));

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

// cppcheck-suppress constParameter
Value _str_to_str(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, U"string(a) function takes only single argument");
    }

    Value val = args[0];

    char32_t* val_str = value_to_string(vm, val);
    ObjString* str = copy_string(vm, val_str, strlen32(val_str));
    push(vm, make_obj_val(str));
    free(val_str);

    return pop(vm);
}

static char32_t* convert_char(char* str) {
    char* read_ptr = str;
    char32_t x[strlen(str) + 1];
    int index = 0;
    while (*read_ptr != '\0') {
        if (*read_ptr == '\\' && *(read_ptr + 1) == 'u') {
            char unic[5];

            for (int i = 0; i < 4; i++) {
                unic[i] = *(read_ptr + i + 2);
            }

            unic[4] = '\0';

            int cp = 0;

            for (int i = 0; i < 4; i++) {
                cp = (cp << 4) | (unic[i] - '0');
            }

            x[index] = cp;
            read_ptr += 6;
            index++;
        } else {
            x[index] = *read_ptr;
            //*wptr = *read_ptr;
            read_ptr++;
            index++;
            // wptr++;
        }
    }
    //*wptr = '\0';
    x[index] = '\0';
    char32_t* s = (char32_t*)malloc((index + 1) * sizeof(char32_t));
    copy_c32(s, x, index + 1);
    return s;
    // cp_println(L"->'%ls'" , x);
    // return c_to_c(U"", 0);
}

Value _str_from_uc(PankVm* vm, int argc, Value* args) {
    ObjString* s = get_as_string(args[0]);
    char* x = c_to_c(s->chars, s->len);

    char32_t* y = convert_char(x);
    ObjString* news = take_string(vm, y, strlen32(y));
    free(x);
    return make_obj_val(news);
}

void push_stdlib_string(PankVm* vm) {
    SL sls[] = {
        msl(U"split", _str_split_delim),
        msl(U"string", _str_to_str),
        msl(U"uc", _str_from_uc),
    };

    _push_stdlib(vm, U"str", sls, 3);
}
