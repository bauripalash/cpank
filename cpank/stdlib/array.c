#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <uchar.h>

#include "../include/common.h"
#include "../include/mem.h"
#include "../include/stdlib.h"
#include "../include/utils.h"
#include "../include/value.h"

Value _arr_pop(PankVm* vm, int argc, Value* args) {
    if (argc != 1) {
        return make_error(vm, U"array pop(a) only takes 1 argument!");
    }

    if (!is_array_obj(args[0])) {
        return make_error(vm, U"pop(a) only works on arrays");
    }

    ObjArray* a = get_as_array(args[0]);
    a->items.len--;
    a->len--;

    return make_nil;
}

Value _arr_push(PankVm* vm, int argc, Value* args) {
    if (argc != 2) {
        return make_error(vm, U"array push(a,b) takes only 2 arguments!");
    }

    if (!is_array_obj(args[0])) {
        return make_error(
            vm,
            U"first argument must be the array you want to push the value in");
    }

    ObjArray* a = get_as_array(args[0]);
    write_valarr(vm, &a->items, args[1]);
    a->len++;

    return make_nil;
}

Value _arr_join_two_arr(PankVm * vm , int argc , Value * args){
    if (argc != 2) {
        return make_error(vm, U"array join(a,b) takes only 2 arguments!");
    }

if (!is_array_obj(args[0]) || !is_array_obj(args[1])) {
        return make_error(
            vm,
            U"two argument must be the array you want to join");
    }

    ObjArray* one = get_as_array(args[0]);
    ObjArray* two = get_as_array(args[1]);
    ObjArray * result = new_array(vm);
int ln = 0;
    for (int i = 0; i < one->len; i++) {
        write_valarr(vm, &result->items, one->items.values[i]); 
        ln++;
    }
    for (int i = 0; i < two->len; i++) {
        write_valarr(vm, &result->items,two->items.values[i]); 
        ln++;
    }
    result->len = ln;

    return make_obj_val(result);
}

void push_stdlib_array(PankVm* vm) {
    SL sls[] = {
        msl(U"pop", _arr_pop),
        msl(U"push", _arr_push),
    };

    _push_stdlib(vm, U"array", sls, 2);
}
