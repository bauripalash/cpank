#include "include/vm.h"
#include "include/common.h"
#include "include/debug.h"
#include "include/instruction.h"
#include "include/value.h"
#include <stdint.h>
#include <wchar.h>

Vm vm;

void reset_stack() { vm.stack_top = vm.stack; }

void boot_vm() { reset_stack(); }

void free_vm() {}

void push(Value value) {
  *vm.stack_top = value;
  vm.stack_top++;
}

Value pop() {
  vm.stack_top--;
  return *vm.stack_top;
}

uint8_t read_bt() { return *vm.ip++; }
Value read_const() { return vm.ins->consts.values[read_bt()]; }

void bin_add() {
  double r = pop();
  double l = pop();
  push(l + r);
}

void bin_sub() {
  double r = pop();
  double l = pop();
  push(l - r);
}

void bin_mul() {
  double r = pop();
  double l = pop();
  push(l * r);
}

void bin_div() {
  double r = pop();
  double l = pop();
  push(l / r);
}

IResult run_vm() {
  for (;;) {
#ifdef DEBUG_TRACE
#ifdef DEBUG_STACK
    wprintf(L"------ STACK ----\n");
    for (Value *slt = vm.stack; slt < vm.stack_top; slt++) {
      wprintf(L"[ ");
      print_val(*slt);
      wprintf(L" ]");
    }
    wprintf(L"--- END STACK ---\n");
#endif
    dissm_ins(vm.ins, (int)(vm.ip - vm.ins->code));

#endif
    uint8_t ins;
    switch (ins = read_bt()) {
    case OP_RETURN: {
      print_val(pop());
      wprintf(L"\n");
      return INTRP_OK;
    }
    case OP_CONST: {
      Value con = read_const();
      push(con);
      break;
    }
    case OP_NEG: {
      push(-pop());
      break;
    }
    case OP_ADD:
      bin_add();
      break;
    case OP_SUB:
      bin_sub();
      break;
    case OP_MUL:
      bin_mul();
      break;
    case OP_DIV:
      bin_div();
      break;
    }
  }
  return INTRP_RUNTIME_ERR;
}

IResult interpret(Instruction *ins) {

  vm.ins = ins;
  vm.ip = vm.ins->code;
  return run_vm();
}
