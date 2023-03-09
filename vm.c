#include "include/vm.h"
#include "include/common.h"
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

IResult run_vm() {
  for (;;) {
    uint8_t ins;
    switch (ins = read_bt()) {
    case OP_RETURN: {
      wprintf(L"OP_RETURN\n");
      return INTRP_OK;
    }
    case OP_CONST: {
      Value con = read_const();
      print_val(con);
      wprintf(L"\n");
      break;
    }
    }
  }
  return INTRP_RUNTIME_ERR;
}

IResult interpret(Instruction *ins) {

  vm.ins = ins;
  vm.ip = vm.ins->code;
  return run_vm();
}
