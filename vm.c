#include "include/common.h"
#include "include/compiler.h"
#include "include/debug.h"
#include "include/instruction.h"
#include "include/mem.h"
#include "include/obj.h"
#include "include/value.h"
#include <locale.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "include/vm.h"

Vm vm;
Value last_pop;
// #define DEBUG_STACK
#define DEBUG_TRACE

void reset_stack() { vm.stack_top = vm.stack; }

void boot_vm() {
  reset_stack();
  vm.objs = NULL;
  last_pop = make_nil();
  init_table(&vm.strings);
  init_table(&vm.globals);
}

void free_vm() {
  last_pop = make_nil();
  free_table(&vm.strings);
  free_table(&vm.globals);
  free_objs();
}

Value get_last_pop() { return last_pop; }

void push(Value value) {
  *vm.stack_top = value;
  vm.stack_top++;
}

Value pop() {
  vm.stack_top--;
  last_pop = *vm.stack_top;
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
uint16_t read_u16() {
  vm.ip += 2;
  return (uint16_t)((vm.ip[-2] << 8) | vm.ip[-1]);
}
Value read_const() { return vm.ins->consts.values[read_bt()]; }
ObjString *read_str_const() { return get_as_string(read_const()); }
void add_string() {
  ObjString *r = get_as_string(pop());
  ObjString *l = get_as_string(pop());

  int newlen = l->len + r->len;
  wchar_t *newchars = ALLOC(wchar_t, newlen + 1);
  wmemcpy(newchars, l->chars, l->len);
  wmemcpy(newchars + l->len, r->chars, r->len);
  newchars[newlen] = '\0';
  ObjString *new_obj = take_string(newchars, newlen);
  push(make_obj_val(new_obj));
}

bool bin_add() {
  if (is_str_obj(peek_vm(0)) && is_str_obj(peek_vm(1))) {
    add_string();
    return true;

  } else if (is_num(peek_vm(0)) && is_num(peek_vm(1))) {
    double r = get_as_number(pop());
    double l = get_as_number(pop());
    push(make_num(l + r));
    return true;

  } else {

    runtime_err(
        L"Operands must be numbers or string for binary addition operation");
    return false;
  }
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

bool bin_gt() {
  if (!is_num(peek_vm(0)) || !is_num(peek_vm(1))) {
    runtime_err(L"Operands must be numbers for binary operation");
    return false;
  }
  double r = get_as_number(pop());
  double l = get_as_number(pop());
  push(make_bool(l > r));
  return true;
}

bool bin_lt() {
  if (!is_num(peek_vm(0)) || !is_num(peek_vm(1))) {
    runtime_err(L"Operands must be numbers for binary operation");
    return false;
  }
  double r = get_as_number(pop());
  double l = get_as_number(pop());
  push(make_bool(l < r));
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
      //  print_val(pop());
      //  wprintf(L"\n");
      return INTRP_OK;
    }
    case OP_CONST: {
      Value con = read_const();
      push(con);
      break;
    }
    case OP_POP:
      pop();
      break;
    case OP_NEG: {
      if (!is_num(peek_vm(0))) {
        return INTRP_RUNTIME_ERR;
      }
      push(make_neg(pop()));
      break;
    }
    case OP_ADD:
      if (!bin_add()) {
        return INTRP_RUNTIME_ERR;
      }
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
      push(make_bool(true));
      break;
    }
    case OP_NOT: {
      bool b_val = is_falsey(pop());
      push(make_bool(b_val));
      break;
    }
    case OP_EQ: {
      Value r = pop();
      Value l = pop();
      push(make_bool(is_equal(l, r)));
      break;
    }
    case OP_GT: {
      if (!bin_gt()) {
        return INTRP_RUNTIME_ERR;
      }
      break;
    }
    case OP_LT: {
      if (!bin_lt()) {
        return INTRP_RUNTIME_ERR;
      }
      break;
    }
    case OP_FALSE:
      push(make_bool(false));
      break;
    case OP_SHOW:
      wprintf(L"p~ ");
      print_val(pop());
      wprintf(L"\n");
      break;
    case OP_DEF_GLOB: {
      ObjString *nm = read_str_const();
      // wprintf(L"Def global -> %ls\n" , nm->chars);
      // print_val(peek_vm(0));
      table_set(&vm.globals, nm, peek_vm(0));
      pop();
      break;
    }
    case OP_GET_GLOB: {
      ObjString *name = read_str_const();
      // wprintf(L"GET_GLOB name -> %ls\n" , name->chars);
      // print_table(&vm.globals , "global");
      // print_obj();
      Value val;
      if (!table_get(&vm.globals, name, &val)) {
        runtime_err(L"Get Global -> Undefined variable '%ls'.", name->chars);
        return INTRP_RUNTIME_ERR;
      }
      push(val);
      break;
    }

    case OP_SET_GLOB: {
      ObjString *name = read_str_const();
      if (table_set(&vm.globals, name, peek_vm(0))) {
        table_del(&vm.globals, name);
        runtime_err(L"Set Global -> Undefined var '%ls'", name->chars);
        return INTRP_RUNTIME_ERR;
      }
      break;
    }

    case OP_GET_LOCAL: {
      uint8_t slot = read_bt();
      push(vm.stack[slot]);
      break;
    }
    case OP_SET_LOCAL: {
      uint8_t slot = read_bt();
      vm.stack[slot] = peek_vm(0);
      break;
    }
    case OP_JMP_IF_FALSE: {
      uint16_t offset = read_u16();
      // wprintf(L"JIF -> %d\n" , offset);
      if (is_falsey(peek_vm(0))) {
        vm.ip += offset;
      }
      break;
    }
    case OP_JMP: {
      uint16_t offset = read_u16();
      vm.ip += offset;
      break;
    }
    case OP_LOOP: {
      uint16_t offset = read_u16();
      vm.ip -= offset;
      break;
    }
    }
  }
  return INTRP_RUNTIME_ERR;
}

IResult interpret(wchar_t *source) {
  Instruction ins;
  init_instruction(&ins);
  // wprintf(L"COMPILER _> %s" , compile(source, &ins) ? "true" : "false");
  // return 0;
  if (!compile(source, &ins)) {
    dissm_ins_chunk(&ins, "BEFORE");
    free_ins(&ins);
    // wprintf(L"COMPILEERR\n");
    return INTRP_COMPILE_ERR;
  }

  vm.ins = &ins;
  vm.ip = vm.ins->code;
  IResult res = run_vm();
  free_ins(&ins);
  // free(source);
  return res;
}
