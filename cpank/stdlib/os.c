#include <stddef.h>
#include <uchar.h>

#include "../include/common.h"
#include "../include/stdlib.h"
#include "../include/utils.h"
#include "../include/value.h"

Value _os_get_name(PankVm* vm, int argc, Value* args) {
    if (argc != 0) {
        return make_error(vm, U"os name() function doesn't take any arguments");
    }

#ifdef IS_WIN

    return make_str(vm, U"windows");

#else

#endif
    return make_str(vm, U"linux");
}

Value _os_get_arch(PankVm* vm, int argc, Value* args) {
    if (sizeof(void*) == (size_t)8) {
        return make_str(vm, U"64");
    } else if (sizeof(void*) == (size_t)4) {
        return make_str(vm, U"32");
    } else {
        return make_str(vm, U"0");
    }
}

void push_stdlib_os(PankVm* vm) {
    SL sls[] = {
        msl(U"name", _os_get_name),
        msl(U"arch", _os_get_arch),
    };

    _push_stdlib(vm, U"os", sls, 2);
}
