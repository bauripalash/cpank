#ifndef cpank_compiler_h
#define cpank_compiler_h

#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#include "common.h"
#include "instruction.h"
#include "lexer.h"
#include "obj.h"

typedef struct Parser {
    Token cur;
    Token prev;
    bool had_err;
    bool panic_mode;
} Parser;

typedef enum Prec {
    PREC_NONE,
    PREC_ASSIGN,
    PREC_OR,
    PREC_AND,
    PREC_EQ,
    PREC_COMP,
    PREC_TERM,
    PREC_FACT,
    PREC_UNARY,
    PREC_CALL,
    PREC_DEFAULT,
} Prec;

typedef struct Local {
    Token name;
    int depth;
    bool is_captd;
} Local;

typedef struct Upval {
    uint8_t index;
    bool is_local;
} Upval;

typedef enum FuncType {
    FTYPE_FUNC,
    FTYPE_SCRIPT,
} FuncType;

typedef struct Compiler {
    ObjFunc *func;
    FuncType type;
    struct Compiler *enclosing;
    Local locals[UINT8_COUNT];
    int local_count;
    int scope_depth;
    Upval upvs[UINT8_COUNT];
} Compiler;

void init_comiler(Compiler *compiler, FuncType type);

ObjFunc *compile(wchar_t *source);
ObjFunc *end_compiler();
void mark_compiler_roots();
ObjFunc *compile_module(wchar_t *source);
#endif
