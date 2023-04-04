#ifndef cpank_compiler_h
#define cpank_compiler_h

#include <stdbool.h>
#include <stdint.h>
#include <uchar.h>
#include <wchar.h>

#include "common.h"
#include "instruction.h"
#include "lexer.h"
#include "obj.h"

typedef struct Parser {
    PankVm *vm;
    Lexer lexer;
    Token cur;
    Token prev;
    bool had_err;
    bool panic_mode;
} Parser;

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
    Parser *parser;
    ObjFunc *func;
    FuncType type;
    struct Compiler *enclosing;
    Local locals[UINT8_COUNT];
    int local_count;
    int scope_depth;
    Upval upvs[UINT8_COUNT];
} Compiler;

void init_compiler(Parser *parser, Compiler *compiler, Compiler *prevcomp,
                   FuncType type);

ObjFunc *compile(PankVm *vm, char32_t *source);
ObjFunc *end_compiler(Compiler *compiler);
void mark_compiler_roots(PankVm *vm, Compiler *compiler);
ObjFunc *compile_module(PankVm *vm, char32_t *source);
#endif
