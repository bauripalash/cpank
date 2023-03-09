#include "include/compiler.h"
#include "include/common.h"
#include "include/instruction.h"
#include "include/value.h"
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
  fprintf(stderr, "[l %d] Error ", tok->line);

  if (tok->type == T_EOF) {
    fprintf(stderr, "at end");
  } else if (tok->type == T_ERR) {

  } else {
    fprintf(stderr, " at %.*ls", tok->length, tok->start);
  }

  fprintf(stderr, " : %ls\n", msg);
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

void parse_prec(Prec prec) {}

void read_number() {
  // wprintf(L"NUMBER -> %ls\n" , parser.prev.start);
  // double val = strtod(, NULL);
  double val = wcstod(parser.prev.start, NULL);
  emit_const(val);
}

void read_expr() { parse_prec(PREC_ASSIGN); }

void read_group() {
  read_expr();
  eat_tok(T_RPAREN, L"Expected ')' after group expression");
}

void read_unary() {
  TokType op = parser.prev.type;

  //read_expr();
  parse_prec(PREC_UNARY);
  switch (op) {
  case T_MINUS:
    emit_bt(OP_NEG);
  default:
    return;
  }
}

ParseRule rules[] = {
  [T_LPAREN] = {read_group , NULL , PREC_NONE},
};


void end_compiler() { emit_return(); }

bool compile(wchar_t *source, Instruction *ins) {
  boot_lexer(source);
  compins = ins;
  parser.had_err = false;
  parser.panic_mode = false;
  advance();
  advance();

  // wprintf(L"CUR->%s\n" , toktype_to_string(parser.cur.type));
  read_expr();
  // advance();
  eat_tok(T_EOF, L"Expected end of expr");
  end_compiler();
  return !parser.had_err;
}
