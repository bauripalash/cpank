#ifndef cpank_stdlib_h
#define cpank_stdlib_h

#include <wchar.h>

#include "obj.h"

#define MAX_STDLIB_ERR 1
#define STDLIB_ALREADY_EXIST_WARNING 2

int _push_stdlib(wchar_t* stdname, wchar_t** key, NativeFn* fn, int len);
void push_stdlib_math();

#endif
