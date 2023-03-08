#include <locale.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#include "bn.h"
#include "lexer.h"

const char *toktype_to_string(TokType t) {
  switch (t) {
  case T_LPAREN:
    return "T_LPAREN";
  case T_RPAREN:
    return "T_RPAREN";
  case T_LBRACE:
    return "T_LBRACE";
  case T_RBRACE:
    return "T_RBRACE";
  case T_MINUS:
    return "T_MINUS";
  case T_PLUS:
    return "T_PLUS";
  case T_DIV:
    return "T_DIV";
  case T_ASTR:
    return "T_ASTR";
  case T_SEMICOLON:
    return "T_SEMICOLON";
  case T_COMMA:
    return "T_COMMA";
  case T_EQ:
    return "T_EQ";
  case T_EQEQ:
    return "T_EQEQ";
  case T_GT:
    return "T_GT";
  case T_GTE:
    return "T_GTE";
  case T_LT:
    return "T_LT";
  case T_LTE:
    return "T_LTE";
  case T_ID:
    return "T_ID";
  case T_STR:
    return "T_STR";
  case T_NUM:
    return "T_NUM";
  case T_LET:
    return "T_LET";
  case T_AND:
    return "T_AND";
  case T_OR:
    return "T_OR";
  case T_END:
    return "T_END";
  case T_IF:
    return "T_IF";
  case T_THEN:
    return "T_THEN";
  case T_ELSE:
    return "T_ELSE";
  case T_WHILE:
    return "T_WHILE";
  case T_SHOW:
    return "T_SHOW";
  case T_NIL:
    return "T_NIL";
  case T_TRUE:
    return "T_TRUE";
  case T_FALSE:
    return "T_FALSE";
  case T_EOF:
    return "T_EOF";
  case T_ERR:
    return "T_ERR";
  };

  return "UNKNOWN_TOKEN";
}

char *token_to_string(Token *tk) {
  char *result = NULL;
  sprintf(result, "T[%s]", toktype_to_string(tk->type));
  return result;
}

Lexer lexer;

bool match_char(wchar_t c) {
  if (is_eof())
    return false;
  if (*lexer.current != c)
    return false;
  lexer.current++;
  return true;
}

wchar_t peek() { return *lexer.current; }

void boot_lexer(wchar_t *src) {
  lexer.start = src;
  lexer.current = src;
  lexer.line = 1;
}

bool is_eof() { return *lexer.current == '\0'; }

Token mktok(TokType type) {
  Token tok;
  tok.type = type;
  tok.start = lexer.start;
  tok.length = (int)(lexer.current - lexer.start);
  tok.line = lexer.line;
  return tok;
}

static Token err_tok(wchar_t *msg) {
  Token tk;
  tk.type = T_ERR;
  tk.start = msg;
  tk.length = (int)wcslen(msg);
  tk.length = lexer.line;
  return tk;
}

void skip_ws() {
  for (;;) {
    wchar_t c = peek();
    switch (c) {
    case ' ':
    case '\r':
    case '\t':
      next();
      break;
    case '\n':
      lexer.line++;
      next();
      break;
    default:
      return;
    }
  }
}

static wchar_t next() {
  lexer.current++;
  return lexer.current[-1];
}

Token get_tok() {
  skip_ws();

  lexer.start = lexer.current;
  if (is_eof())
    return mktok(T_EOF);

  wchar_t c = next();
  switch (c) {
  case '(':
    return mktok(T_LPAREN);
  case ')':
    return mktok(T_RPAREN);
  case '{':
    return mktok(T_LBRACE);
  case '}':
    return mktok(T_RBRACE);
  case ';':
    return mktok(T_SEMICOLON);
  case ',':
    return mktok(T_COMMA);
  case '>':
    return mktok(T_GT);
  case '<':
    return mktok(T_LT);
  default:
    if (is_bn_num(c)) {
      wprintf(L"is bn num\n");
    } else if (is_bn_char(c)) {
      wprintf(L"is bn char\n");
    }
  }

  return err_tok(L"Unknown character");
}
