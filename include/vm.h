#ifndef cpank_vm_h
#define cpank_vm_h

#include "htable.h"
#include "instruction.h"
#include "value.h"
#include <stdint.h>
#include <wchar.h>
#define STACK_SIZE 256

typedef struct {
  Instruction *ins;
  uint8_t *ip;
  Value stack[STACK_SIZE];
  Value *stack_top;
  Htable strings;
  Obj *objs;
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

#endif
