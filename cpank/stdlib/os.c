#include <stddef.h>
#include <stdlib.h>
#include <uchar.h>

#include "../include/common.h"
#include "../include/stdlib.h"
#include "../include/utils.h"
#include "../include/value.h"

Value _os_get_name(PankVm* vm, int argc, Value* args) {
    if (argc != 0) {
        return make_error(vm, U"os name() function doesn't take any arguments");
    }
#if defined(IS_WIN)
    return make_str(vm, U"windows");
#elif defined(IS_UNIX)
    return make_str(vm, U"unix");
#else
    return make_str(vm, U"unknown");
#endif
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

Value _os_get_username(PankVm* vm, int argc, Value* args) {
#if defined(IS_WIN)
    char* username = getenv("USERNAME");
#elif defined(IS_UNIX)
    char* username = getenv("USER");
#else
    char* username = NULL;
#endif
    if (username == NULL) {
        return make_str(vm, U"unknown");
    } else {
        char32_t* un = chto16(username);
        Value result = make_str(vm, un);
        free(un);
        return result;
    }
}

Value _os_get_homedir(PankVm* vm, int argc, Value* args) {
    // TODO: Make more fail proof
#ifdef IS_UNIX
    char* hdir = getenv("HOME");
#elif IS_WIN
    char* hdir = getenv("USERPROFILE");
#else
    char* hdir = NULL;
#endif
    if (hdir == NULL) {
        return make_str(vm, U"unknown");
    } else {
        char32_t* hd = chto16(hdir);
        Value result = make_str(vm, hd);
        free(hd);
        return result;
    }
    return make_num(0);
}

void push_stdlib_os(PankVm* vm) {
    SL sls[] = {
        msl(U"name", _os_get_name),
        msl(U"arch", _os_get_arch),
        msl(U"user", _os_get_username),
        msl(U"home", _os_get_homedir),
    };

    _push_stdlib(vm, U"os", sls, 4);
}
