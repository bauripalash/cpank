#ifndef cpank_stdlib_h
#define cpank_stdlib_h

#include <wchar.h>

#include "obj.h"

#define MAX_STDLIB_ERR 1
#define STDLIB_ALREADY_EXIST_WARNING 2

typedef struct SL {
    wchar_t* key;
    NativeFn func;
} SL;

// Return a Stdlib (SL) struct
SL msl(wchar_t* key, NativeFn func);

int _push_stdlib(wchar_t* stdname, SL* funcs, int len);
void push_stdlib_math();

#endif
