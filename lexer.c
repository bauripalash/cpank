#include <stdbool.h>
#include <stdio.h>
#include "lexer.h"

const char* toktype_to_string(TokType t){
	switch (t) {
		case T_LPAREN: return "T_LPAREN";
		case T_RPAREN: return "T_RPAREN";
		case T_LBRACE: return "T_LBRACE";
		case T_RBRACE: return "T_RBRACE";
		case T_MINUS: return "T_MINUS";
		case T_PLUS: return "T_PLUS";
		case T_DIV: return "T_DIV";
		case T_ASTR: return "T_ASTR";
		case T_SEMICOLON: return "T_SEMICOLON";
		case T_COMMA: return "T_COMMA";
		case T_EQ: return "T_EQ";
		case T_EQEQ: return "T_EQEQ";
		case T_GT : return "T_GT";
		case T_GTE : return "T_GTE";
		case T_LT: return "T_LT";
		case T_LTE : return "T_LTE";
		case T_ID: return "T_ID";
		case T_STR : return "T_STR";
		case T_NUM: return "T_NUM";
		case T_LET : return "T_LET";
		case T_AND: return "T_AND"; 
		case T_OR: return "T_OR"; 
		case T_END: return "T_END";
		case T_IF: return "T_IF";
		case T_THEN: return "T_THEN";
		case T_ELSE: return "T_ELSE";
		case T_WHILE: return "T_WHILE";
		case T_SHOW : return "T_SHOW";
		case T_NIL: return "T_NIL";
		case T_TRUE : return "T_TRUE";
		case T_FALSE : return "T_FALSE";
		case T_EOF :  return "T_EOF";
		case T_ERR : return "T_ERR";
	};

	return "UNKNOWN_TOKEN";

}

Lexer lexer;

void init_lexer(const char * source){
	lexer.start = source;
	lexer.current = source;
	lexer.line = 1;
}

bool is_at_eof(){
	return *lexer.current == '\0';
}

Token make_tok(TokType type){
	Token tok;
	tok.type = type;
	tok.start = lexer.start;
	tok.length =(int)(lexer.current - lexer.start);
	tok.line = lexer.line;
	return tok;
}
static char adv(){
	lexer.current++;
	return lexer.current[-1];
}
Token next_token(){
	lexer.start = lexer.current;
	if (is_at_eof()) return make_tok(T_EOF) ;
	
	char c = adv();
	switch (c) {
		case '(' :	
			return make_tok(T_LPAREN);
		case ')' : 
			return make_tok(T_RPAREN);
		case '{' :
			return make_tok(T_LBRACE);
		case '}' :
			return make_tok(T_RBRACE);
		case ';' : 
			return make_tok(T_SEMICOLON);
		case ',' :
			return make_tok(T_COMMA);
		case '>' :
			return make_tok(T_GT);
		case '<' : 
			return make_tok(T_LT);

	}

	return make_tok(T_ERR);
}



void lex(){
	while (!is_at_eof()){
		Token tok = next_token();
		printf("TOK-> %.*s -> %s\n" , tok.length ,tok.start , toktype_to_string(tok.type));
		//printf("a");
		//printf("%.*s" , tok.length , tok.start);
		//adv();
	}
}


