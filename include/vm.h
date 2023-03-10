#ifndef cpank_vm_h
#define cpank_vm_h

#include "common.h"
#include "htable.h"
#include "instruction.h"
#include "obj.h"
#include "value.h"
#include <stdint.h>
#include <wchar.h>

#define FRAME_SIZE 64
#define STACK_SIZE (FRAME_SIZE * UINT8_COUNT)

typedef struct {
  ObjClosure *closure;
  uint8_t *ip;
  Value *slots;
} CallFrame;

typedef struct {
  Instruction *ins;
  uint8_t *ip;
  Value stack[STACK_SIZE];
  Value *stack_top;
  Htable strings;
  Htable globals;
  Obj *objs;
  CallFrame frames[FRAME_SIZE];
  int frame_count;
  ObjUpVal *open_upvs;
  int gray_count;
  int gray_cap;
  Obj **gray_stack;
  size_t bts_allocated;
  size_t next_gc;
  Value last_pop;
} Vm;

typedef enum {
  INTRP_OK,
  INTRP_COMPILE_ERR,
  INTRP_RUNTIME_ERR,
} IResult;

extern Vm vm;

void boot_vm();
void free_vm();
IResult interpret(wchar_t *source);
void push(Value value);
Value pop();
Value get_last_pop();
bool call_val(Value calle, int argc);
bool call(ObjClosure *closure, int argc);
void define_native(wchar_t *name, NativeFn func);
Value clock_ntv_fn(int argc, Value *args);
ObjUpVal *capture_upv(Value *local);
void close_upval(Value *last);

#endif
