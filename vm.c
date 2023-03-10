#include "include/vm.h"
#include "include/common.h"
#include "include/compiler.h"
#include "include/debug.h"
#include "include/instruction.h"
#include "include/value.h"
#include <locale.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <wchar.h>

#define DEBUG_STACK
#define DEBUG_TRACE

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

Value peek_vm(int dist) { return vm.stack_top[-1 - dist]; }

void runtime_err(wchar_t *format, ...) {
  setlocale(LC_CTYPE, "");
  // wprintf(format);
  va_list args;
  va_start(args, format);
  vfwprintf(stderr, format, args);

  va_end(args);
  fputwc('\n', stderr);

  size_t inst = vm.ip - vm.ins->code - 1;
  int line = vm.ins->lines[inst];
  fwprintf(stderr, L"Error [line %d] in script\n", line);
  reset_stack();
}

uint8_t read_bt() { return *vm.ip++; }
Value read_const() { return vm.ins->consts.values[read_bt()]; }

bool bin_add() {
  if (!is_num(peek_vm(0)) || !is_num(peek_vm(1))) {
    runtime_err(L"Operands must be numbers for binary operation");
    return false;
  }
  double r = get_as_number(pop());
  double l = get_as_number(pop());
  push(make_num(l + r));
  return true;
}

bool bin_sub() {
  if (!is_num(peek_vm(0)) || !is_num(peek_vm(1))) {
    runtime_err(L"Operands must be numbers for binary operation");
    return false;
  }
  double r = get_as_number(pop());
  double l = get_as_number(pop());
  push(make_num(l - r));
  return true;
}

bool bin_mul() {
  if (!is_num(peek_vm(0)) || !is_num(peek_vm(1))) {
    runtime_err(L"Operands must be numbers for binary operation");
    return false;
  }
  double r = get_as_number(pop());
  double l = get_as_number(pop());
  push(make_num(l * r));
  return true;
}

bool bin_div() {
  if (!is_num(peek_vm(0)) || !is_num(peek_vm(1))) {
    runtime_err(L"Operands must be numbers for binary operation");
    return false;
  }
  double r = get_as_number(pop());
  double l = get_as_number(pop());
  push(make_num(l / r));
  return true;
}

IResult run_vm() {
  for (;;) {
#ifdef DEBUG_TRACE
#ifdef DEBUG_STACK
    wprintf(L"------ STACK ----\n");
    for (Value *slt = vm.stack; slt < vm.stack_top; slt++) {
      wprintf(L"[ ");
      print_val(*slt);
      wprintf(L" ]\n");
    }
    wprintf(L"--- END STACK ---\n");
#endif
    // dissm_ins(vm.ins, (int)(vm.ip - vm.ins->code));

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
      if (!is_num(peek_vm(0))) {
        return INTRP_RUNTIME_ERR;
      }
      push(make_neg(pop()));
      break;
    }
    case OP_ADD:
      if (!bin_add()) {
        // runtime_err(L"Failed to + operation" , L"x");
        return INTRP_RUNTIME_ERR;
      }
      // bin_add();
      break;
    case OP_SUB:
      if (!bin_sub()) {
        return INTRP_RUNTIME_ERR;
      }
      break;
    case OP_MUL:
      if (!bin_mul()) {
        return INTRP_RUNTIME_ERR;
      }
      break;
    case OP_DIV:
      if (!bin_div()) {
        return INTRP_RUNTIME_ERR;
      }
      break;
    case OP_NIL:
      push(make_nil());
      break;
    case OP_TRUE: {
      // Value tv = make_bool(true);
      // wprintf(L"\nTV-> %d\n" , tv.as.boolean);
      // print_val(tv);
      push(make_bool(true));
      break;
    }
    case OP_FALSE:
      push(make_bool(false));
      break;
    }
  }
  return INTRP_RUNTIME_ERR;
}

IResult interpret(wchar_t *source) {
  Instruction ins;
  init_instruction(&ins);

  if (!compile(source, &ins)) {
    free_ins(&ins);
    return INTRP_COMPILE_ERR;
  }

  vm.ins = &ins;
  vm.ip = vm.ins->code;
  IResult res = run_vm();
  free_ins(&ins);
  return res;
}
