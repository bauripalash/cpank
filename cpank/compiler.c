/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#include "include/compiler.h"

#include <string.h>
#include <uchar.h>

#include "include/common.h"
#include "include/errmsgs.h"
#include "include/helper/comp.h"
#include "include/instruction.h"
#include "include/lexer.h"
#include "include/mem.h"
#include "include/obj.h"
#include "include/utils.h"
#include "include/value.h"
#include "include/vm.h"

// #define DEBUG_PRINT_CODE
// #define DEBUG_LEXER
#ifdef DEBUG_PRINT_CODE
 #include "include/debug.h"
#endif

// #define DEBUG_LEXER

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

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

static void start_scope(Compiler *compiler);
static void end_scope(Compiler *compiler);
static void emit_bt(Compiler *compiler, uint8_t bt);
static void emit_two(Compiler *compiler, uint8_t bt_1, uint8_t bt_2);
static void emit_return(Compiler *compiler);
static void parse_prec(Compiler *compiler, Prec prec);
static void read_stmt(Compiler *compiler);
static void read_declr(Compiler *compiler);

typedef void (*ParseFn)(Compiler *compiler, bool can_assign);

typedef struct ParseRule {
    ParseFn prefix;
    ParseFn infix;
    Prec prec;
} ParseRule;

static ParseRule *get_parse_rule(TokType tt);

// Parser parser;
// Compiler *current = NULL;

// initialize a compiler instance
// set `enclosing` to previous compiler
// (*current declared above) which get set to it
//
// func to NULL
// FuncType for compiler
// FTYPE_FUNC -> for Functions
// FTYPE_SCRIPT -> for main script
// local_count to zero
void init_compiler(Parser *parser, Compiler *compiler, Compiler *prevcomp,
                   FuncType type) {
    compiler->parser = parser;
    compiler->enclosing = prevcomp;
    compiler->func = NULL;
    compiler->type = type;
    compiler->local_count = 0;
    compiler->scope_depth = 0;
    if (parser != NULL) {
        compiler->func = new_func(parser->vm);

        parser->vm->compiler = compiler;
        if (type != FTYPE_SCRIPT) {
            compiler->func->name =
                copy_string(parser->vm, compiler->parser->prev.start,
                            compiler->parser->prev.length);
        }
    }
    // compiler->func = new_func(vm);

    Local *local = &compiler->locals[compiler->local_count++];
    local->depth = compiler->scope_depth;
    // local->is_captd = false;
    local->name.start = U"";
    local->name.length = 0;
}

// get instruction set from current compiler
static Instruction *cur_ins(Compiler *compiler) { return &compiler->func->ins; }

// helper function for err(..) and err_at_cur(..)
static void err_at(Parser *parser, Token *tok, char32_t *msg, bool atcur) {
    if (parser->panic_mode) {
        return;
    }

    char *msg_str = c_to_c(msg, 0);

    parser->panic_mode = true;
    int lineindex = 1;
    if (atcur) {
        lineindex = parser->prev.line;
    } else {
        lineindex = parser->cur.line;
    }
    char32_t *line = get_line(&parser->lexer, lineindex);
    if (line != NULL) {
        if (parser->vm->need_buffer) {
            write_pbuffer(&parser->vm->buffer, "%d | %ls\n", lineindex, line);
        } else {
#if defined(PANK_OS_WIN)
            char *line_c = c_to_c(line, strlen32(line));
            cp_err_println(L"%d | %S\n", lineindex, line_c);
            free(line_c);
#else
            cp_print(L"%d | %ls\n", lineindex, line);
#endif
        }
        // fwprintf(stderr, L"%d | %ls\n", lineindex, line);

        free(line);
    }
    if (parser->vm->need_buffer) {
        write_pbuffer(&parser->vm->buffer, "[L %d] %ls ", tok->line,
                      geterrmsg(EMSG_KW));
    } else {
#if defined(PANK_OS_WIN)
        char32_t *xmsg = geterrmsg(EMSG_KW);
        char *emsg_c = c_to_c(xmsg, strlen32(xmsg));
        cp_err_println(L"[l %d] %S ", tok->line, emsg_c);
        free(emsg_c);
#else
        cp_print(L"[l %d] %ls ", tok->line, geterrmsg(EMSG_KW));
#endif
    }

    if (tok->type == T_EOF) {
        if (parser->vm->need_buffer) {
            write_pbuffer(&parser->vm->buffer, "%ls", geterrmsg(EMSG_AT_END));
        } else {
            cp_print(L"%ls", geterrmsg(EMSG_AT_END));
        }
        if (atcur) {
            if (parser->vm->need_buffer) {
                write_pbuffer(&parser->vm->buffer, "->%.*ls<-",
                              parser->prev.length, parser->prev.start);
            } else {
                cp_print(L"->%.*ls<-", parser->prev.length, parser->prev.start);
            }
        }
    } else if (tok->type == T_ERR) {
    } else if (tok->type == T_UNKNOWN) {
        if (parser->vm->need_buffer) {
            write_pbuffer(&parser->vm->buffer, "-> %.*ls <- : %ls\n",
                          tok->length, tok->start,
                          geterrmsg(EMSG_UNKNOWN_CHAR));
        } else {
            cp_println(L"-> %.*ls <- : %ls", tok->length, tok->start,
                       geterrmsg(EMSG_UNKNOWN_CHAR));
        }
    } else {
        char *t_str = c32_to_char(
            tok->start, tok->length);  // c_to_c(tok->start, tok->length);
        if (parser->vm->need_buffer) {
            write_pbuffer(&parser->vm->buffer, "%s -> %s <- ",
                          geterrmsg(EMSG_HERE), t_str);
        } else {
#if defined(PANK_OS_WIN)
            fwprintf(stdout, L" at %.*S", tok->length, t_str);
#else
            cp_println(L" %ls -> %s <- ", geterrmsg(EMSG_HERE), t_str);
#endif
        }
        free(t_str);
    }
    if (tok->type != T_UNKNOWN) {
        if (parser->vm->need_buffer) {
            write_pbuffer(&parser->vm->buffer, " : %s\n\n", msg_str);
        } else {
#if defined(PANK_OS_WIN)
            cp_println(L" : %S\n", msg_str);
#else
            cp_println(L" : %s\n", msg_str);
#endif
        }
    }
    free(msg_str);
    parser->had_err = true;
}

// Throw error for previous token
static void err(Parser *parser, char32_t *msg) {
    err_at(parser, &parser->prev, msg, false);
}

// Throw error for current function
static void err_at_cur(Parser *parser, char32_t *msg) {
    err_at(parser, &parser->cur, msg, true);
}

static void advance(Parser *parser) {
    parser->prev = parser->cur;
    for (;;) {
        parser->cur = get_tok(&parser->lexer);
        if (parser->cur.type == T_ERR || parser->cur.type == T_UNKNOWN) {
            err_at_cur(parser, parser->cur.start);
        } else {
            break;
        }
    }
}

// emit bytecode
static void emit_bt(Compiler *compiler, uint8_t bt) {
    write_ins(compiler->parser->vm, cur_ins(compiler), bt,
              compiler->parser->prev.line);
}

// emit two bytecode or anything which is
// uint8_t
static void emit_two(Compiler *compiler, uint8_t bt_1, uint8_t bt_2) {
    emit_bt(compiler, bt_1);
    emit_bt(compiler, bt_2);
}

// emit 'nil' opcode and
// then 'return' opcode
static void emit_return(Compiler *compiler) {
    emit_bt(compiler, OP_NIL);
    emit_bt(compiler, OP_RETURN);
}

// add a constant to current instruction
// and return its index at constant array
// of the instruction set
static uint8_t make_const(Compiler *compiler, Value val) {
    int con = add_const(compiler->parser->vm, cur_ins(compiler), val);
    if (con > UINT8_MAX) {
        err(compiler->parser, U"Too many constants");
        return 0;
    }

    return (uint8_t)con;
}

// make a constant (uint8_t make_const(Value))
// and emit const opcode
static void emit_const(Compiler *compiler, Value value) {
    emit_two(compiler, OP_CONST, make_const(compiler, value));
}

// parse numbers
// double
static void read_number(Compiler *compiler, bool can_assign) {
    char *str = c32_to_char(compiler->parser->prev.start,
                            compiler->parser->prev.length);
    // double val = 100 ;//wcstod(compiler->parser->prev.start, NULL);
    double val = strtod(str, NULL);

    //    cp_println(L"STR -> %s" , str);
    //    cp_println(L"NUM -> %.8f" , val);

    free(str);
    emit_const(compiler, make_num(val));
}

static inline int ch32_parse_hex(const char32_t *str, int len, int curindex) {
    int cp = 0;
    for (int x = 0; x < len; x++) {
        char32_t ux = str[curindex + x + 2];
        if (ux >= '0' && ux <= '9') {
            cp = (cp << 4) | (ux - '0');
        } else if (ux >= 'a' && ux <= 'f') {
            cp = (cp << 4) | (ux - 'a' + 10);
        } else if (ux >= 'A' && ux <= 'F') {
            cp = (cp << 4) | (ux - 'A' + 10);
        }
    }
    return cp;
}

char32_t *unescape_unicode(char32_t *str, int len) {
    char32_t result[len + 1];
    int j = 0;

    for (int i = 0; i < len; i++) {
        // cp_println(L"i->%d|" , i);
        if (str[i] == '\\') {
            switch (str[i + 1]) {
                case 'n':
                    result[j++] = '\n';
                    i++;
                    break;
                case '?':
                    result[j++] = '\?';
                    i++;
                    break;
                case 'a':
                    result[j++] = '\a';
                    i++;

                    break;
                case 'b':
                    result[j++] = '\b';
                    i++;
                    break;
                case 'f':
                    result[j++] = '\b';
                    i++;
                    break;
                case 'r':
                    result[j++] = '\r';
                    i++;
                    break;
                case 't':
                    result[j++] = '\t';
                    i++;
                    break;
                case 'v':
                    result[j++] = '\v';
                    i++;
                    break;
                case '\\':
                    result[j++] = '\\';
                    i++;
                    break;
                case '\'':
                case '"':
                    break;
                case 'u': {
                    result[j++] = (char32_t)ch32_parse_hex(str, 4, i);
                    // strtol(unic , NULL , 16);
                    i += 5;
                    break;
                }
                case 'U': {
                    result[j++] = (char32_t)ch32_parse_hex(str, 8, i);
                    i += 9;
                    break;
                }
            }
        } else {
            result[j++] = str[i];
            // i++;
        }
    }

    result[j] = '\0';
    char32_t *s = (char32_t *)malloc((j + 1) * sizeof(char32_t));
    copy_c32(s, result, j + 1);
    return s;
}

// read strings
// "...."
static void read_string(Compiler *compiler, bool can_assign) {
    int raw_len = compiler->parser->prev.length - 2;

    char32_t *newstr =
        unescape_unicode((compiler->parser->prev.start + 1), raw_len);
    if (newstr == NULL) {
        err(compiler->parser,
            U"Failed to allocate memory for unescaping string");
        return;
    }
    ObjString *raws =
        take_string(compiler->parser->vm, newstr, strlen32(newstr));
    raws->obj.tok_len = raw_len;
    raws->obj.is_virt = true;
    raws->obj.tok_colpos = compiler->parser->prev.colpos;
    Value v = make_obj_val(raws);
    emit_const(compiler, v);
}

void emit_loop(Compiler *compiler, int ls) {
    emit_bt(compiler, OP_LOOP);
    int offset = cur_ins(compiler)->len - ls + 2;
    if (offset > UINT16_MAX) {
        err(compiler->parser, U"loop body too big");
    }
    emit_bt(compiler, (offset >> 8) & 0xFF);
    emit_bt(compiler, offset & 0xFF);
}

int emit_jump(Compiler *compiler, uint8_t ins) {
    emit_bt(compiler, ins);
    emit_two(compiler, 0xFF, 0xFF);
    return cur_ins(compiler)->len - 2;
}

void patch_jump(Compiler *compiler, int offset) {
    int jmp = cur_ins(compiler)->len - offset - 2;
    if (jmp > UINT16_MAX) {
        err(compiler->parser, U"too big jump");
    }

    cur_ins(compiler)->code[offset] = (jmp >> 8) & 0xFF;
    cur_ins(compiler)->code[offset + 1] = jmp & 0xFF;
}

void read_and(Compiler *compiler, bool can_assign) {
    int ej = emit_jump(compiler, OP_JMP_IF_FALSE);
    emit_bt(compiler, OP_POP);
    parse_prec(compiler, PREC_AND);
    patch_jump(compiler, ej);
}

void read_or(Compiler *compiler, bool can_assign) {
    int else_jmp = emit_jump(compiler, OP_JMP_IF_FALSE);
    int endjmp = emit_jump(compiler, OP_JMP);

    patch_jump(compiler, else_jmp);
    emit_bt(compiler, OP_POP);
    parse_prec(compiler, PREC_OR);
    patch_jump(compiler, endjmp);
}

// eat current token;
// Matches current token
// if true advance else throw error
static void eat_tok(Compiler *compiler, TokType tt, char32_t *errmsg) {
    if (compiler->parser->cur.type == tt) {
        advance(compiler->parser);
        return;
    }

    err_at_cur(compiler->parser, errmsg);
}

// Checks 'current' token;
static bool check_tok(Compiler *compiler, TokType tt) {
    return compiler->parser->cur.type == tt;
}

// Matches 'current' token
static bool match_tok(Compiler *compiler, TokType tt) {
    if (!check_tok(compiler, tt)) {
        return false;
    } else {
        advance(compiler->parser);
        return true;
    }
}

// read expression
// alias for
// parse_prec(PREC_ASSIGN)
static void read_expr(Compiler *compiler) { parse_prec(compiler, PREC_ASSIGN); }

uint8_t read_arg_list(Compiler *compiler) {
    uint8_t argc = 0;
    if (!check_tok(compiler, T_RPAREN)) {
        do {
            read_expr(compiler);
            if (argc == 255) {
                err(compiler->parser, geterrmsg(EMSG_ARGLIST_TOO_MUCH_ARG));
            }
            argc++;
        } while (match_tok(compiler, T_COMMA));
    }

    eat_tok(compiler, T_RPAREN, geterrmsg(EMSG_RBRAC_AFTER_ARG));
    return argc;
    ;
}

void read_call(Compiler *compiler, bool can_assign) {
    uint8_t arg_count = read_arg_list(compiler);
    emit_two(compiler, OP_CALL, arg_count);
}

void read_group(Compiler *compiler, bool can_assign) {
    read_expr(compiler);
    eat_tok(compiler, T_RPAREN, geterrmsg(EMSG_RBRAC_AFTER_GROUP));
}

void read_unary(Compiler *compiler, bool can_assign) {
    TokType op = compiler->parser->prev.type;

    // read_expr();
    parse_prec(compiler, PREC_UNARY);
    switch (op) {
        case T_MINUS:
            emit_bt(compiler, OP_NEG);
            break;
        case T_BANG:
            emit_bt(compiler, OP_NOT);
            break;
        default:
            return;
    }
}

// read binary operations
// add , sub , multiply , divide
// not eq (!=) , eqaul (==)
void read_binary(Compiler *compiler, bool can_assign) {
    TokType op = compiler->parser->prev.type;
    ParseRule *prule = get_parse_rule(op);
    parse_prec(compiler, (Prec)(prule->prec + 1));
    switch (op) {
        case T_PLUS:
            emit_bt(compiler, OP_ADD);
            break;
        case T_MINUS:
            emit_bt(compiler, OP_SUB);
            break;
        case T_ASTR:
            emit_bt(compiler, OP_MUL);
            break;
        case T_DIV:
            emit_bt(compiler, OP_DIV);
            break;
        case T_NOTEQ:
            emit_two(compiler, OP_EQ, OP_NOT);
            break;
        case T_EQEQ:
            emit_bt(compiler, OP_EQ);
            break;
        case T_GT:
            emit_bt(compiler, OP_GT);
            break;
        case T_GTE:
            emit_bt(compiler, OP_GTE);
            break;
        case T_LT:
            emit_bt(compiler, OP_LT);
            break;
        case T_LTE:
            emit_bt(compiler, OP_LTE);
            break;
        default:
            return;
    }
}

// read and emit
// TRUE / FALSE / NIL
void literal(Compiler *compiler, bool can_assign) {
    switch (compiler->parser->prev.type) {
        case T_FALSE:
            emit_bt(compiler, OP_FALSE);
            break;
        case T_NIL:
            emit_bt(compiler, OP_NIL);
            break;
        case T_TRUE:
            emit_bt(compiler, OP_TRUE);
            break;
        default:
            return;
    }
}

void read_array(Compiler *compiler, bool can_assign) {
    int items = 0;
    if (!check_tok(compiler, T_RSBRACKET)) {
        do {
            read_expr(compiler);
            if (items == 255) {
                err(compiler->parser, geterrmsg(EMSG_TOO_MANY_ARR_ITEM));
            }
            items++;
        } while (match_tok(compiler, T_COMMA));
    }

    eat_tok(compiler, T_RSBRACKET, geterrmsg(EMSG_RBRAC_AFTER_ARR));
    emit_two(compiler, OP_ARRAY, (uint8_t)items);
}

void read_index_expr(Compiler *compiler, bool can_assign) {
    read_expr(compiler);
    eat_tok(compiler, T_RSBRACKET, geterrmsg(EMSG_RBRAC_INDEX));
    if (can_assign && match_tok(compiler, T_EQ)) {
        read_expr(compiler);
        emit_bt(compiler, OP_SUBSCRIPT_ASSIGN);
    } else {
        emit_bt(compiler, OP_ARR_INDEX);
    }

    // cp_println(L"can assign subscript -> %s", can_assign ? "true" : "false"
    // );
}

void add_local(Compiler *compiler, Token name) {
    if (compiler->local_count == UINT8_COUNT) {
        err(compiler->parser, geterrmsg(EMSG_TOO_MANY_LOCALVARS));
        return;
    }
    Local *local = &compiler->locals[compiler->local_count++];
    local->name = name;
    local->depth = compiler->scope_depth;
    local->is_captd = false;
    // local->depth = current->scope_depth;
}

bool id_eq(Token *l, Token *r) {
    if (l->length != r->length) {
        return false;
    }
    return str32cmp_gen_n(l->start, r->start, l->length);
}

int resolve_local(Compiler *compiler, Token *token) {
    for (int i = compiler->local_count - 1; i >= 0; i--) {
        Local *local = &compiler->locals[i];
        if (id_eq(token, &local->name)) {
            if (local->depth == -1) {
                err(compiler->parser,
                    U"Can't read local var in its own initializer.");
            }
            return i;
        }
    }
    return -1;
}

int add_upval(Compiler *compiler, uint8_t index, bool is_local) {
    int upc = compiler->func->up_count;

    for (int i = 0; i < upc; i++) {
        Upval *upv = &compiler->upvs[i];
        if (upv->index == index && upv->is_local == is_local) {
            return i;
        }
    }

    if (upc == UINT8_COUNT) {
        err(compiler->parser, geterrmsg(EMSG_TOO_MANY_CLOSUREVARS));
        return 0;
    }

    compiler->upvs[upc].is_local = is_local;
    compiler->upvs[upc].index = index;
    return compiler->func->up_count++;
}

int resolve_upval(Compiler *compiler, Token *name) {
    if (compiler->enclosing == NULL) {
        return -1;
    }

    int local = resolve_local(compiler->enclosing, name);
    if (local != -1) {
        compiler->enclosing->locals[local].is_captd = true;
        return add_upval(compiler, (uint8_t)local, true);
    }

    int upv = resolve_upval(compiler->enclosing, name);
    if (upv != -1) {
        return add_upval(compiler, (uint8_t)upv, false);
    }

    return -1;
}

uint8_t make_id_const(Compiler *compiler, Token *name) {
    ObjString *s = copy_string(compiler->parser->vm, name->start, name->length);
    s->obj.tok_len = name->length;
    s->obj.tok_colpos = name->colpos;
    s->obj.is_virt = false;
    return make_const(compiler, make_obj_val(s));
}

void named_var(Compiler *compiler, Token name, bool can_assign) {
    // uint8_t arg = make_id_const(&name);
    uint8_t get_op, set_op;
    int arg = resolve_local(compiler, &name);
    if (arg != -1) {
        get_op = OP_GET_LOCAL;
        set_op = OP_SET_LOCAL;
    } else if ((arg = resolve_upval(compiler, &name)) != -1) {
        get_op = OP_GET_UP;
        set_op = OP_SET_UP;
    } else {
        arg = make_id_const(compiler, &name);
        get_op = OP_GET_GLOB;
        set_op = OP_SET_GLOB;
    }

    // wprintf(L"NAMED_VAR -> %s\n" , can_assign ? "true" : false);
    if (can_assign && match_tok(compiler, T_EQ)) {
        read_expr(compiler);
        emit_two(compiler, set_op, (uint8_t)arg);
    } else {
        emit_two(compiler, get_op, (uint8_t)arg);
    }
}

void read_var(Compiler *compiler, bool can_assign) {
    named_var(compiler, compiler->parser->prev, can_assign);
}

void read_dot(Compiler *compiler, bool can_assign) {
    eat_tok(compiler, T_ID, geterrmsg(EMSG_FUNCFIELD_AFTERDOT));
    uint8_t name = make_id_const(compiler, &compiler->parser->prev);

    if (can_assign && match_tok(compiler, T_EQ)) {
        read_expr(compiler);
        emit_two(compiler, OP_SET_MOD_PROP, name);
    } else {
        emit_two(compiler, OP_GET_MOD_PROP, name);
    }
}

void read_hmap(Compiler *compiler, bool can_assign) {
    int count = 0;
    do {
        if (check_tok(compiler, T_RBRACE)) {
            break;
        }
        read_expr(compiler);
        eat_tok(compiler, T_COLON, geterrmsg(EMSG_COLON_AFTER_HASHKEY));
        read_expr(compiler);
        count++;
    } while (match_tok(compiler, T_COMMA));

    eat_tok(compiler, T_RBRACE, U"expected '}' after hashmap");
    emit_two(compiler, OP_HMAP, (uint8_t)count);
}

ParseRule parse_rules[] = {
    [T_LPAREN] = {read_group, read_call, PREC_CALL},
    [T_RPAREN] = {NULL, NULL, PREC_NONE},
    [T_LBRACE] = {read_hmap, NULL, PREC_NONE},
    [T_RBRACE] = {NULL, NULL, PREC_NONE},
    [T_COMMA] = {NULL, NULL, PREC_NONE},
    [T_DOT] = {NULL, read_dot, PREC_CALL},
    [T_MINUS] = {read_unary, read_binary, PREC_TERM},
    [T_PLUS] = {NULL, read_binary, PREC_TERM},
    [T_SEMICOLON] = {NULL, NULL, PREC_NONE},
    [T_DIV] = {NULL, read_binary, PREC_FACT},
    [T_ASTR] = {NULL, read_binary, PREC_FACT},
    [T_BANG] = {read_unary, NULL, PREC_NONE},
    [T_NOTEQ] = {NULL, read_binary, PREC_EQ},
    [T_EQ] = {NULL, NULL, PREC_NONE},
    [T_EQEQ] = {NULL, read_binary, PREC_EQ},
    [T_GT] = {NULL, read_binary, PREC_COMP},
    [T_GTE] = {NULL, read_binary, PREC_COMP},
    [T_LT] = {NULL, read_binary, PREC_COMP},
    [T_LTE] = {NULL, read_binary, PREC_COMP},
    [T_ID] = {read_var, NULL, PREC_NONE},
    [T_STR] = {read_string, NULL, PREC_NONE},
    [T_NUM] = {read_number, NULL, PREC_NONE},
    [T_AND] = {NULL, read_and, PREC_AND},
    [T_ELSE] = {NULL, NULL, PREC_NONE},
    [T_FALSE] = {literal, NULL, PREC_NONE},
    [T_FUNC] = {NULL, NULL, PREC_NONE},
    [T_IF] = {NULL, NULL, PREC_NONE},
    [T_NIL] = {literal, NULL, PREC_NONE},
    [T_OR] = {NULL, read_or, PREC_OR},
    [T_SHOW] = {NULL, NULL, PREC_NONE},
    [T_RETURN] = {NULL, NULL, PREC_NONE},
    [T_TRUE] = {literal, NULL, PREC_NONE},
    [T_LET] = {NULL, NULL, PREC_NONE},
    [T_WHILE] = {NULL, NULL, PREC_NONE},
    [T_MKERR] = {NULL, NULL, PREC_NONE},
    [T_ERR] = {NULL, NULL, PREC_NONE},
    [T_EOF] = {NULL, NULL, PREC_NONE},
    [T_LSBRACKET] = {read_array, read_index_expr, PREC_CALL},
    [T_RSBRACKET] = {NULL, NULL, PREC_NONE},

};

static ParseRule *get_parse_rule(TokType tt) { return &parse_rules[tt]; }

static void start_scope(Compiler *compiler) { compiler->scope_depth++; }

static void end_scope(Compiler *compiler) {
    compiler->scope_depth--;
    while (compiler->local_count > 0 &&
           compiler->locals[compiler->local_count - 1].depth >
               compiler->scope_depth) {
        if (compiler->locals[compiler->local_count - 1].is_captd) {
            emit_bt(compiler, OP_CLS_UP);
        } else {
            emit_bt(compiler, OP_POP);
        }
        compiler->local_count--;
    }
}

static void sync_errors(Compiler *compiler) {
    compiler->parser->panic_mode = false;
    while (compiler->parser->cur.type != T_EOF) {
        if (compiler->parser->prev.type == T_SEMICOLON) {
            return;
        }

        switch (compiler->parser->cur.type) {
            case T_FUNC:
            case T_LET:
            case T_IF:
            case T_WHILE:
            case T_SHOW:
            case T_RETURN:
            case T_ERR:
                return;
            default:;
        }
        advance(compiler->parser);
    }
}

static void parse_prec(Compiler *compiler, Prec prec) {
    advance(compiler->parser);
    ParseFn pref_rule = get_parse_rule(compiler->parser->prev.type)->prefix;
    if (pref_rule == NULL) {
        err(compiler->parser, U"Expected expression");
        return;
    }
    bool can_assign = prec <= PREC_ASSIGN;
    pref_rule(compiler, can_assign);

    while (prec <= get_parse_rule(compiler->parser->cur.type)->prec) {
        advance(compiler->parser);
        ParseFn infix_rule = get_parse_rule(compiler->parser->prev.type)->infix;
        infix_rule(compiler, can_assign);
    }

    if (can_assign && match_tok(compiler, T_EQ)) {
        err(compiler->parser, U"Invalid asssignment");
    }
}

// parse and compile
// return statements
static void return_stmt(Compiler *compiler) {
    // top level script must not have
    // return statement
    if (compiler->type == FTYPE_SCRIPT) {
        err(compiler->parser, U"cannot return from top-level code/script");
    }

    // if there is a semicolon just after
    // 'return'; emit nil and return
    if (match_tok(compiler, T_SEMICOLON)) {
        emit_return(compiler);
    } else {
        // if expression after return is present
        // parse the expression and emit return
        read_expr(compiler);
        if (check_tok(compiler, T_SEMICOLON)) {
            eat_tok(compiler, T_SEMICOLON, U"expected ';' after return value");
        }
        emit_bt(compiler, OP_RETURN);
    }
}

// read expression statement
// bare expressions without any
// statements or keywords
// 1 + 2;
static void read_expr_stmt(Compiler *compiler) {
    read_expr(compiler);

    if (check_tok(compiler, T_SEMICOLON)) {
        eat_tok(compiler, T_SEMICOLON,
                U"Expected ';' after expression statement");
    }
    emit_bt(compiler, OP_POP);
}

static void read_print_stmt(Compiler *compiler) {
    read_expr(compiler);

    if (check_tok(compiler, T_SEMICOLON)) {
        eat_tok(compiler, T_SEMICOLON, U"expected ';' after show stmt");
    }

    emit_bt(compiler, OP_SHOW);
}

static void declare_var(Compiler *compiler) {
    if (compiler->scope_depth == 0) {
        return;
    }

    Token *name = &compiler->parser->prev;
    for (int i = compiler->local_count - 1; i >= 0; i--) {
        Local *local = &compiler->locals[i];
        if (local->depth != -1 && local->depth < compiler->scope_depth) {
            break;
        }

        if (id_eq(name, &local->name)) {
            err(compiler->parser,
                U"already a variable with this name in the current scope");
        }
    }
    add_local(compiler, *name);
}

static uint8_t parse_var(Compiler *compiler, char32_t *errmsg) {
    eat_tok(compiler, T_ID, errmsg);
    declare_var(compiler);
    if (compiler->scope_depth > 0) {
        return 0;
    }
    return make_id_const(compiler, &compiler->parser->prev);
}

static void mark_init(Compiler *compiler) {
    if (compiler->scope_depth == 0) {
        return;
    }
    compiler->locals[compiler->local_count - 1].depth = compiler->scope_depth;
}

static void define_var(Compiler *compiler, uint8_t global) {
    if (compiler->scope_depth > 0) {
        mark_init(compiler);
        return;
    }
    emit_two(compiler, OP_DEF_GLOB, global);
}

static void let_declr(Compiler *compiler) {
    uint8_t global = parse_var(compiler, geterrmsg(EMSG_NO_VAR_AFTER_LET));
    if (match_tok(compiler, T_EQ)) {
        read_expr(compiler);
    } else {
        emit_bt(compiler, OP_NIL);
    }

    if (check_tok(compiler, T_SEMICOLON)) {
        eat_tok(compiler, T_SEMICOLON, U"expected ';' after variable declr");
    }
    define_var(compiler, global);
}

static void read_to_end(Compiler *compiler) {
    while (!check_tok(compiler, T_END) && !check_tok(compiler, T_EOF)) {
        read_declr(compiler);
    }

    eat_tok(compiler, T_END, U"Expected 'end' after stmts");
}

static void build_func(Compiler *compiler, Compiler *funcCompiler,
                       FuncType type) {
    init_compiler(compiler->parser, funcCompiler, compiler, type);
    start_scope(funcCompiler);
    eat_tok(funcCompiler, T_LPAREN, geterrmsg(EMSG_LBRAC_AFTER_FUNC_NAME));

    if (!check_tok(funcCompiler, T_RPAREN)) {
        do {
            funcCompiler->func->arity++;
            if (funcCompiler->func->arity > 256) {
                err_at_cur(funcCompiler->parser, U"too many function params");
            }
            uint8_t con = parse_var(funcCompiler, U"Expected param name");
            define_var(funcCompiler, con);
        } while (match_tok(funcCompiler, T_COMMA));
    }

    eat_tok(funcCompiler, T_RPAREN, U"expected ')' after func params");
    read_to_end(funcCompiler);
    ObjFunc *fn = end_compiler(funcCompiler);
    emit_two(compiler, OP_CLOSURE, make_const(compiler, make_obj_val(fn)));

    for (int i = 0; i < fn->up_count; i++) {
        emit_bt(funcCompiler->enclosing,
                funcCompiler->upvs[i].is_local ? 1 : 0);
        emit_bt(funcCompiler->enclosing, funcCompiler->upvs[i].index);
    }
}

static void funct_declr(Compiler *compiler) {
    Compiler funcCompiler;
    uint8_t global = parse_var(compiler, U"Expected function name");
    // mk_func(compiler, FTYPE_FUNC);
    mark_init(compiler);
    build_func(compiler, &funcCompiler, FTYPE_FUNC);
    define_var(compiler, global);
}

static void read_declr(Compiler *compiler) {
    if (match_tok(compiler, T_LET)) {
        let_declr(compiler);
    } else if (match_tok(compiler, T_FUNC)) {
        funct_declr(compiler);
    } else {
        read_stmt(compiler);
    }
    if (compiler->parser->panic_mode) {
        sync_errors(compiler);
    }
}

/*got end?*/
static void read_if_block(Compiler *compiler) {
    start_scope(compiler);
    while (!check_tok(compiler, T_END) && !check_tok(compiler, T_ELSE) &&
           !check_tok(compiler, T_EOF)) {
        read_declr(compiler);
    }

    end_scope(compiler);
}

static void read_if_stmt(Compiler *compiler) {
    read_expr(compiler);
    eat_tok(compiler, T_THEN, U"expected 'then'  after if expression");

    int then_jump = emit_jump(compiler, OP_JMP_IF_FALSE);
    emit_bt(compiler, OP_POP);

    read_if_block(compiler);

    int else_jmp = emit_jump(compiler, OP_JMP);
    patch_jump(compiler, then_jump);
    emit_bt(compiler, OP_POP);
    if (match_tok(compiler, T_ELSE)) {
        start_scope(compiler);
        read_to_end(compiler);
        end_scope(compiler);
    } else {
        eat_tok(compiler, T_END, U"Expected end after if without else");
    }

    patch_jump(compiler, else_jmp);
}

static void read_while_stmt(Compiler *compiler) {
    int ls = cur_ins(compiler)->len;
    eat_tok(compiler, T_LPAREN, U"expected '(' after 'while'");
    read_expr(compiler);
    eat_tok(compiler, T_RPAREN, U"expected ')' after expression");
    int exitjmp = emit_jump(compiler, OP_JMP_IF_FALSE);
    emit_bt(compiler, OP_POP);
    start_scope(compiler);
    read_to_end(compiler);
    end_scope(compiler);
    // read_stmt();
    emit_loop(compiler, ls);
    patch_jump(compiler, exitjmp);
    emit_bt(compiler, OP_POP);
}

static void read_import_stmt(Compiler *compiler) {
    eat_tok(compiler, T_ID, U"expected import name");
    uint8_t index = make_id_const(compiler, &compiler->parser->prev);
    read_expr(compiler);
    if (check_tok(compiler, T_SEMICOLON)) {
        eat_tok(compiler, T_SEMICOLON,
                U"Expected semicolon after import file name");
    }
    emit_bt(compiler, OP_IMPORT_NONAME);
    emit_bt(compiler, index);
}

static void read_block(Compiler *compiler) {
    while (!check_tok(compiler, T_RBRACE) && !check_tok(compiler, T_EOF)) {
        read_declr(compiler);
    }

    eat_tok(compiler, T_RBRACE, U"Expected '}' after block stmts");
}

static void read_err_stmt(Compiler *compiler) {
    read_expr(compiler);
    if (check_tok(compiler, T_SEMICOLON)) {
        eat_tok(compiler, T_SEMICOLON, U"Expected ';' after error statement");
    }
    emit_bt(compiler, OP_ERR);
}

// read statement
//
// * Show/print statement
// * If Else
// * While
// * Return statement
// * Block statement {...}
// * Expression statement
//   example : 1 + 2 + 3;
static void read_stmt(Compiler *compiler) {
    if (match_tok(compiler, T_SHOW)) {
        read_print_stmt(compiler);
    } else if (match_tok(compiler, T_IF)) {
        read_if_stmt(compiler);
    } else if (match_tok(compiler, T_WHILE)) {
        read_while_stmt(compiler);
    } else if (match_tok(compiler, T_RETURN)) {
        return_stmt(compiler);
    } else if (match_tok(compiler, T_IMPORT)) {
        read_import_stmt(compiler);
    } else if (match_tok(compiler, T_MKERR)) {
        read_err_stmt(compiler);
    } else if (match_tok(compiler, T_LBRACE)) {
        start_scope(compiler);
        read_block(compiler);
        end_scope(compiler);
    } else {
        read_expr_stmt(compiler);
    }
}

ObjFunc *end_compiler(Compiler *compiler) {
    emit_return(compiler);
    ObjFunc *function = compiler->func;
#ifdef DEBUG_PRINT_CODE
    if (!compiler->parser->had_err) {
        dissm_ins_chunk(cur_ins(compiler), compiler->func->name != NULL
                                               ? compiler->func->name->chars
                                               : U"<script>");
    }
#endif
    // compiler = compiler->enclosing;
    compiler->parser->vm->compiler = compiler->enclosing;
    return function;
}

ObjFunc *compile(PankVm *vm, char32_t *source) {
    Parser parser;
    parser.vm = vm;
    parser.had_err = false;
    parser.panic_mode = false;
    Lexer lexer;

    boot_lexer(&lexer, source);
    parser.lexer = lexer;
    Compiler compiler;
    init_compiler(&parser, &compiler, NULL, FTYPE_SCRIPT);

    const int codelen = strlen32(source);
    vm->code = (char32_t *)calloc(codelen + 1, sizeof(char32_t));

    copy_c32(vm->code, source, strlen32(source));

    // init_comiler(compiler.vm  , &compiler, FTYPE_SCRIPT);
    //  compins = ins;
    // compiler.parser->had_err = false;
    // compiler.parser->panic_mode = false;
#ifdef DEBUG_LEXER
    Token tk = get_tok(&lexer);
    while (tk.type != T_EOF) {
 #ifdef PANK_OS_WIN
        char *tokstr = c_to_c(tk.start, 0);
        cp_println(L"TOK[%S][%.*S]\n", toktype_to_string(tk.type), tk.length,
                   tokstr);
 #else
        cp_println(L"TOK[%s][%.*ls]", toktype_to_string(tk.type), tk.length,
                   tk.start);
 #endif

        tk = get_tok(&lexer);
    }
    boot_lexer(&lexer, source);
#endif
    // advance();
    advance(compiler.parser);
    while (!match_tok(&compiler, T_EOF)) {
        read_declr(&compiler);
    }

    // wprintf(L"CUR->%s\n" , toktype_to_string(parser.cur.type));
    // read_expr();
    // advance();
    // eat_tok(T_EOF, L"Expected end of expr");
    ObjFunc *fn = end_compiler(&compiler);

    return parser.had_err ? NULL : fn;
}

ObjFunc *compile_module(PankVm *vm, char32_t *source) {
    Parser parser;
    parser.had_err = false;
    parser.panic_mode = false;
    parser.vm = vm;
    Lexer lexer;

    boot_lexer(&lexer, source);
    parser.lexer = lexer;
    Compiler modcompiler;
    init_compiler(&parser, &modcompiler, NULL, FTYPE_SCRIPT);
    advance(modcompiler.parser);
    while (!match_tok(&modcompiler, T_EOF)) {
        read_declr(&modcompiler);
    }

    ObjFunc *fn = end_compiler(&modcompiler);

    mark_compiler_roots(vm, &modcompiler);

    if (!parser.had_err) {
        make_changes_for_mod(&fn->ins);
    }

    return parser.had_err ? NULL : fn;
}

void mark_compiler_roots(PankVm *vm, Compiler *compiler) {
    Compiler *comp = compiler;
    while (comp != NULL) {
        // cp_color_println('b', L"COMP -> %d", comp->type);
        mark_obj(vm, (Obj *)comp->func);

        comp = comp->enclosing;
    }
}
