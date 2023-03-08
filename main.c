#include "lexer.h"
#include <locale.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
wchar_t *y = L"নাম";
wchar_t *x = L"(hello) world";

/*
typedef struct {
  // start of source
  wchar_t *start;
  // current chars being read;
  wchar_t *current;
  int curpos;
  int line;

} Lex;

Lex lx;


void start_lex(wchar_t *s) {
  lx.start = s;
  lx.current = s;
  lx.curpos = 0;
  lx.line = 0;
  // return lx;
}

static Token mtok(TokType tt) {
  Token token;
  token.type = tt;
  token.start = lx.start;
  token.length = lx.current - lx.start;
  token.line = lx.line;
  return token;
}

wchar_t advance() {
  // lx.curpos++;
  lx.current++;
  return lx.current[-1];
}
*/

int main() {

  setlocale(LC_CTYPE, "");
  boot_lexer(y);
  while (!is_eof()) {
    Token tk = get_tok();
    if (tk.type != T_ERR) {
      wprintf(L"T[%s] '%.*ls' \n", toktype_to_string(tk.type), tk.length,
              tk.start);
    } else {
      wprintf(L"E[] '%ls'\n", tk.start);
    }
  }
  // printf("L->%d" , is_eof());
  // Token tk = get_token();
  // wprintf(L"TOKEN-> %ls" , tk.start);
  // Lx l = start_lex(y);
  //  printf("%s" , eof(&l) ? "true" : "false");
  // while (!eof(&l)) {
  //   Token tk = get_token(&l);
  //   printf("T -> %s\n" , tk.start);
  // printf("TOK-> %s %d\n", toktype_to_string(tk.type), tk.length);
  // printf("TOK->%s\n", token_to_string(&tk));
  //}
  // printf("-> %s\n" , advance(&l) );

  // printf("-> %s\n" , advance(&l) );

  // char * s = x;
  // size_t ret , off;
  // wprintf("%lc\n" , y);
  // printf("%lc" , y);
  // wprintf(L"LEN-> %ld\n" ,wcslen(y) );
  // wprintf(L"%lc\n" , y[0]);

  // wprintf(L"%lc\n" , y[1]);

  // wprintf(L"%lc\n" , y[2]);
}
