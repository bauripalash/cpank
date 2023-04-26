/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#ifndef cpank_stdlib_h
#define cpank_stdlib_h

#include <uchar.h>
#include <wchar.h>

#include "obj.h"

#define MAX_STDLIB_ERR               1
#define STDLIB_ALREADY_EXIST_WARNING 2
#define CONST_PI                     3.14159265358979323846
#define CONST_E                      2.71828182845904523536

// Stdlib names
#define STDMATH    U"math"
#define STDMATH_BN U"\u0997\u09a3\u09bf\u09a4"
#define STDOS      U"os"
#define STDOS_BN   U"\u0993\u098f\u09b8"
#define STDCOMMON  U"common"
#define STDARRAY   U"array"
#define STDSTR     U"str"
#define STDFILE    U"file"
#define STDHMAP    U"map"
#define STDBIG     U"big"

#define check_argc_count(funcname, want, got)                    \
 if (argc != want) {                                             \
  return make_argc_want_mismatch_error(vm, funcname, want, got); \
 }

#define bn_check_argc_count(funcname, want, got)                    \
 if (argc != want) {                                                \
  return make_bn_argc_want_mismatch_error(vm, funcname, want, got); \
 }

typedef struct SL {
    char32_t* key;
    NativeFn func;
} SL;

// Return a Stdlib (SL) struct
SL msl(char32_t* key, NativeFn func);

int _push_stdlib(PankVm* vm, char32_t* stdname, SL* funcs, int len);
int _push_single_stdfunc(char32_t* stdname, ObjString* key, NativeFn fn);
void push_stdlib_math(PankVm* vm);
void push_stdlib_math_bn(PankVm* vm);
void push_stdlib_os(PankVm* vm);
void push_stdlib_os_bn(PankVm* vm);
void push_stdlib_common(PankVm* vm);
void push_stdlib_array(PankVm* vm);
void push_stdlib_string(PankVm* vm);
void push_stdlib_file(PankVm* vm);
void push_stdlib_map(PankVm* vm);
void push_stdlib_big(PankVm* vm);
#endif
