/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#ifndef cpank_builtins_h
#define cpank_builtins_h
#ifdef __cplusplus
extern "C" {
#endif
#include <uchar.h>

#include "value.h"

char32_t clock_ntv_name[] = U"clock";
char32_t show_ntv_name[] = U"show";
char32_t asserteq_ntv_name[] = U"asserteq";
char32_t type_ntv_name[] = U"type";
char32_t bn_native_fn_name[] = U"প্রকার";
char32_t len_ntv_name[] = U"len";
char32_t bn_show_ntf_fn[] = U"\u09a6\u09c7\u0996\u09be\u0993";

// This builtin function returns the current time in seconds since
// the program started to execute.
Value clock_ntv_fn(PankVm *vm, int argc, Value *args);

Value show_ntv_fn(PankVm *vm, int argc, Value *args);

// This builtin function returns `true` if the 1st and 2nd argument are equal.
// For now it returns boolean, but in future it would create an error.
Value asserteq_ntv_fn(PankVm *vm, int argc, Value *args);

// This builtin function returns the type of the first argument.
// The result will be in `English`
Value type_ntv_fn(PankVm *vm, int argc, Value *args);

// This builtin function returns the type of the first argument.
// The result will be in `Bengali`
Value bn_type_ntv_fn(PankVm *vm, int argc, Value *args);

// This builtin function returns the `length` of the first argument.
// This function only works on a subset of types available in Pankti.
// * String -> The number of characters.
// * Array  -> The number of elements.
// * Map    -> The number of key-value pairs.
Value len_ntv_fn(PankVm *vm, int argc, Value *args);

#ifdef __cplusplus
}
#endif
#endif
