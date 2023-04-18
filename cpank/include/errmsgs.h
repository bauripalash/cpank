#ifndef cpank_errmsg_h
#define cpank_errmsg_h

#include <uchar.h>

#include "common.h"
#define EMSG_KW     90
#define EMSG_HERE   91
#define EMSG_AT_END 92
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
// More than 255 arguments in argument list (call)
#define EMSG_ARGLIST_TOO_MUCH_ARG 106

#define EMSG_RBRAC_AFTER_GROUP    107
#define EMSG_TOO_MANY_ARR_ITEM    108
#define EMSG_RBRAC_INDEX          109
#define EMSG_TOO_MANY_LOCALVARS   110

char32_t* geterrmsg(int code);

#endif
