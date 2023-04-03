#ifndef cpank_stdlib_h
#define cpank_stdlib_h

#include <uchar.h>
#include <wchar.h>

#include "obj.h"

#define MAX_STDLIB_ERR 1
#define STDLIB_ALREADY_EXIST_WARNING 2
#define CONST_PI 3.14159265358979323846
#define CONST_E 2.71828182845904523536

// Stdlib names
#define STDMATH u"math"
#define STDMATH_BN u"গণিত"

typedef struct SL {
    char16_t* key;
    NativeFn func;
} SL;

// Return a Stdlib (SL) struct
SL msl(char16_t* key, NativeFn func);

int _push_stdlib(PankVm* vm, char16_t* stdname, SL* funcs, int len);
int _push_single_stdfunc(char16_t* stdname, ObjString* key, NativeFn fn);
void push_stdlib_math(PankVm* vm);
void push_stdlib_math_bn(PankVm* vm);

#endif
