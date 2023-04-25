/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#include <stddef.h>
#include <stdlib.h>
#include <uchar.h>

#include "../include/common.h"
#include "../include/helper/os.h"
#include "../include/stdlib.h"
#include "../include/utils.h"
#include "../include/value.h"

Value _bn_os_get_name(PankVm* vm, int argc, Value* args) {
    if (argc != 0) {
        return make_error(
            vm,
            U"\u0993\u098f\u09b8 -\u098f\u09b0 \u09a8\u09be\u09ae() "
            U"\u0995\u09be\u099c\u099f\u09bf \u0995\u09cb\u09a8\u09cb "
            U"\u099a\u09b2\u09b0\u09be\u09b6\u09bf "
            U"\u0997\u09cd\u09b0\u09b9\u09a3 \u0995\u09b0\u09c7 \u09a8\u09be");
        // ওএস -এর নাম() কাজটি কোনো চলরাশি গ্রহণ করে না
    }
#if defined(PANK_OS_LINUX)
    return make_str(vm, U"\u09b2\u09bf\u09a8\u09be\u0995\u09cd\u09b8");
#elif defined(PANK_OS_WIN)
    return make_str(vm, U"\u0989\u0987\u09a8\u09cd\u09a1\u09cb\u099c");
#elif defined(PANK_OS_UNIX)
    return make_str(vm, U"\u0987\u0989\u09a8\u09bf\u0995\u09cd\u09b8");
#elif defined(PANK_OS_ANDROID)
    return make_str(vm,
                    U"\u0986\u09a8\u09cd\u09a1\u09cd\u09b0\u09df\u09c7\u09a1");
#elif defined(PANK_OS_MACOS)
    return make_str(vm, U"\u09ae\u09cd\u09af\u09be\u0995\u0993\u098f\u09b8");
#else
    return make_str(vm, U"\u0985\u099c\u09be\u09a8\u09be");
#endif
}

Value _bn_os_get_arch(PankVm* vm, int argc, Value* args) {
#if defined(PANK_ARCH_64)
    return make_str(vm, U"64");
#elif defined(PANK_ARCH_32)
    return make_str(vm, U"32");
#elif defined(PANK_ARCH_ARM)
    return make_str(vm, U"arm");
#else
    return make_str(vm, U"\u0985\u099c\u09be\u09a8\u09be");
#endif
}

Value _bn_os_get_username(PankVm* vm, int argc, Value* args) {
#if defined(PANK_OS_WIN)
    char* username = getenv("USERNAME");
#elif defined(PANK_OS_LINUX) || defined(PANK_OS_UNIX) || defined(PANK_OS_MACOS)
    char* username = getenv("USER");
#else
    char* username = NULL;
    return make_str(vm, U"\u0985\u099c\u09be\u09a8\u09be");
#endif
    if (username == NULL) {
        return make_str(vm, U"\u0985\u099c\u09be\u09a8\u09be");
    } else {
        char32_t* un = char_to_32(username);
        Value result = make_str(vm, un);
        free(un);
        return result;
    }
}

Value _bn_os_get_homedir(PankVm* vm, int argc, Value* args) {
    // TODO: Make more fail proof
#if defined(PANK_OS_UNIX) || defined(PANK_OS_LINUX)
    char* hdir = getenv("HOME");
#elif defined(PANK_OS_WIN)
    char* hdir = getenv("USERPROFILE");
#else
    char* hdir = NULL;
    return make_str(vm, U"\u0985\u099c\u09be\u09a8\u09be");
#endif
    if (hdir == NULL) {
        return make_str(vm, U"\u0985\u099c\u09be\u09a8\u09be");
    } else {
        char32_t* hd = char_to_32(hdir);
        Value result = make_str(vm, hd);
        free(hd);
        return result;
    }
    return make_num(0);
}

Value _bn_os_get_cwd(PankVm* vm, int argc, Value* args) {
    if (argc != 0) {
        return make_error(
            vm,
            U"\u09ac\u09b0\u09cd\u09a4\u09ae\u09be\u09a8() "
            U"\u0995\u09be\u099c\u099f\u09bf \u0995\u09cb\u09a8\u09cb "
            U"\u099a\u09b2\u09b0\u09be\u09b6\u09bf "
            U"\u0997\u09cd\u09b0\u09b9\u09a3 \u0995\u09b0\u09c7 \u09a8\u09be");
    }
    char* rawdir = get_cur_dir();
    char32_t* result = char_to_32(rawdir);
    free(rawdir);
    Value res = make_str(vm, result);
    free(result);
    return res;
}

void push_stdlib_os_bn(PankVm* vm) {
    SL sls[] = {
        msl(U"\u09a8\u09be\u09ae", _bn_os_get_name),        // নাম
        msl(U"\u0986\u09b0\u09cd\u099a", _bn_os_get_arch),  // আর্চ
        msl(U"\u09ac\u09be\u09ac\u09b9\u09be\u09b0\u0995\u09be\u09b0\u09bf",
            _bn_os_get_username),  // বাবহারকারি
        msl(U"\u0998\u09b0", _bn_os_get_homedir),  // ঘর
        msl(U"\u09ac\u09b0\u09cd\u09a4\u09ae\u09be\u09a8",
            _bn_os_get_cwd),  // বর্তমান
    };

    _push_stdlib(vm, U"\u0993\u098f\u09b8", sls, 5);  // ওএস
}
