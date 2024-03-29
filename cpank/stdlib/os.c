/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#include "../include/helper/os.h"

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
#if defined(PANK_OS_LINUX)
    return make_str(vm, U"linux");
#elif defined(PANK_OS_WIN)
    return make_str(vm, U"windows");
#elif defined(PANK_OS_UNIX)
    return make_str(vm, U"unix");
#elif defined(PANK_OS_ANDROID)
    return make_str(vm, U"android");
#elif defined(PANK_OS_MACOS)
    return make_str(vm, U"macos");
#else
    return make_str(vm, U"unknown");
#endif
}

Value _os_get_arch(PankVm* vm, int argc, Value* args) {
#if defined(PANK_ARCH_64)
    return make_str(vm, U"64");
#elif defined(PANK_ARCH_32)
    return make_str(vm, U"32");
#elif defined(PANK_ARCH_ARM)
    return make_str(vm, U"arm");
#else
    return make_str(vm, U"unknown");
#endif
}

Value _os_get_username(PankVm* vm, int argc, Value* args) {
#if defined(PANK_OS_WIN)
    char* username = getenv("USERNAME");
#elif defined(PANK_OS_LINUX) || defined(PANK_OS_UNIX) || defined(PANK_OS_MACOS)
    char* username = getenv("USER");
#else
    char* username = NULL;
    return make_str(vm, U"unknown");
#endif
    if (username == NULL) {
        return make_str(vm, U"unknown");
    } else {
        char32_t* un = char_to_32(username);
        Value result = make_str(vm, un);
        free(un);
        return result;
    }
}

Value _os_get_homedir(PankVm* vm, int argc, Value* args) {
    // TODO: Make more fail proof
#if defined(PANK_OS_UNIX) || defined(PANK_OS_LINUX)
    char* hdir = getenv("HOME");
#elif defined(PANK_OS_WIN)
    char* hdir = getenv("USERPROFILE");
#else
    char* hdir = NULL;
    return make_str(vm, U"unknown");
#endif
    if (hdir == NULL) {
        return make_str(vm, U"unknown");
    } else {
        char32_t* hd = char_to_32(hdir);
        Value result = make_str(vm, hd);
        free(hd);
        return result;
    }
    return make_num(0);
}

Value _os_get_cwd(PankVm* vm, int argc, Value* args) {
    if (argc != 0) {
        return make_error(vm, U"curdir() takes no arguments");
    }
    char* rawdir = get_cur_dir();
    char32_t* result = char_to_32(rawdir);
    free(rawdir);
    Value res = make_str(vm, result);
    free(result);
    return res;
}

void push_stdlib_os(PankVm* vm) {
    SL sls[] = {
        msl(U"name", _os_get_name),     msl(U"arch", _os_get_arch),
        msl(U"user", _os_get_username), msl(U"home", _os_get_homedir),
        msl(U"curdir", _os_get_cwd),
    };

    _push_stdlib(vm, U"os", sls, 5);
}
