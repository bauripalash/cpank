#include "include/compiler.h"
#include "include/common.h"
#include "include/instruction.h"
#include "include/mem.h"
#include "include/obj.h"
#include "include/value.h"

#ifdef DEBUG_PRINT_CODE
#include "include/debug.h"
#endif

// #define DEBUG_LEXER

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

Parser parser;
Compiler *current = NULL;
// Instruction *compins;

void init_comiler(Compiler *compiler, FuncType type) {
  compiler->enclosing = current;
  compiler->func = NULL;
  compiler->type = type;
  compiler->local_count = 0;
  compiler->scope_depth = 0;
  compiler->func = new_func();
  current = compiler;

  if (type != FTYPE_SCRIPT) {
    current->func->name = copy_string(parser.prev.start, parser.prev.length);
  }

  Local *local = &current->locals[current->local_count++];
  local->depth = 0;
  local->name.start = L"";
  local->name.length = 0;
}

Instruction *cur_ins() { return &current->func->ins; }
void err_at(Token *tok, wchar_t *msg) {
  if (parser.panic_mode) {

    return;
  }

  parser.panic_mode = true;
  fwprintf(stderr, L"[l %d] Error ", tok->line);

  if (tok->type == T_EOF) {
    fwprintf(stderr, L"at end");
  } else if (tok->type == T_ERR) {

  } else {
    fwprintf(stderr, L" at %.*ls", tok->length, tok->start);
  }

  fwprintf(stderr, L" : %ls\n\n", msg);
  parser.had_err = true;
}

void err(wchar_t *msg) { err_at(&parser.prev, msg); }

void err_at_cur(wchar_t *msg) { err_at(&parser.cur, msg); }

void advance() {
  parser.prev = parser.cur;
  for (;;) {
    parser.cur = get_tok();
    if (parser.cur.type != T_ERR) {
      break;
    } else {
      err_at_cur(parser.cur.start);
    }
  }

  // wprintf(L"CUR->%s\n" , toktype_to_string(parser.cur.type));
}

void sync_errors() {
  parser.panic_mode = false;
  while (parser.cur.type != T_EOF) {
    if (parser.prev.type == T_SEMICOLON) {
      return;
    }

    switch (parser.cur.type) {
    case T_FUNC:
    case T_LET:
    case T_IF:
    case T_WHILE:
    case T_SHOW:
    case T_RETURN:
      return;
    default:;
    }
    advance();
  }
}

// eat current token;
void eat_tok(TokType tt, wchar_t *errmsg) {
  if (parser.cur.type == tt) {
    advance();
    return;
  }

  err_at_cur(errmsg);
}

// Checks 'current' token;
bool check_tok(TokType tt) { return parser.cur.type == tt; }

// Matches 'current' token
bool match_tok(TokType tt) {
  if (!check_tok(tt)) {
    return false;
  } else {
    advance();
    return true;
  }
}

void emit_bt(uint8_t bt) { write_ins(cur_ins(), bt, parser.prev.line); }

void emit_two(uint8_t bt_1, uint8_t bt_2) {
  emit_bt(bt_1);
  emit_bt(bt_2);
}

void emit_return() {
  emit_bt(OP_NIL);
  emit_bt(OP_RETURN);
}

uint8_t make_const(Value val) {
  int con = add_const(cur_ins(), val);
  if (con > UINT8_MAX) {
    err(L"Too many constants");
    return 0;
  }

  return (uint8_t)con;
}

void emit_const(Value value) { emit_two(OP_CONST, make_const(value)); }

void parse_prec(Prec prec) {
  advance();
  ParseFn pref_rule = get_parse_rule(parser.prev.type)->prefix;
  if (pref_rule == NULL) {
    err(L"Expected expression");
    return;
  }
  bool can_assign = prec <= PREC_ASSIGN;
  pref_rule(can_assign);

  while (prec <= get_parse_rule(parser.cur.type)->prec) {
    advance();
    ParseFn infix_rule = get_parse_rule(parser.prev.type)->infix;
    infix_rule(can_assign);
  }

  if (can_assign && match_tok(T_EQ)) {
    err(L"Invalid asssignment");
  }
}

void read_number(bool can_assign) {
  // wprintf(L"NUMBER -> %ls\n" , parser.prev.start);
  // double val = strtod(, NULL);
  double val = wcstod(parser.prev.start, NULL);
  emit_const(make_num(val));
}

void read_string(bool can_assign) {
  Value v =
      make_obj_val(copy_string(parser.prev.start + 1, parser.prev.length - 2));
  emit_const(v);
}

void read_expr() { parse_prec(PREC_ASSIGN); }
void read_stmt() {
  if (match_tok(T_SHOW)) {
    read_print_stmt();
  } else if (match_tok(T_IF)) {
    read_if_stmt();
  } else if (match_tok(T_WHILE)) {
    read_while_stmt();
  } else if (match_tok(T_RETURN)) {
    return_stmt();
  } else if (match_tok(T_LBRACE)) {
    start_scope();
    read_block();
    end_scope();
  } else {
    read_expr_stmt();
  }
}

void return_stmt() {

  if (current->type == FTYPE_SCRIPT) {
    err(L"cannot return from top-level code");
  }

  if (match_tok(T_SEMICOLON)) {
    emit_return();
  } else {
    read_expr();
    eat_tok(T_SEMICOLON, L"expected ';' after return value");
    emit_bt(OP_RETURN);
  }
}

void read_expr_stmt() {
  read_expr();
  eat_tok(T_SEMICOLON, L"Expected ';' after expression stmt");
  emit_bt(OP_POP);
}

void read_print_stmt() {
  read_expr();
  eat_tok(T_SEMICOLON, L"expected ';' after show stmt");
  emit_bt(OP_SHOW);
}

void read_declr() {
  if (match_tok(T_LET)) {
    let_declr();
  } else if (match_tok(T_FUNC)) {
    funct_declr();
  } else {
    read_stmt();
  }
  if (parser.panic_mode) {
    sync_errors();
  }
}

void let_declr() {
  uint8_t global = parse_var(L"Expected variable name after let");
  if (match_tok(T_EQ)) {
    read_expr();
  } else {
    emit_bt(OP_NIL);
  }

  eat_tok(T_SEMICOLON, L"expected ';' after variable declr");
  define_var(global);
}

void funct_declr() {
  uint8_t global = parse_var(L"Expected function name");
  mark_init();
  build_func(FTYPE_FUNC);
  define_var(global);
}

void build_func(FuncType type) {
  Compiler new_compiler;
  init_comiler(&new_compiler, type);
  start_scope();
  eat_tok(T_LPAREN, L"expected '(' after func name");

  if (!check_tok(T_RPAREN)) {
    do {
      current->func->arity++;
      if (current->func->arity > 256) {
        err_at_cur(L"too many function params");
      }
      uint8_t con = parse_var(L"Expected param name");
      define_var(con);
    } while (match_tok(T_COMMA));
  }

  eat_tok(T_RPAREN, L"expected ')' after func params");
  // TODO: END
  eat_tok(T_LBRACE, L"expected '{' before func body");
  read_block();
  ObjFunc *fn = end_compiler();
  emit_two(OP_CLOSURE, make_const(make_obj_val(fn)));

  for (int i = 0; i < fn->up_count; i++) {
    emit_bt(new_compiler.upvs[i].is_local ? 1 : 0);
    emit_bt(new_compiler.upvs[i].index);
  }
}

uint8_t parse_var(wchar_t *errmsg) {
  eat_tok(T_ID, errmsg);
  declare_var();
  if (current->scope_depth > 0) {
    return 0;
  }
  return make_id_const(&parser.prev);
}

int resolve_upval(Compiler *compiler, Token *name) {
  if (compiler->enclosing == NULL) {
    return -1;
  }

  int local = resolve_local(compiler->enclosing, name);
  if (local != -1) {
    compiler->enclosing->locals[local].is_captd = true;
    return add_upval(compiler, (uint8_t)local, true);
  }

  int upv = resolve_upval(compiler->enclosing, name);
  if (upv != -1) {
    return add_upval(compiler, (uint8_t)upv, false);
  }

  return -1;
}

int add_upval(Compiler *compiler, uint8_t index, bool is_local) {
  int upc = compiler->func->up_count;

  for (int i = 0; i < upc; i++) {
    Upval *upv = &compiler->upvs[i];
    if (upv->index == index && upv->is_local == is_local) {
      return i;
    }
  }

  if (upc == UINT8_COUNT) {
    err(L"Too many closure vars");
    return 0;
  }

  compiler->upvs[upc].is_local = is_local;
  compiler->upvs[upc].index = index;
  return compiler->func->up_count++;
}

void define_var(uint8_t global) {

  if (current->scope_depth > 0) {
    mark_init();
    return;
  }
  emit_two(OP_DEF_GLOB, global);
}

void mark_init() {
  if (current->scope_depth == 0) {
    return;
  }
  current->locals[current->local_count - 1].depth = current->scope_depth;
}

void declare_var() {
  if (current->scope_depth == 0) {
    return;
  }

  Token *name = &parser.prev;
  for (int i = current->local_count - 1; i >= 0; i--) {
    Local *local = &current->locals[i];
    if (local->depth != -1 && local->depth < current->scope_depth) {
      break;
    }

    if (id_eq(name, &local->name)) {
      err(L"already a variable with this name in the current scope");
    }
  }
  add_local(*name);
}

bool id_eq(Token *l, Token *r) {
  if (l->length != r->length) {
    return false;
  }

  return wmemcmp(l->start, r->start, l->length) == 0;
}

void add_local(Token name) {
  if (current->local_count == UINT8_COUNT) {
    err(L"too many local vars");
    return;
  }
  Local *local = &current->locals[current->local_count++];
  local->name = name;
  local->depth = -1;
  local->is_captd = false;
  //local->depth = current->scope_depth;
}

void read_var(bool can_assign) { named_var(parser.prev, can_assign); }

void named_var(Token name, bool can_assign) {
  // uint8_t arg = make_id_const(&name);
  uint8_t get_op, set_op;
  int arg = resolve_local(current, &name);
  if (arg != -1) {
    get_op = OP_GET_LOCAL;
    set_op = OP_SET_LOCAL;
  } else if ((arg = resolve_upval(current, &name)) != -1) {
    get_op = OP_GET_UP;
    set_op = OP_SET_UP;
  } else {
    arg = make_id_const(&name);
    get_op = OP_GET_GLOB;
    set_op = OP_SET_GLOB;
  }

  // wprintf(L"NAMED_VAR -> %s\n" , can_assign ? "true" : false);
  if (can_assign && match_tok(T_EQ)) {
    read_expr();
    emit_two(set_op, (uint8_t)arg);
  } else {
    emit_two(get_op, (uint8_t)arg);
  }
}

int resolve_local(Compiler *compiler, Token *token) {
  for (int i = compiler->local_count - 1; i >= 0; i--) {
    Local *local = &compiler->locals[i];
    if (id_eq(token, &local->name)) {
      if (local->depth == -1) {
        err(L"Can't read local var in its own initializer.");
      }
      return i;
    }
  }
  return -1;
}

void read_to_end() {

  while (!check_tok(T_END) && !check_tok(T_EOF)) {
    read_declr();
  }

  eat_tok(T_END, L"Expected 'end' after stmts");
}

/*got end?*/
void read_if_block() {
  start_scope();
  while (!check_tok(T_END) && !check_tok(T_ELSE) && !check_tok(T_EOF)) {
    read_declr();
  }

  end_scope();
}

void read_if_stmt() {
  read_expr();
  eat_tok(T_THEN, L"expected 'then'  after if expression");

  int then_jump = emit_jump(OP_JMP_IF_FALSE);
  emit_bt(OP_POP);

  read_if_block();

  int else_jmp = emit_jump(OP_JMP);
  patch_jump(then_jump);
  emit_bt(OP_POP);
  if (match_tok(T_ELSE)) {
    start_scope();
    read_to_end();
    end_scope();
  } else {
    eat_tok(T_END, L"Expected end after if without else");
  }

  patch_jump(else_jmp);
}

void read_while_stmt() {
  int ls = cur_ins()->len;
  eat_tok(T_LPAREN, L"expected '(' after 'while'");
  read_expr();
  eat_tok(T_RPAREN, L"expected ')' after expression");
  int exitjmp = emit_jump(OP_JMP_IF_FALSE);
  emit_bt(OP_POP);
  start_scope();
  read_to_end();
  end_scope();
  // read_stmt();
  emit_loop(ls);
  patch_jump(exitjmp);
  emit_bt(OP_POP);
}

void emit_loop(int ls) {
  emit_bt(OP_LOOP);
  int offset = cur_ins()->len - ls + 2;
  if (offset > UINT16_MAX) {
    err(L"loop body too big");
  }
  emit_bt((offset >> 8) & 0xFF);
  emit_bt(offset & 0xFF);
}

void read_and(bool can_assign) {
  int ej = emit_jump(OP_JMP_IF_FALSE);
  emit_bt(OP_POP);
  parse_prec(PREC_AND);
  patch_jump(ej);
}

void read_or(bool can_assign) {
  int else_jmp = emit_jump(OP_JMP_IF_FALSE);
  int endjmp = emit_jump(OP_JMP);

  patch_jump(else_jmp);
  emit_bt(OP_POP);
  parse_prec(PREC_OR);
  patch_jump(endjmp);
}

void read_call(bool can_assign) {
  uint8_t arg_count = read_arg_list();
  emit_two(OP_CALL, arg_count);
}

uint8_t read_arg_list() {
  uint8_t argc = 0;
  if (!check_tok(T_RPAREN)) {
    do {
      read_expr();
      if (argc == 255) {
        err(L"Too many arguments; more than 255");
      }
      argc++;
    } while (match_tok(T_COMMA));
  }

  eat_tok(T_RPAREN, L"Expected ')' after arguments");
  return argc;
  ;
}

int emit_jump(uint8_t ins) {
  emit_bt(ins);
  emit_two(0xFF, 0xFF);
  return cur_ins()->len - 2;
}

void patch_jump(int offset) {
  int jmp = cur_ins()->len - offset - 2;
  if (jmp > UINT16_MAX) {
    err(L"too big jump");
  }

  cur_ins()->code[offset] = (jmp >> 8) & 0xFF;
  cur_ins()->code[offset + 1] = jmp & 0xFF;
}

uint8_t make_id_const(Token *name) {
  return make_const(make_obj_val(copy_string(name->start, name->length)));
}

void read_group(bool can_assign) {
  read_expr();
  eat_tok(T_RPAREN, L"Expected ')' after group expression");
}

void read_unary(bool can_assign) {
  TokType op = parser.prev.type;

  // read_expr();
  parse_prec(PREC_UNARY);
  switch (op) {
  case T_MINUS:
    emit_bt(OP_NEG);
    break;
  case T_BANG:
    emit_bt(OP_NOT);
    break;
  default:
    return;
  }
}

void read_binary(bool can_assign) {
  TokType op = parser.prev.type;
  ParseRule *prule = get_parse_rule(op);
  parse_prec((Prec)(prule->prec + 1));
  switch (op) {
  case T_PLUS:
    emit_bt(OP_ADD);
    break;
  case T_MINUS:
    emit_bt(OP_SUB);
    break;
  case T_ASTR:
    emit_bt(OP_MUL);
    break;
  case T_DIV:
    emit_bt(OP_DIV);
    break;
  case T_NOTEQ:
    emit_two(OP_EQ, OP_NOT);
    break;
  case T_EQEQ:
    emit_bt(OP_EQ);
    break;
  case T_GT:
    emit_bt(OP_GT);
    break;
  case T_GTE:
    emit_two(OP_LT, OP_NOT);
    break;
  case T_LT:
    emit_bt(OP_LT);
    break;
  case T_LTE:
    emit_two(OP_GT, OP_NOT);
    break;
  default:
    return;
  }
}

void literal(bool can_assign) {
  switch (parser.prev.type) {
  case T_FALSE:
    emit_bt(OP_FALSE);
    break;
  case T_NIL:
    emit_bt(OP_NIL);
    break;
  case T_TRUE:
    emit_bt(OP_TRUE);
    break;
  default:
    return;
  }
}

void read_block() {
  while (!check_tok(T_RBRACE) && !check_tok(T_EOF)) {
    read_declr();
  }

  eat_tok(T_RBRACE, L"Expected '}' after block stmts");
}

void start_scope() { current->scope_depth++; }

void end_scope() {
  current->scope_depth--;
  while (current->local_count > 0 &&
         current->locals[current->local_count - 1].depth >
             current->scope_depth) {
    if (current->locals[current->local_count - 1].is_captd) {
      emit_bt(OP_CLS_UP);
    } else {
      emit_bt(OP_POP);
    }
    current->local_count--;
  }
}

ParseRule parse_rules[] = {
    [T_LPAREN] = {read_group, read_call, PREC_CALL},
    [T_RPAREN] = {NULL, NULL, PREC_NONE},
    [T_LBRACE] = {NULL, NULL, PREC_NONE},
    [T_RBRACE] = {NULL, NULL, PREC_NONE},
    [T_COMMA] = {NULL, NULL, PREC_NONE},
    // T_DOT
    [T_MINUS] = {read_unary, read_binary, PREC_TERM},
    [T_PLUS] = {NULL, read_binary, PREC_TERM},
    [T_SEMICOLON] = {NULL, NULL, PREC_NONE},
    [T_DIV] = {NULL, read_binary, PREC_FACT},
    [T_ASTR] = {NULL, read_binary, PREC_FACT},
    [T_BANG] = {read_unary, NULL, PREC_NONE},
    [T_NOTEQ] = {NULL, read_binary, PREC_EQ},
    [T_EQ] = {NULL, NULL, PREC_NONE},
    [T_EQEQ] = {NULL, read_binary, PREC_EQ},
    [T_GT] = {NULL, read_binary, PREC_COMP},
    [T_GTE] = {NULL, read_binary, PREC_COMP},
    [T_LT] = {NULL, read_binary, PREC_COMP},
    [T_LTE] = {NULL, read_binary, PREC_COMP},
    [T_ID] = {read_var, NULL, PREC_NONE},
    [T_STR] = {read_string, NULL, PREC_NONE},
    [T_NUM] = {read_number, NULL, PREC_NONE},
    [T_AND] = {NULL, read_and, PREC_AND},
    [T_ELSE] = {NULL, NULL, PREC_NONE},
    [T_FALSE] = {literal, NULL, PREC_NONE},
    [T_FUNC] = {NULL, NULL, PREC_NONE},
    [T_IF] = {NULL, NULL, PREC_NONE},
    [T_NIL] = {literal, NULL, PREC_NONE},
    [T_OR] = {NULL, read_or, PREC_OR},
    [T_SHOW] = {NULL, NULL, PREC_NONE},
    [T_RETURN] = {NULL, NULL, PREC_NONE},
    [T_TRUE] = {literal, NULL, PREC_NONE},
    [T_LET] = {NULL, NULL, PREC_NONE},
    [T_WHILE] = {NULL, NULL, PREC_NONE},
    [T_ERR] = {NULL, NULL, PREC_NONE},
    [T_EOF] = {NULL, NULL, PREC_NONE},
};

ParseRule *get_parse_rule(TokType tt) { return &parse_rules[tt]; }

ObjFunc *end_compiler() {
  emit_return();
  ObjFunc *func = current->func;
#ifdef DEBUG_PRINT_CODE
  if (!parser.had_err) {
    dissm_ins_chunk(cur_ins(),
                    func->name != NULL ? func->name->chars : L"<script>");
  }
#endif
  current = current->enclosing;
  return func;
}

ObjFunc *compile(wchar_t *source) {
  boot_lexer(source);
  Compiler compiler;
  init_comiler(&compiler, FTYPE_SCRIPT);
  // compins = ins;
  parser.had_err = false;
  parser.panic_mode = false;
#ifdef DEBUG_LEXER
  Token tk = get_tok();
  while (tk.type != T_EOF) {
    wprintf(L"TOK[%s][%.*ls]\n", toktype_to_string(tk.type), tk.length,
            tk.start);
    tk = get_tok();
  }
  boot_lexer(source);
#endif
  // advance();
  advance();
  while (!match_tok(T_EOF)) {
    read_declr();
  }

  // wprintf(L"CUR->%s\n" , toktype_to_string(parser.cur.type));
  // read_expr();
  // advance();
  // eat_tok(T_EOF, L"Expected end of expr");
  ObjFunc *fn = end_compiler();
  return parser.had_err ? NULL : fn;
}

void mark_compiler_roots() {
  Compiler *compiler = current;
  while (compiler != NULL) {
#ifdef DEBUG_LOG_GC

    wprintf(L">>>>marking compiler function<<<<<\n");

#endif
    mark_obj((Obj *)compiler->func);

#ifdef DEBUG_LOG_GC

    wprintf(L">>>>finished marking compiler function<<<<<\n");

#endif
    compiler = compiler->enclosing;
  }
}
