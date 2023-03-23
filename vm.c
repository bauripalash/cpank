#include "include/common.h"
#include "include/compiler.h"
#include "include/debug.h"
#include "include/instruction.h"
#include "include/mem.h"
#include "include/obj.h"
#include "include/value.h"
#include <locale.h>
#include <math.h>
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
const wchar_t *default_mod = L"__d__";
// #define DEBUG_STACK

void reset_stack() {
  vm.stack_top = vm.stack;
  // vm.frame_count = 0;
  // vm.open_upvs = NULL;
}

void boot_vm() {
  reset_stack();
  vm.objs = NULL;
  vm.last_pop = make_nil();

  vm.bts_allocated = 0;
  vm.next_gc = 1024 * 1024;
  vm.gray_cap = 0;
  vm.gray_count = 0;
  vm.gray_stack = NULL;
  vm.mod_count = 0;
  // vm.current_mod = 0;

  init_table(&vm.strings);

  vm.mod_names[vm.mod_count] = 0;
  Module *dmod = &vm.modules[vm.mod_count++];
  init_module(dmod, default_mod);
  // init_table(&dmod->globals);
  dmod->is_default = true;
  dmod->origin = NULL;
  vm.mod_names[vm.mod_count] = get_hash(default_mod, wcslen(default_mod));
  vm.current_mod = dmod;
  // init_table(&vm.globals);
  // define_native(L"clock", clock_ntv_fn);
}

void init_module(Module *mod, const wchar_t *name) {

  init_table(&mod->globals);
  mod->frame_count = 0;
  //for (int i = 0; i != FRAME_SIZE; i++) {
  //  CallFrame *frm = &mod->frames[i];
    // frm = NULL;
  //}

  mod->name = malloc(sizeof(wchar_t) * (wcslen(name) + 1));
  wmemcpy(mod->name, name, wcslen(name) + 1);
  mod->open_upvs = NULL;
}

bool is_default(Module *mod) {
  return wcscmp(mod->name, default_mod) == 0 && mod->is_default;
}

Module *get_mod_by_hash(uint32_t hash) {
  for (int i = 0; i < vm.mod_count; i++) {
    // wprintf(L"M| %4d -> %ld -> %ls | %ld \n" , i , vm.mod_names[i] ,
    // vm.modules[i].name , hash);
    if (vm.mod_names[i] == hash) {
      return &vm.modules[i];
    }
  }
  return NULL;
}

void print_mod_names() {
  for (int i = 0; i < vm.mod_count; i++) {

    wprintf(L"M| %4d -> %ld -> %ls \n", i, vm.mod_names[i], vm.modules[i].name);
  }
}
void free_module(Module *mod) {
  //setlocale(LC_CTYPE, "");
  // wprintf(L"freeing module -> %ls -> is default -> %s\n", mod->name,
  //         is_default(mod) ? "true" : "false");
  free_table(&mod->globals);
  mod->frame_count = 0;

  free(mod->name);
  // ObjMod *objmod = mod->mod;
  //  free_single_obj((Obj *)objmod->name);
  //  FREE(ObjString, objmod->name);
  // for (int i = 0; i < mod->frame_count; i++) {
  //  CallFrame *frame = &mod->frames[i];
  //  FREE(ObjClosure, frame->closure);
  // free_single_obj((Obj *)frame->closure);
  //}
  // mod->open_upvs = NULL;
}

void define_native(wchar_t *name, NativeFn func) {
  push(make_obj_val(copy_string(name, (int)wcslen(name))));
  push(make_obj_val(new_native(func)));
  // Value k = make_obj_val(copy_string(name, (int)wcslen(name)));
  // Value fn = make_obj_val(new_native(func));
#ifdef DEBUG_STACK
  wprintf(L"------ STACK in Define native----\n");
  for (Value *slt = vm.stack; slt < vm.stack_top; slt++) {
    wprintf(L"[ ");
    print_val(*slt);
    wprintf(L" ]\n");
  }
  wprintf(L"--- END STACK ---\n");
#endif
  table_set(&vm.modules[0].globals, get_as_string(vm.stack[0]), vm.stack[1]);
  pop();
  pop();
}

void print_modframes() {
  for (int i = 0; i < vm.mod_count; i++) {
    Module *mod = &vm.modules[i];
    wprintf(L"module %ls has %d frames \n", mod->name, mod->frame_count);
    for (int f = 0; f < mod->frame_count; f++) {
      CallFrame *fr = &mod->frames[f];
      wchar_t *name = L"unknown";
      if (fr->closure != NULL) {
        if (fr->closure->func != NULL) {
          if (fr->closure->func->name != NULL) {
            if (fr->closure->func->name->chars != NULL) {
              name = fr->closure->func->name->chars;
            }
          }
        }
      }
      wprintf(L"]] FRAME %d | %ls\n", f, name);
    }
  }
}

void free_vm() {
  //wprintf(L"----------FREE VM----------\n");
  //print_modframes();
  // free(vm.last_pop);
  // vm.last_pop = make_nil();
  free_table(&vm.strings);
  // free_table(&vm.globals);
  for (int i = 0; i < vm.mod_count; i++) {
    free_module(&vm.modules[i]);
  }
  free_objs();

  //wprintf(L"----------DONE FREE VM----------\n");
}

Value get_last_pop() { return vm.last_pop; }
Module *get_cur_mod() { return vm.current_mod; }

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
  //setlocale(LC_CTYPE, "");
  // wprintf(format);
  va_list args;
  va_start(args, format);
  vfwprintf(stderr, format, args);

  va_end(args);
  fputwc('\n', stderr);

  for (int i = get_cur_mod()->frame_count - 1; i >= 0; i--) {
    CallFrame *frm = &get_cur_mod()->frames[i];
    //&vm.frames[i];
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

CallFrame *get_cur_farme() {
  return &get_cur_mod()->frames[get_cur_mod()->frame_count - 1];
}
uint8_t read_bt(CallFrame *frame) {
  uint8_t result = *frame->ip++;
  // wprintf(L"<<<<<<<<< BT ->>> %s\n", print_opcode(result));
  return result;
}
uint8_t peek_bt(CallFrame *frame) {
  uint8_t result = read_bt(frame);
  frame->ip--;
  return result;
}
uint16_t read_u16(CallFrame *frame) {
  CallFrame *cf = frame; // get_cur_farme();
  cf->ip += 2;
  return (uint16_t)((cf->ip[-2] << 8) | cf->ip[-1]);
}
Value read_const(CallFrame *frame) {
  return frame->closure->func->ins.consts.values[read_bt(frame)];
}
ObjString *read_str_const(CallFrame *frame) {
  return get_as_string(read_const(frame));
}
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

bool bin_gte() {
  if (!is_num(peek_vm(0)) || !is_num(peek_vm(1))) {
    runtime_err(L"Operands must be numbers for greater than equal operation");
    return false;
  }

  double r = get_as_number(pop());
  double l = get_as_number(pop());
  push(make_bool(l >= r));
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

bool bin_lte() {
  if (!is_num(peek_vm(0)) || !is_num(peek_vm(1))) {
    runtime_err(L"Operands must be numbers for less than equal operation");
    return false;
  }
  double r = get_as_number(pop());
  double l = get_as_number(pop());
  push(make_bool(l <= r));
  return true;
}

void define_stdlib_fn(wchar_t *name, NativeFn func) {
  // push(make_obj_val(copy_string(name, (int)wcslen(name))));
  // push(;
  Value k = make_obj_val(copy_string(name, (int)wcslen(name)));
  Value fn = make_obj_val(new_native(func));
#ifdef DEBUG_STACK
  wprintf(L"------ STACK in Define native----\n");
  for (Value *slt = vm.stack; slt < vm.stack_top; slt++) {
    wprintf(L"[ ");
    print_val(*slt);
    wprintf(L" ]\n");
  }
  wprintf(L"--- END STACK ---\n");
#endif
  table_set(&get_cur_mod()->globals, get_as_string(k), fn);
  // pop();
  // pop();
}

Value math_add_stdlib(int argc, Value *args) {
  if (argc != 2) {
    runtime_err(L"add function only takes two arguments");
    return make_nil();
  }

  if (is_num(args[0]) && is_num(args[1])) {
    double l = get_as_number(args[0]);
    double r = get_as_number(args[1]);
    return make_num(l + r);
  } else {
    runtime_err(L"add function only works on numbers");
    return make_nil();
  }
}
Value math_pow_stdlib(int argc, Value *args) {
  if (argc != 2) {
    runtime_err(L"pow function only takes two arguments");
    return make_nil();
  }

  if (is_num(args[0]) && is_num(args[1])) {
    double l = get_as_number(args[0]);
    double r = get_as_number(args[1]);

    return make_num(pow(l, r));
  } else {
    runtime_err(L"pow function only works on numbers");
    return make_nil();
  }
}

static bool import_custom(wchar_t *import_name) {
  wchar_t *dummy_source_code = NULL;
  if (wcscmp(import_name, L"x") == 0) {

    dummy_source_code = L"let name = \"palash\";";
  } else {

    dummy_source_code = L"fun hello(x) let pi = 100; show \"hello \" + x; end";
  }
  Module *mod = &vm.modules[vm.mod_count++];
  mod->is_default = false;
  mod->origin = get_cur_mod();

  init_module(mod, import_name);

  // wprintf(L"is globals null -> %s\n",
  //         mod->globals.entries == NULL ? "true" : "false");
  int origin_caller = get_cur_mod()->frame_count - 1;
  ObjMod *objmod = new_mod(copy_string(import_name, wcslen(import_name)));
  vm.mod_names[vm.mod_count - 1] = objmod->name->hash;
  push(make_obj_val(mod));
  ObjString *strname = copy_string(import_name, wcslen(import_name));

  table_set(&get_cur_mod()->globals, strname, make_obj_val(objmod));
  vm.current_mod = mod; // vm.mod_count - 1;
  ObjFunc *newfn = compile_module(dummy_source_code);
  //dissm_ins_chunk(&newfn->ins, import_name);
  // write_ins(&newfn->ins, OP_END_MOD, 9999);
  if (newfn == NULL) {
    return false;
  }

  // dissm_ins_chunk(&newfn->ins, L"MODULE");

  push(make_obj_val(newfn));
  ObjClosure *cls = new_closure(newfn, objmod->name->hash);
  cls->global_owner = objmod->name->hash;
  cls->globals = &mod->globals;
  pop();
  // push(make_obj_val(cls));
  // wprintf(L"BEFORE MOD CALL\n");
  // print_modframes();
  call(cls, origin_caller, 0);
  // wprintf(L"AFTER MOD CALL\n");
  // print_modframes();

  return true;
}

static bool import_file(wchar_t *import_name) {
  if (wcscmp(import_name, L"math") == 0) {
    define_stdlib_fn(L"add", math_add_stdlib);
    define_stdlib_fn(L"pow", math_pow_stdlib);
    return true;
  } else {
    return import_custom(import_name);
  }
}

void print_stack() {
  wprintf(L"------ STACK ----\n");
  for (Value *slt = vm.stack; slt < vm.stack_top; slt++) {
    wprintf(L"[ ");
    print_val(*slt);
    wprintf(L" ]\n");
  }
  wprintf(L"--- END STACK ---\n");
}

IResult run_vm() {
  CallFrame *frame = &get_cur_mod()->frames[get_cur_mod()->frame_count - 1];
  for (;;) {
#ifdef DEBUG_STACK
    wprintf(L"------ STACK ----\n");
    for (Value *slt = vm.stack; slt < vm.stack_top; slt++) {
      wprintf(L"[ ");
      print_val(*slt);
      wprintf(L" ]\n");
    }
    wprintf(L"--- END STACK ---\n");
#endif
#ifdef DEBUG_TRACE

    dissm_ins(&frame->closure->func->ins,
              (int)(frame->ip - frame->closure->func->ins.code));

#endif
    // wprintf(L"----------------\nCURMOD ->%ls\n" , get_cur_mod()->name);
    // wprintf(L"FRAME X -> %ld\n" , get_cur_farme()->global_owner);
    // wprintf(L"FRAME OWNER ->%ld\n" , frame->global_owner);
    uint8_t ins = read_bt(frame);
    // wprintf(L"INS -> %s\n--------------\n" , print_opcode(ins));
    switch (ins) {
    case OP_END_MOD:
      pop(); // in modules last nil still exists
      get_cur_mod()->frame_count--;

      vm.current_mod = get_cur_mod()->origin;

      frame = &vm.current_mod->frames[vm.current_mod->frame_count - 1];
      continue;
      // vm.current_mod--;
      // frame = &get_cur_mod()->frames[get_cur_mod()->frame_count - 1];
      // break;
    case OP_RETURN: {
      // int origin = frame->origin;

      // for (int i = 0; i <= vm.mod_count; i++) {
      //   wprintf(L"R | module %ls has %d frames \n", vm.modules[i].name,
      //           vm.modules[i].frame_count);
      // }

      Value res = pop();
      // Module *mod = get_mod_by_hash(frame->global_owner);
      // if (peek_bt(frame) == OP_END_MOD) {
      // read_bt(frame);
      //  wprintf(L"PEEEEEEK\n");
      // get_cur_mod()->frame_count--;
      // vm.current_mod = mod->origin;
      // continue;
      //  break;
      //}
      close_upval(get_cur_mod(), frame->slots);

      get_cur_mod()->frame_count--;
      // vm.frame_count--;

      if (get_cur_mod()->frame_count == 0 && vm.current_mod->is_default) {
        pop();
        return INTRP_OK;
      }

      vm.stack_top = frame->slots;
      push(res);
      frame = &get_cur_mod()->frames[get_cur_mod()->frame_count - 1];

      break;
      //  print_val(pop());
      //  wprintf(L"\n");
      // return INTRP_OK;
    }
    case OP_CONST: {
      Value con = read_const(frame);
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
    case OP_GTE: {
      if (!bin_gte()) {
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

    case OP_LTE: {
      if (!bin_lte()) {
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
      vm.last_pop = to_show;
      print_val(to_show);
      wprintf(L"\n");
      break;
    case OP_DEF_GLOB: {
      ObjString *nm = read_str_const(frame);
      table_set(frame->globals, nm, peek_vm(0));
      /*for (int i = 0; i <= vm.mod_count; i++) {
        Module * mod = &vm.modules[i];
        for (int j = 0; j < mod->frame_count ; j++) {
          Htable * tbl = mod->frames[j].globals;
          print_table(tbl , "frames");
         }
      }*/

      pop();
      break;
    }
    case OP_GET_GLOB: {
      ObjString *name = read_str_const(frame);
      // wprintf(L"GET_GLOB name -> %ls\n" , name->chars);
      // print_table(&vm.globals , "global");
      // print_obj();
      Value val;
      if (!table_get(frame->globals, name, &val)) {
        runtime_err(L"Get Global -> Undefined variable '%ls'.", name->chars);
        return INTRP_RUNTIME_ERR;
      }
      push(val);
      break;
    }

    case OP_SET_GLOB: {
      ObjString *name = read_str_const(frame);
      wprintf(L"setting global -> %ls -> %ld\n", name->chars,
              frame->global_owner);
      if (table_set(frame->globals, name, peek_vm(0))) {
        table_del(frame->globals, name);
        runtime_err(L"Set Global -> Undefined var '%ls'", name->chars);
        return INTRP_RUNTIME_ERR;
      }
      break;
    }

    case OP_GET_LOCAL: {
      uint8_t slot = read_bt(frame);
      // push(vm.stack[slot]);
      push(frame->slots[slot]);
      break;
    }
    case OP_SET_LOCAL: {
      uint8_t slot = read_bt(frame);
      frame->slots[slot] = peek_vm(0);
      break;
    }
    case OP_JMP_IF_FALSE: {
      uint16_t offset = read_u16(frame);
      // wprintf(L"JIF -> %d\n" , offset);
      if (is_falsey(peek_vm(0))) {
        frame->ip += offset;
      }
      break;
    }
    case OP_JMP: {
      uint16_t offset = read_u16(frame);
      frame->ip += offset;
      break;
    }
    case OP_LOOP: {
      uint16_t offset = read_u16(frame);
      frame->ip -= offset;
      break;
    }
    case OP_CLOSURE: {
      ObjFunc *func = get_as_func(read_const(frame));
      ObjClosure *cls = new_closure(func, frame->global_owner);
      // cls->global_owner = vm.mod_names[vm.current_mod];
      // cls->globals = &vm.modules[vm.current_mod].globals;
      push(make_obj_val(cls));
      Module *mod = get_mod_by_hash(frame->global_owner);
      for (int i = 0; i < cls->upv_count; i++) {
        uint16_t is_local = read_bt(frame);
        uint8_t index = read_bt(frame);
        if (is_local) {
          cls->upv[i] = capture_upv(mod, frame->slots + index);
          // capture
        } else {
          cls->upv[i] = frame->closure->upv[index];
        }
      }
      // frame->globals = cls->globals;
      // frame->global_owner = cls->global_owner;
      // wprintf(L"closure -> %ld == %ld\n" , cls->global_owner ,
      // frame->global_owner);
      break;
    }
    case OP_GET_UP: {
      uint8_t slot = read_bt(frame);
      push(*frame->closure->upv[slot]->location);
      break;
    }
    case OP_SET_UP: {
      uint8_t slot = read_bt(frame);
      *frame->closure->upv[slot]->location = peek_vm(0);
      break;
    }
    case OP_CLS_UP: {
      Module *mod = get_mod_by_hash(frame->global_owner);
      close_upval(mod, vm.stack_top - 1);
      pop();
      break;
    }
    case OP_CALL: {
      int argc = read_bt(frame);
      if (!call_val(peek_vm(argc), argc)) {
        return INTRP_RUNTIME_ERR;
      }

      frame = &get_cur_mod()->frames[get_cur_mod()->frame_count - 1];
      break;
    }
    case OP_IMPORT_NONAME: {
      // print_stack();
      Value raw_file_name = pop();
      if (!is_str_obj(raw_file_name)) {
        runtime_err(L"import file name must be string");
        return INTRP_RUNTIME_ERR;
      }
      ObjString *filename = get_as_string(raw_file_name);
      if (!import_file(filename->chars)) {
        runtime_err(L"local file importing does not work yet;\
        only a subset of stdlib is available\n");
        return INTRP_RUNTIME_ERR;
      }

      frame = &get_cur_mod()->frames[get_cur_mod()->frame_count - 1];
      // wprintf(L"after import -> %ld\n" , get_cur_mod()->frame_count);
      // else {
      //   frame = &get_cur_mod()->frames[get_cur_mod()->frame_count - 1];
      //   continue;
      // }

      // vm.current_mod--;

      // print_val(import_file_name);
      break;
    }
    case OP_GET_MOD_PROP: {
      // print_stack();
      // print_val(peek_vm(0));
      ObjMod *modname = get_as_mod(peek_vm(0));

      ObjString *prop = read_str_const(frame);
      Value value;
      // wprintf(L"MODNAME -> %ls\n" , modname->name->chars);
      // current_owner = modname->name->hash;
      Module *mod = get_mod_by_hash(modname->name->hash);

      if (mod == NULL) {
        runtime_err(L"module not found %ls\n", modname->name->chars);
        return INTRP_RUNTIME_ERR;
      }

      // print_table(&mod->globals, "curmodglobals");
      // print_table(frame->globals, "frame modules");

      if (table_get(&mod->globals, prop, &value)) {
        pop();
        push(value);
        break;
      } else {
        runtime_err(L"Error method or variable '%ls' not found for module %ls",
                    prop->chars, modname->name->chars);
        return INTRP_RUNTIME_ERR;
      }
    }
    }
  }
  return INTRP_RUNTIME_ERR;
}

void close_upval(Module *module, Value *last) {
  while (module->open_upvs != NULL && module->open_upvs->location >= last) {
    ObjUpVal *upval = module->open_upvs;
    upval->closed = *upval->location;
    upval->location = &upval->closed;
    module->open_upvs = upval->next;
  }
}

ObjUpVal *capture_upv(Module *module, Value *local) {
  ObjUpVal *prev = NULL;
  ObjUpVal *upv = module->open_upvs;
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
    module->open_upvs = new_upv;
  } else {
    prev->next = new_upv;
  }
  return new_upv;
}

Value clock_ntv_fn(int argc, Value *args) {
  return make_num((double)clock() / CLOCKS_PER_SEC);
}

bool call_val(Value calle, int argc) {
  int origin = get_cur_mod()->frame_count - 1;
  if (is_obj(calle)) {
    switch (get_obj_type(calle)) {
    case OBJ_CLOUSRE:
      return call(get_as_closure(calle), origin, argc);
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

bool call(ObjClosure *closure, int origin, int argc) {
  if (closure->func->arity != argc) {
    runtime_err(L"Expected %d args but got %d", closure->func->arity, argc);
    return false;
  }

  if (get_cur_mod()->frame_count == FRAME_SIZE) {
    runtime_err(L"Too many frames! Stack overflow");
    return false;
  }

  CallFrame *frame = &get_cur_mod()->frames[get_cur_mod()->frame_count++];
  frame->closure = closure;
  frame->ip = closure->func->ins.code;
  frame->slots = vm.stack_top - argc - 1;
  // frame->origin = origin;
  //  print_mod_names();
  frame->global_owner = closure->global_owner;
  frame->globals = closure->globals;

  // wprintf(L"CALL | current mod argc -> %d | vm -> %ld | frame -> %ld|\n\n" ,
  // argc, vm.mod_names[vm.current_mod] , frame->global_owner);
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
  ObjClosure *cls = new_closure(fn, 0);
  pop();
  push(make_obj_val(cls));
  cls->global_owner = 0;
  cls->globals = &vm.modules[0].globals;

  call(cls, 0, 0);
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
