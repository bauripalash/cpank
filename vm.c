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
#include <string.h>
#include <time.h>
#include <wchar.h>

#include "include/vm.h"

Vm vm;

void reset_stack() {
  vm.stack_top = vm.stack;
  vm.frame_count = 0;
  vm.open_upvs = NULL;
}

void boot_vm() {
  reset_stack();
  vm.objs = NULL;
  // last_pop = make_nil();
  // vm.last_pop = make_nil();
  // v//m.last_pop = (Value*)malloc(sizeof(Value));
  vm.last_pop = make_nil();

  vm.bts_allocated = 0;
  vm.next_gc = 1024 * 1024;
  vm.gray_cap = 0;
  vm.gray_count = 0;
  vm.gray_stack = NULL;

  init_table(&vm.strings);
  init_table(&vm.globals);
  define_native(L"clock", clock_ntv_fn);
}

void free_vm() {
  // free(vm.last_pop);
  // vm.last_pop = make_nil();
  free_table(&vm.strings);
  free_table(&vm.globals);
  free_objs();
}

Value get_last_pop() { return vm.last_pop; }

void push(Value value) {
  *vm.stack_top = value;
  vm.stack_top++;
}

Value pop() {
  vm.stack_top--;
  // vm.last_pop = *vm.stack_top;

  // print_val_type(last_pop.type);
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

  for (int i = vm.frame_count - 1; i >= 0; i--) {
    CallFrame *frm = &vm.frames[i];
    ObjFunc *fn = frm->closure->func;
    size_t inst = frm->ip - fn->ins.code - 1; // vm.ip - vm.ins->code - 1;
    int line = fn->ins.lines[inst];           // vm.ins->lines[inst];
    fwprintf(stderr, L"Error [line %d] in \n", line);
    if (fn->name == NULL) {
      fwprintf(stderr, L"script\n");
    } else {
      fwprintf(stderr, L"%.*ls()", fn->name->len, fn->name->chars);
    }
  }

  reset_stack();
}

CallFrame *get_cur_farme() { return &vm.frames[vm.frame_count - 1]; }
uint8_t read_bt() { return *get_cur_farme()->ip++; }
uint16_t read_u16() {
  CallFrame *cf = get_cur_farme();
  cf->ip += 2;
  return (uint16_t)((cf->ip[-2] << 8) | cf->ip[-1]);
}
Value read_const() {
  return get_cur_farme()->closure->func->ins.consts.values[read_bt()];
}
ObjString *read_str_const() { return get_as_string(read_const()); }
void add_string() {
  ObjString *r = get_as_string(peek_vm(0));
  ObjString *l = get_as_string(peek_vm(1));

  int newlen = l->len + r->len;
  wchar_t *newchars = ALLOC(wchar_t, newlen + 1);
  wmemcpy(newchars, l->chars, l->len);
  wmemcpy(newchars + l->len, r->chars, r->len);
  newchars[newlen] = '\0';
  ObjString *new_obj = take_string(newchars, newlen);
  pop();
  pop();
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
  CallFrame *frame = &vm.frames[vm.frame_count - 1];
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
    dissm_ins(&frame->func.ins, (int)(frame->ip - frame->func.ins.code));

#endif
    uint8_t ins;
    switch (ins = read_bt()) {
    case OP_RETURN: {
      Value res = pop();
      close_upval(frame->slots);
      vm.frame_count--;
      if (vm.frame_count == 0) {
        pop();
        return INTRP_OK;
      }

      vm.stack_top = frame->slots;
      push(res);
      frame = &vm.frames[vm.frame_count - 1];
      break;
      //  print_val(pop());
      //  wprintf(L"\n");
      // return INTRP_OK;
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

      Value to_show = pop();

      // memcpy(vm.last_pop, to_show, sizeof(Value));
      vm.last_pop = to_show;
      print_val(to_show);
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
      // push(vm.stack[slot]);
      push(frame->slots[slot]);
      break;
    }
    case OP_SET_LOCAL: {
      uint8_t slot = read_bt();
      frame->slots[slot] = peek_vm(0);
      break;
    }
    case OP_JMP_IF_FALSE: {
      uint16_t offset = read_u16();
      // wprintf(L"JIF -> %d\n" , offset);
      if (is_falsey(peek_vm(0))) {
        frame->ip += offset;
      }
      break;
    }
    case OP_JMP: {
      uint16_t offset = read_u16();
      frame->ip += offset;
      break;
    }
    case OP_LOOP: {
      uint16_t offset = read_u16();
      frame->ip -= offset;
      break;
    }
    case OP_CLOSURE: {
      ObjFunc *func = get_as_func(read_const());
      ObjClosure *cls = new_closure(func);
      push(make_obj_val(cls));
      for (int i = 0; i < cls->upv_count; i++) {
        uint16_t is_local = read_bt();
        uint8_t index = read_bt();
        if (is_local) {
          cls->upv[i] = capture_upv(frame->slots + index);
          // capture
        } else {
          cls->upv[i] = frame->closure->upv[index];
        }
      }
      break;
    }
    case OP_GET_UP: {
      uint8_t slot = read_bt();
      push(*frame->closure->upv[slot]->location);
      break;
    }
    case OP_SET_UP: {
      uint8_t slot = read_bt();
      *frame->closure->upv[slot]->location = peek_vm(0);
      break;
    }
    case OP_CLS_UP: {
      close_upval(vm.stack_top - 1);
      pop();
      break;
    }
    case OP_CALL: {
      int argc = read_bt();
      if (!call_val(peek_vm(argc), argc)) {
        return INTRP_RUNTIME_ERR;
      }

      frame = &vm.frames[vm.frame_count - 1];
      break;
    }
    }
  }
  return INTRP_RUNTIME_ERR;
}

void close_upval(Value *last) {
  while (vm.open_upvs != NULL && vm.open_upvs->location >= last) {
    ObjUpVal *upval = vm.open_upvs;
    upval->closed = *upval->location;
    upval->location = &upval->closed;
    vm.open_upvs = upval->next;
  }
}

void define_native(wchar_t *name, NativeFn func) {
  push(make_obj_val(copy_string(name, (int)wcslen(name))));
  push(make_obj_val(new_native(func)));
  table_set(&vm.globals, get_as_string(vm.stack[0]), vm.stack[1]);
  pop();
  pop();
}

ObjUpVal *capture_upv(Value *local) {
  ObjUpVal *prev = NULL;
  ObjUpVal *upv = vm.open_upvs;
  while (upv != NULL && upv->location > local) {
    prev = upv;
    upv = upv->next;
  }

  if (upv != NULL && upv->location == local) {
    return upv;
    ;
  }
  ObjUpVal *new_upv = new_up_val(local);
  new_upv->next = upv;
  if (prev == NULL) {
    vm.open_upvs = new_upv;
  } else {
    prev->next = new_upv;
  }
  return new_upv;
}

Value clock_ntv_fn(int argc, Value *args) {
  return make_num((double)clock() / CLOCKS_PER_SEC);
}

bool call_val(Value calle, int argc) {
  if (is_obj(calle)) {
    switch (get_obj_type(calle)) {
    case OBJ_CLOUSRE:
      return call(get_as_closure(calle), argc);
    case OBJ_NATIVE: {
      NativeFn native = get_as_native(calle);
      Value result = native(argc, vm.stack_top - argc);
      vm.stack_top -= argc + 1;
      push(result);
      return true;
    }
    default:
      break;
    }
  }
  runtime_err(L"can only call functions");
  return false;
}

bool call(ObjClosure *closure, int argc) {
  if (closure->func->arity != argc) {
    runtime_err(L"Expected %d args but got %d", closure->func->arity, argc);
    return false;
  }

  if (vm.frame_count == FRAME_SIZE) {
    runtime_err(L"Too many frames! Stack overflow");
    return false;
  }

  CallFrame *frame = &vm.frames[vm.frame_count++];
  frame->closure = closure;
  frame->ip = closure->func->ins.code;
  frame->slots = vm.stack_top - argc - 1;
  return true;
}

IResult interpret(wchar_t *source) {
  // init_instruction(&ins);
  //  wprintf(L"COMPILER _> %s" , compile(source, &ins) ? "true" : "false");
  //  return 0;

  // Instruction ins
  ObjFunc *fn = compile(source);
  if (fn == NULL) {
    return INTRP_COMPILE_ERR;
  }

  push(make_obj_val(fn));
  ObjClosure *cls = new_closure(fn);
  pop();
  push(make_obj_val(cls));
  call(cls, 0);
  // CallFrame *frame = &vm.frames[vm.frame_count++];
  // frame->func = fn;
  // frame->ip = fn->ins.code;
  // frame->slots = vm.stack;

  // if (ins == NULL) {
  //   dissm_ins_chunk(&ins, "BEFORE");
  //   free_ins(&ins);
  //  wprintf(L"COMPILEERR\n");
  //  return INTRP_COMPILE_ERR;
  //}
  //

  // vm.ins = &fn->ins;
  // vm.ip = vm.ins->code;
  // IResult res = run_vm();
  // free_ins(&fn->ins);
  //  free(source);
  // return res;
  return run_vm();
}
