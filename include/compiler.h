#ifndef cpank_compiler_h
#define cpank_compiler_h

#include "instruction.h"
#include "lexer.h"
#include <stdbool.h>
#include <wchar.h>

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

typedef void (*ParseFn)() ;

typedef struct {
  ParseFn prefix; 
  ParseFn infix;
  Prec prec;
}ParseRule;


bool compile(wchar_t *source, Instruction *ins);

#endif
