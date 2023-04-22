#include "../include/stdlib.h"

Value _map_exists(PankVm* vm, int argc, Value* args) {
    check_argc_count("iskey(map , key)", 2, argc);
    return make_num(0);
}

void push_stdlib_map(PankVm* vm) {
    // SL sls[] = {};

    //_push_stdlib(vm, U"map", sls, 4);
}
