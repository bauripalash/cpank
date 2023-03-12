#ifndef cpank_obj_h
#define cpank_obj_h

#include "common.h"
#include "instruction.h"
#include "value.h"
#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

typedef enum {
  OBJ_STR,
  OBJ_FUNC,
  OBJ_NATIVE,
} ObjType;

struct Obj {
  ObjType type;
  struct Obj *next;
};

typedef struct {
  Obj obj;
  int arity;
  Instruction ins;
  ObjString *name;
} ObjFunc;

typedef Value (*NativeFn)(int argc, Value *args);

typedef struct {
  Obj obj;
  NativeFn func;
} ObjNative;

ObjNative *new_native(NativeFn fn);

struct ObjString {
  Obj obj;
  int len;
  wchar_t *chars;
  uint32_t hash;
};

ObjFunc *new_func();
ObjType get_obj_type(Value val);
bool is_obj_type(Value val, ObjType ot);
bool is_str_obj(Value val);
bool is_func_obj(Value val);
bool is_native_obj(Value val);
ObjFunc *get_as_func(Value val);
ObjString *get_as_string(Value val);
wchar_t *get_as_native_string(Value val);
NativeFn get_as_native(Value val);
ObjString *copy_string(wchar_t *chars, int len);
ObjString *take_string(wchar_t *chars, int len);
void print_obj(Value val);

/*
#define get_as_native(value) \
  (((ObjNative *)get_as_obj(value))->func)
*/
#endif
