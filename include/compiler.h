#ifndef cpank_compiler_h
#define cpank_compiler_h

#include "common.h"
#include "instruction.h"
#include "lexer.h"
#include "obj.h"
#include <stdbool.h>
#include <stdint.h>
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

typedef struct {
  Token name;
  int depth;
} Local;

typedef enum {
  FTYPE_FUNC,
  FTYPE_SCRIPT,
} FuncType;

typedef struct Compiler {
  ObjFunc *func;
  FuncType type;
  struct Compiler *enclosing;
  Local locals[UINT8_COUNT];
  int local_count;
  int scope_depth;
} Compiler;

void init_comiler(Compiler *compiler, FuncType type);
void start_scope();
void end_scope();
void read_block();
void declare_var();
void add_local(Token name);
bool id_eq(Token *l, Token *r);
int resolve_local(Compiler *compiler, Token *token);
void mark_init();
void read_if_stmt();
int emit_jump(uint8_t ins);
void patch_jump(int offset);
void read_and(bool can_assign);
void read_or(bool can_assign);
void read_while_stmt();
void emit_loop(int ls);
void read_call(bool can_assign);
uint8_t read_arg_list();
typedef void (*ParseFn)(bool can_assign);

typedef struct {
  ParseFn prefix;
  ParseFn infix;
  Prec prec;
} ParseRule;

ParseRule *get_parse_rule(TokType tt);

ObjFunc *compile(wchar_t *source);
void read_expr();
void read_stmt();
void read_declr();
bool match_tok(TokType tt);
void read_print_stmt();
void read_expr_stmt();
uint8_t make_id_const(Token *name);
uint8_t parse_var(wchar_t *errmsg);
void let_declr();
void funct_declr();
void build_func(FuncType type);
void define_var(uint8_t global);
void read_var(bool can_assign);
void named_var(Token name, bool can_assign);
void return_stmt();
ObjFunc *end_compiler();
#endif
