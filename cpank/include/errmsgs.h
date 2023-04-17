#ifndef cpank_errmsg_h
#define cpank_errmsg_h

#include <uchar.h>

#include "common.h"

// Lexer Error : Unknown Character
#define EMSG_UNKNOWN_CHAR 101
// Missing ']' After Array
#define EMSG_RBRAC_AFTER_ARR 102
// Missing ')' after arglist (call expression)
#define EMSG_RBRAC_AFTER_ARG 103
// No Variable name after 'let'
#define EMSG_NO_VAR_AFTER_LET 104
// No  '(' after function name
#define EMSG_LBRAC_AFTER_FUNC_NAME 105

char32_t* geterrmsg(int code);

#endif
