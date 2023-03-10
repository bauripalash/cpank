#include "include/compiler.h"
#include "include/common.h"
#include "include/instruction.h"
#include "include/value.h"

#ifdef DEBUG_PRINT_CODE
#include "include/debug.h"
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

Parser parser;
Instruction *compins;

Instruction *cur_ins() { return compins; }
void err_at(Token *tok, wchar_t *msg) {
  if (parser.panic_mode) {
    return;

    parser.had_err = true;
  }
  fwprintf(stderr, L"[l %d] Error ", tok->line);

  if (tok->type == T_EOF) {
    fwprintf(stderr, L"at end");
  } else if (tok->type == T_ERR) {

  } else {
    fwprintf(stderr, L" at %.*ls", tok->length, tok->start);
  }

  fwprintf(stderr, L" : %ls\n\n", msg);
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

void eat_tok(TokType tt, wchar_t *errmsg) {
  if (parser.cur.type == tt) {
    advance();
    return;
  }

  err_at_cur(errmsg);
}

void emit_bt(uint8_t bt) { write_ins(cur_ins(), bt, parser.prev.line); }

void emit_two(uint8_t bt_1, uint8_t bt_2) {
  emit_bt(bt_1);
  emit_bt(bt_2);
}

void emit_return() { emit_bt(OP_RETURN); }

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

  pref_rule();

  while (prec <= get_parse_rule(parser.cur.type)->prec) {
    advance();
    ParseFn infix_rule = get_parse_rule(parser.prev.type)->infix;
    infix_rule();
  }
}

void read_number() {
  // wprintf(L"NUMBER -> %ls\n" , parser.prev.start);
  // double val = strtod(, NULL);
  double val = wcstod(parser.prev.start, NULL);
  emit_const(make_num(val));
}

void read_expr() { parse_prec(PREC_ASSIGN); }

void read_group() {
  read_expr();
  eat_tok(T_RPAREN, L"Expected ')' after group expression");
}

void read_unary() {
  TokType op = parser.prev.type;

  // read_expr();
  parse_prec(PREC_UNARY);
  switch (op) {
  case T_MINUS:
    emit_bt(OP_NEG);
  case T_BANG:
    emit_bt(OP_NOT);
  default:
    return;
  }
}

void read_binary() {
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

void literal() {
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

ParseRule parse_rules[] = {
    [T_LPAREN] = {read_group, NULL, PREC_NONE},
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
    [T_ID] = {NULL, NULL, PREC_NONE},
    [T_STR] = {NULL, NULL, PREC_NONE},
    [T_NUM] = {read_number, NULL, PREC_NONE},
    [T_AND] = {NULL, NULL, PREC_NONE},
    [T_ELSE] = {NULL, NULL, PREC_NONE},
    [T_FALSE] = {literal, NULL, PREC_NONE},
    [T_FUNC] = {NULL, NULL, PREC_NONE},
    [T_IF] = {NULL, NULL, PREC_NONE},
    [T_NIL] = {literal, NULL, PREC_NONE},
    [T_OR] = {NULL, NULL, PREC_NONE},
    [T_SHOW] = {NULL, NULL, PREC_NONE},
    [T_RETURN] = {NULL, NULL, PREC_NONE},
    [T_TRUE] = {literal, NULL, PREC_NONE},
    [T_LET] = {NULL, NULL, PREC_NONE},
    [T_WHILE] = {NULL, NULL, PREC_NONE},
    [T_ERR] = {NULL, NULL, PREC_NONE},
    [T_EOF] = {NULL, NULL, PREC_NONE},
};

ParseRule *get_parse_rule(TokType tt) { return &parse_rules[tt]; }

void end_compiler() {
  emit_return();

#ifdef DEBUG_PRINT_CODE
  if (!parser.had_err) {
    dissm_ins_chunk(cur_ins(), "code");
  }
#endif
}

bool compile(wchar_t *source, Instruction *ins) {
  boot_lexer(source);
  compins = ins;
  parser.had_err = false;
  parser.panic_mode = false;
  // advance();
  advance();

  // wprintf(L"CUR->%s\n" , toktype_to_string(parser.cur.type));
  read_expr();
  // advance();
  // eat_tok(T_EOF, L"Expected end of expr");
  end_compiler();
  return !parser.had_err;
}
