#ifndef cpank_obj_h
#define cpank_obj_h

#include "common.h"
#include "instruction.h"
#include "value.h"
#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

typedef enum {
  OBJ_STR = 0,
  OBJ_FUNC,
  OBJ_NATIVE,
  OBJ_CLOUSRE,
  OBJ_UPVAL,
} ObjType;

struct Obj {
  ObjType type;
  bool is_marked;
  struct Obj *next;
};

typedef struct {
  Obj obj;
  int arity;
  Instruction ins;
  ObjString *name;
  int up_count;
} ObjFunc;

typedef struct ObjUpVal {
  Obj obj;
  Value *location;
  struct ObjUpVal *next;
  Value closed;
} ObjUpVal;

typedef Value (*NativeFn)(int argc, Value *args);

typedef struct {
  Obj obj;
  NativeFn func;
} ObjNative;

ObjNative *new_native(NativeFn fn);

typedef struct {
  Obj obj;
  ObjFunc *func;
  ObjUpVal **upv;
  int upv_count;
} ObjClosure;
ObjClosure *new_closure(ObjFunc *function);

struct ObjString {
  Obj obj;
  int len;
  wchar_t *chars;
  uint32_t hash;
};

ObjFunc *new_func();
ObjUpVal *new_up_val(Value *val);
ObjType get_obj_type(Value val);
bool is_obj_type(Value val, ObjType ot);
bool is_str_obj(Value val);
bool is_func_obj(Value val);
bool is_native_obj(Value val);
bool is_closure_obj(Value val);
ObjFunc *get_as_func(Value val);
ObjClosure *get_as_closure(Value val);
ObjString *get_as_string(Value val);
wchar_t *get_as_native_string(Value val);
NativeFn get_as_native(Value val);
ObjString *copy_string(wchar_t *chars, int len);
ObjString *take_string(wchar_t *chars, int len);
void print_obj(Value val);
wchar_t *get_obj_type_as_string(ObjType o);

/*
#define get_as_native(value) \
  (((ObjNative *)get_as_obj(value))->func)
*/
#endif
