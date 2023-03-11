#ifndef cpank_compiler_h
#define cpank_compiler_h

#include "instruction.h"
#include "lexer.h"
#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#define DEBUG_PRINT_CODE

typedef struct {
  Token cur;
  Token prev;
  bool had_err;
  bool panic_mode;
} Parser;

typedef enum {
  PREC_NONE,
  PREC_ASSIGN,
  PREC_OR,
  PREC_AND,
  PREC_EQ,
  PREC_COMP,
  PREC_TERM,
  PREC_FACT,
  PREC_UNARY,
  PREC_CALL,
  PREC_DEFAULT,
} Prec;

typedef void (*ParseFn)(bool can_assign);

typedef struct {
  ParseFn prefix;
  ParseFn infix;
  Prec prec;
} ParseRule;

ParseRule *get_parse_rule(TokType tt);

bool compile(wchar_t *source, Instruction *ins);
void read_expr();
void read_stmt();
void read_declr();
bool match_tok(TokType tt);
void read_print_stmt();
void read_expr_stmt();
uint8_t make_id_const(Token *name);
uint8_t parse_var(wchar_t *errmsg);
void let_declr();
void define_var(uint8_t global);
void read_var(bool can_assign);
void named_var(Token name, bool can_assign);

#endif
