#ifndef cpank_vm_h
#define cpank_vm_h

#include "instruction.h"
#include "value.h"
#include <stdint.h>
#define STACK_SIZE 256

typedef struct {
  Instruction *ins;
  uint8_t *ip;
  Value stack[STACK_SIZE];
  Value *stack_top;
} Vm;

typedef enum {
  INTRP_OK,
  INTRP_COMPILE_ERR,
  INTRP_RUNTIME_ERR,
} IResult;

void boot_vm();
void free_vm();
IResult interpret(Instruction *ins);
void push(Value value);
Value pop();

#endif
