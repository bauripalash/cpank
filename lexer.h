#include <stdbool.h>
typedef enum {
	T_LPAREN, T_RPAREN,
	T_LBRACE, T_RBRACE,
	T_MINUS, T_PLUS, T_DIV, T_ASTR,
	T_SEMICOLON, T_COMMA,
	T_EQ, T_EQEQ,
	T_GT , T_GTE,
	T_LT , T_LTE,
	T_ID , T_STR, T_NUM,
	T_LET,
	T_AND , T_OR , T_END,
	T_IF , T_THEN , T_ELSE,
	T_WHILE,
	T_SHOW,
	T_NIL,
	T_TRUE, T_FALSE,


	T_ERR, T_EOF,
}TokType;



typedef struct {
	TokType type;
	const char *start;
	int length;
	int line;
} Token;


typedef struct{
	const char * start;
	const char * current;
	int line;
}Lexer ;

void init_lexer(const char * source);
bool is_at_eof();
Token make_tok(TokType type);
Token next_token();
void lex();
