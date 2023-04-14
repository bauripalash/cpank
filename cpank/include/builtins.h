/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#ifndef cpank_builtins_h
#define cpank_builtins_h
#include <uchar.h>

#include "value.h"

char32_t clock_ntv_name[] = U"clock";
char32_t asserteq_ntv_name[] = U"asserteq";
char32_t type_ntv_name[] = U"type";
char32_t bn_native_fn_name[] = U"প্রকার";
char32_t len_ntv_name[] = U"len";

Value clock_ntv_fn(PankVm *vm, int argc, Value *args);
Value asserteq_ntv_fn(PankVm *vm, int argc, Value *args);
Value type_ntv_fn(PankVm *vm, int argc, Value *args);
Value bn_type_ntv_fn(PankVm *vm, int argc, Value *args);
Value len_ntv_fn(PankVm *vm, int argc, Value *args);
#endif
