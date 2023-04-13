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
#define STDMATH_BN U"গণিত"
#define STDOS      U"os"
#define STDCOMMON  U"common"
#define STDARRAY   U"array"
#define STDSTR     U"str"

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
void push_stdlib_common(PankVm* vm);
void push_stdlib_array(PankVm* vm);
void push_stdlib_string(PankVm* vm);
#endif
