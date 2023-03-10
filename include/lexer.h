#include <stdbool.h>
#include <wchar.h>
typedef enum {
  // ( -> Left Paren
  T_LPAREN,
  // )
  T_RPAREN,
  // {
  T_LBRACE,
  // }
  T_RBRACE,
  // -
  T_MINUS,
  // +
  T_PLUS,
  // /
  T_DIV,
  // *
  T_ASTR,
  // ;
  T_SEMICOLON,
  // ,
  T_COMMA,
  // =
  T_EQ,
  // ==
  T_EQEQ,
  // >
  T_GT,
  // >=
  T_GTE,
  // <
  T_LT,
  // <=
  T_LTE,
  // let --> name <-- = "Palash";
  T_ID,
  // "hello"
  T_STR,
  // 123 , 3.14
  // TO DO: Add different number tokens
  // for floats and integers
  T_NUM,
  // --> let <-- x = y
  // let / dhori
  T_LET,
  // and / ebong
  T_AND,
  // or / ba
  T_OR,
  // end / sesh
  T_END,
  // if / jodi
  T_IF,
  // then / tahole
  T_THEN,
  // else / nahole
  T_ELSE,
  // while / jotokkhon
  T_WHILE,
  // show / dekhao
  T_SHOW,
  // nil / shunno
  T_NIL,
  // true / sotto
  T_TRUE,
  // false / mittha
  T_FALSE,
  // return / ferao
  T_RETURN,
  // fun / work
  T_FUNC,
  // !
  T_BANG,

  // !=
  T_NOTEQ,

  // Error , unknown token
  T_ERR,
  // End of script
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
Token err_tok(wchar_t *msg);
wchar_t next();
Token get_tok();
/*
void init_lexer(const char * source);
bool is_at_eof();
Token make_tok(TokType type);
Token next_token();
void lex();
*/
