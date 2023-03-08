#include <stdbool.h>
#include <wchar.h>
typedef enum {
  T_LPAREN,
  T_RPAREN,
  T_LBRACE,
  T_RBRACE,
  T_MINUS,
  T_PLUS,
  T_DIV,
  T_ASTR,
  T_SEMICOLON,
  T_COMMA,
  T_EQ,
  T_EQEQ,
  T_GT,
  T_GTE,
  T_LT,
  T_LTE,
  T_ID,
  T_STR,
  T_NUM,
  T_LET,
  T_AND,
  T_OR,
  T_END,
  T_IF,
  T_THEN,
  T_ELSE,
  T_WHILE,
  T_SHOW,
  T_NIL,
  T_TRUE,
  T_FALSE,

  T_ERR,
  T_EOF,
} TokType;

typedef struct {
  TokType type;
  wchar_t *start;
  int length;
  int line;
} Token;

const char *toktype_to_string(TokType tt);
char *token_to_string(Token *t);
void boot_lexer(wchar_t *src);

typedef struct {
  // start of lexeme;
  wchar_t *start;
  // current char being read;
  wchar_t *current;
  int line;
} Lexer;

// check if lexer at the end of input
// Checks if current char is '\0'
bool is_eof();

// Given a type craetes a token;
Token mktok(TokType type);
static Token err_tok(wchar_t *msg);
static wchar_t next();
Token get_tok();
/*
void init_lexer(const char * source);
bool is_at_eof();
Token make_tok(TokType type);
Token next_token();
void lex();
*/
