#include "include/instruction.h"
#include "include/mem.h"
#include "include/value.h"
#include "include/vm.h"
#include <stdint.h>
#include <stdlib.h>

void init_instruction(Instruction *ins) {
  ins->len = 0;
  ins->cap = 0;
  ins->code = NULL;
  ins->lines = NULL;
  init_valarr(&ins->consts);
}

void free_ins(Instruction *ins) {
  FREE_ARR(uint8_t, ins->code, ins->cap);
  FREE_ARR(int, ins->lines, ins->cap);
  free_valarr(&ins->consts);
  init_instruction(ins);
}

void write_ins(Instruction *ins, uint8_t bt, int lines) {
  if (ins->cap < ins->len + 1) {
    int old_cap = ins->cap;
    ins->cap = GROW_CAP(old_cap);
    ins->code = GROW_ARR(uint8_t, ins->code, old_cap, ins->cap);
    ins->lines = GROW_ARR(int, ins->lines, old_cap, ins->cap);
  }

  ins->code[ins->len] = bt;
  ins->lines[ins->len] = lines;
  ins->len++;
}

int add_const(Instruction *ins, Value val) {
  push(val);
  write_valarr(&ins->consts, val);
  pop();
  return ins->consts.len - 1;
}
