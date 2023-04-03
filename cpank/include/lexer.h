#ifndef cpank_lexer_h
#define cpank_lexer_h
#include <stdbool.h>
#include <uchar.h>
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
    T_LSBRACKET,
    T_RSBRACKET,
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
    // . (dot) (full stop) (period)
    T_DOT,
    // !=
    T_NOTEQ,
    // !
    T_BANG,
    // :
    T_COLON,
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
    // let / dhori (see `token.h`)
    T_LET,
    // and / ebong (see `token.h`)
    T_AND,
    // or / ba (see `token.h`)
    T_OR,
    // end / sesh (see `token.h`)
    T_END,
    // if / jodi (see `token.h`)
    T_IF,
    // then / tahole (see `token.h`)
    T_THEN,
    // else / nahole (see `token.h`)
    T_ELSE,
    // while / jotokkhon (see `token.h`)
    T_WHILE,
    // show / dekhao (see `token.h`)
    T_SHOW,
    // nil (see `token.h`)
    T_NIL,
    // true / sotto (see `token.h`)
    T_TRUE,
    // false / mittha (see `token.h`)
    T_FALSE,
    // return / ferao (see `token.h`)
    T_RETURN,
    // fun / work (see `token.h`)
    T_FUNC,

    // panic (see `token.h`)
    T_MKERR,

    // import (see `token.h`)
    T_IMPORT,

    // Error , unknown token
    T_ERR,
    // End of script
    T_EOF,
} TokType;

typedef struct {
    TokType type;
    char16_t *start;
    int length;
    int line;
} Token;

typedef struct {
    // start of lexeme;
    char16_t *start;
    // current char being read;
    char16_t *current;
    int line;
} Lexer;

// get token type as string
const char *toktype_to_string(TokType tt);

// Convert token to string
// you must free the result
char *token_to_string(const Token *t);

// Initialize the lexer
void boot_lexer(Lexer *lexer, char16_t *src);

// check if lexer at the end of input
// Checks if current char is '\0'
bool is_eof(Lexer *lexer);

// Get the next token
Token get_tok(Lexer *lexer);
#endif
