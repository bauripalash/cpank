#ifndef cpank_instruction_h
#define cpank_instruction_h

#include "common.h"
#include "value.h"
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
  OP_LT,
  OP_SHOW,
  OP_POP,
  OP_DEF_GLOB,
  OP_SET_GLOB,
  OP_GET_GLOB,
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

#endif
