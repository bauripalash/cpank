#ifndef cpank_instruction_h
#define cpank_instruction_h

#include "common.h"
#include "value.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  OP_RETURN,
  OP_CONST,
  OP_NEG,
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_NIL,
  OP_TRUE,
  OP_FALSE,
  OP_NOT,
  OP_EQ,
  OP_GT,
  OP_GTE,
  OP_LT,
  OP_LTE,
  OP_SHOW,
  OP_POP,
  OP_DEF_GLOB,
  OP_SET_GLOB,
  OP_GET_GLOB,
  OP_GET_LOCAL,
  OP_SET_LOCAL,
  OP_JMP_IF_FALSE,
  OP_JMP,
  OP_LOOP,
  OP_CALL,
  OP_CLOSURE,
  OP_GET_UP,
  OP_SET_UP,
  OP_CLS_UP,
  OP_IMPORT_NONAME,
  OP_SET_MOD_PROP,
  OP_GET_MOD_PROP,
  OP_END_MOD,

} Op;

typedef struct {
  int len;
  int cap;
  uint8_t *code;
  int *lines;
  Valarr consts;

} Instruction;

void init_instruction(Instruction *ins);
void write_ins(Instruction *ins, uint8_t bt, int line);
void free_ins(Instruction *ins);
int add_const(Instruction *ins, Value val);
bool make_changes_for_mod(Instruction *ins);
#endif
