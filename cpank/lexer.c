/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#include "include/lexer.h"

#include <locale.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>
#include <wctype.h>

#include "include/bn.h"
// #include "include/token.h"
#include "include/utils.h"
#include "include/value.h"

const char32_t EN_KW_LET[] = U"let";
const char32_t EN_KW_SHOW[] = U"show";
const char32_t EN_KW_RETURN[] = U"return";
const char32_t EN_KW_IF[] = U"if";
const char32_t EN_KW_THEN[] = U"then";
const char32_t EN_KW_ELSE[] = U"else";
const char32_t EN_KW_END[] = U"end";
const char32_t EN_KW_WHILE[] = U"while";
const char32_t EN_KW_AND[] = U"and";
const char32_t EN_KW_OR[] = U"or";
const char32_t EN_KW_TRUE[] = U"true";
const char32_t EN_KW_FALSE[] = U"false";
const char32_t EN_KW_FUNC[] = U"fun";
const char32_t EN_KW_NIL[] = U"nil";
const char32_t EN_KW_IMPORT[] = U"import";
const char32_t EN_KW_PANIC[] = U"panic";

const char32_t BN_KW_LET[] = U"\u09a7\u09b0\u09bf";

const char32_t BN_KW_SHOW[] = U"\u09a6\u09c7\u0996\u09be\u0993";
const char32_t BN_KW_RETURN[] = U"\u09ab\u09c7\u09b0\u09be\u0993";
const char32_t BN_KW_IF[] = U"\u09af\u09a6\u09bf";
const char32_t BN_KW_THEN[] = U"\u09a4\u09be\u09b9\u09b2\u09c7";
const char32_t BN_KW_ELSE[] = U"\u09a8\u09be\u09b9\u09b2\u09c7";
const char32_t BN_KW_END[] = U"\u09b6\u09c7\u09b7";
const char32_t BN_KW_WHILE[] = U"\u09af\u09a4\u0995\u09cd\u09b7\u09a3";
const char32_t BN_KW_AND[] = U"\u098f\u09ac\u0982";
const char32_t BN_KW_OR[] = U"\u09ac\u09be";
const char32_t BN_KW_TRUE[] = U"\u09b8\u09a4\u09cd\u09af\u09bf";
const char32_t BN_KW_FALSE[] = U"\u09ae\u09bf\u09a5\u09cd\u09af\u09be";
const char32_t BN_KW_FUNC[] = U"\u0995\u09be\u099c";
const char32_t BN_KW_NIL[] = U"\u09a8\u09bf\u09b2";
const char32_t BN_KW_IMPORT[] = U"\u0986\u09a8\u09df\u09a8";
const char32_t BN_KW_PANIC[] = U"\u0997\u09cb\u09b2\u09ae\u09be\u09b2";

const char32_t PHON_KW_LET[] = U"dhori";
const char32_t PHON_KW_SHOW[] = U"dekhao";
const char32_t PHON_KW_RETURN[] = U"fearo";
const char32_t PHON_KW_IF[] = U"jodi";
const char32_t PHON_KW_THEN[] = U"tahole";
const char32_t PHON_KW_ELSE[] = U"nahole";
const char32_t PHON_KW_END[] = U"sesh";
const char32_t PHON_KW_WHILE[] = U"jotokkhon";
const char32_t PHON_KW_AND[] = U"ebong";
const char32_t PHON_KW_OR[] = U"ba";
const char32_t PHON_KW_TRUE[] = U"sotti";
const char32_t PHON_KW_FALSE[] = U"mittha";
const char32_t PHON_KW_FUNC[] = U"kaj";
const char32_t PHON_KW_NIL[] = U"nil";
const char32_t PHON_KW_IMPORT[] = U"anoyon";
const char32_t PHON_KW_PANIC[] = U"golmal";

const char *toktype_to_string(TokType t) {
    switch (t) {
        case T_LPAREN:
            return "T_LPAREN";
        case T_RPAREN:
            return "T_RPAREN";
        case T_LBRACE:
            return "T_LBRACE";
        case T_RBRACE:
            return "T_RBRACE";
        case T_MINUS:
            return "T_MINUS";
        case T_PLUS:
            return "T_PLUS";
        case T_DIV:
            return "T_DIV";
        case T_ASTR:
            return "T_ASTR";
        case T_SEMICOLON:
            return "T_SEMICOLON";
        case T_COMMA:
            return "T_COMMA";
        case T_EQ:
            return "T_EQ";
        case T_EQEQ:
            return "T_EQEQ";
        case T_GT:
            return "T_GT";
        case T_GTE:
            return "T_GTE";
        case T_LT:
            return "T_LT";
        case T_LTE:
            return "T_LTE";
        case T_ID:
            return "T_ID";
        case T_STR:
            return "T_STR";
        case T_NUM:
            return "T_NUM";
        case T_LET:
            return "T_LET";
        case T_RETURN:
            return "T_RETURN";
        case T_AND:
            return "T_AND";
        case T_OR:
            return "T_OR";
        case T_END:
            return "T_END";
        case T_IF:
            return "T_IF";
        case T_THEN:
            return "T_THEN";
        case T_ELSE:
            return "T_ELSE";
        case T_WHILE:
            return "T_WHILE";
        case T_SHOW:
            return "T_SHOW";
        case T_NIL:
            return "T_NIL";
        case T_TRUE:
            return "T_TRUE";
        case T_FALSE:
            return "T_FALSE";
        case T_FUNC:
            return "T_FUNC";
        case T_BANG:
            return "T_BANG";
        case T_DOT:
            return "T_DOT";
        case T_IMPORT:
            return "T_IMPORT";
        case T_NOTEQ:
            return "T_NOTEQ";
        case T_EOF:
            return "T_EOF";
        case T_ERR:
            return "T_ERR";
        case T_MKERR:
            return "T_MKERR";
        case T_LSBRACKET:
            return "T_LSBRACKET";
        case T_RSBRACKET:
            return "T_RSBRACKET";
        case T_COLON:
            return "T_COLON";
        case T_UNKNOWN:
            return "T_UNKNOWN";
    };

    return "UNKNOWN_TOKEN";
}

// cppcheck-suppress unusedFunction
char *token_to_string(const Token *t) {
    // wchar_t * wstr = (wchar_t*)malloc(sizeof(char16_t) * t->length);
    // mbstowcs(wstr , t->start , t->length);
    // size_t size_to_write = sizeof(char) * 16;
    // char *result = (char *)malloc(size_to_write);
    // snprintf(result, size_to_write, "T[%s]", toktype_to_string(tk->type));
    // return result;

    // wprintf(L"char16_t ->>>>>> '%s'\n" , y);

    return "t";
}

char32_t *get_line(Lexer *lexer, int line) {
    char32_t *resultline = NULL;
    if (lexer->line < line) {
        return NULL;
    }
    int lineindex = line - 1;
    int linecount = 0;
    char32_t **lines = split32(lexer->src, U"\n", &linecount);
    if (lines == NULL) {
        return NULL;
    }
    for (int i = 0; i < linecount; i++) {
        if (i == lineindex) {
            char32_t *rawline = lines[i];
            int len = strlen32(rawline);
            resultline = (char32_t *)calloc(len + 1, sizeof(char32_t));
            if (resultline == NULL) {
                free(lines[i]);
                continue;
            }
            copy_c32(resultline, rawline, len);
            resultline[len] = U'\0';
            free(lines[i]);
            // return resultline;
        } else {
            free(lines[i]);
        }
    }

    free(lines);

    return resultline;
}

// Lexer lexer;
char32_t next(Lexer *lexer) {
    lexer->col++;
    lexer->current++;
    return lexer->current[-1];
}
bool match_char(Lexer *lexer, char32_t c) {
    if (is_eof(lexer)) return false;
    if (*lexer->current != c) return false;
    lexer->current++;
    return true;
}

char32_t peek(Lexer *lexer) { return *lexer->current; }
char32_t peek_next(Lexer *lexer) {
    if (is_eof(lexer)) return '\0';
    return lexer->current[1];
}

void boot_lexer(Lexer *lexer, char32_t *src) {
    lexer->start = src;
    lexer->current = src;
    lexer->src = src;
    lexer->line = 1;
    lexer->col = 1;
}

bool is_eof(Lexer *lexer) { return *lexer->current == '\0'; }

Token mktok(Lexer *lexer, TokType type) {
    Token tok;
    tok.type = type;
    tok.start = lexer->start;
    tok.length = (int)(lexer->current - lexer->start);
    tok.line = lexer->line;
    tok.colpos = lexer->col;
    return tok;
}

Token mk_num_tok(Lexer *lexer) {
    Token tok;
    tok.type = T_NUM;
    tok.start = lexer->start;
    tok.length = (int)(lexer->current - lexer->start);
    tok.line = lexer->line;
    tok.colpos = lexer->col;
    conv_bn_to_en_num(tok.start, tok.length);
    return tok;
}

bool match_kw(const char32_t *input, const char32_t *kw, int len) {
    return str32cmp_n(input, kw, len);
}

TokType get_ident_tok_type(char32_t *input, int len) {
    TokType tt = T_ID;
    char32_t *tc = input;

    if (match_kw(tc, EN_KW_LET, len) || match_kw(tc, BN_KW_LET, len) ||
        match_kw(tc, PHON_KW_LET, len)) {
        tt = T_LET;
    } else if (match_kw(tc, EN_KW_SHOW, len) || match_kw(tc, BN_KW_SHOW, len) ||
               match_kw(tc, PHON_KW_SHOW, len)) {
        tt = T_SHOW;
    } else if (match_kw(tc, EN_KW_RETURN, len) ||
               match_kw(tc, BN_KW_RETURN, len) ||
               match_kw(tc, PHON_KW_RETURN, len)) {
        tt = T_RETURN;
    } else if (match_kw(tc, EN_KW_IF, len) || match_kw(tc, BN_KW_IF, len) ||
               match_kw(tc, PHON_KW_IF, len)) {
        tt = T_IF;
    } else if (match_kw(tc, EN_KW_THEN, len) || match_kw(tc, BN_KW_THEN, len) ||
               match_kw(tc, PHON_KW_THEN, len)) {
        tt = T_THEN;
    } else if (match_kw(tc, EN_KW_ELSE, len) || match_kw(tc, BN_KW_ELSE, len) ||
               match_kw(tc, PHON_KW_ELSE, len)) {
        tt = T_ELSE;
    } else if (match_kw(tc, EN_KW_END, len) || match_kw(tc, BN_KW_END, len) ||
               match_kw(tc, PHON_KW_END, len)) {
        tt = T_END;
    } else if (match_kw(tc, EN_KW_WHILE, len) ||
               match_kw(tc, BN_KW_WHILE, len) ||
               match_kw(tc, PHON_KW_WHILE, len)) {
        tt = T_WHILE;
    } else if (match_kw(tc, EN_KW_AND, len) || match_kw(tc, BN_KW_AND, len) ||
               match_kw(tc, PHON_KW_AND, len)) {
        tt = T_AND;
    } else if (match_kw(tc, EN_KW_OR, len) || match_kw(tc, BN_KW_OR, len) ||
               match_kw(tc, PHON_KW_OR, len)) {
        tt = T_OR;
    } else if (match_kw(tc, EN_KW_TRUE, len) || match_kw(tc, BN_KW_TRUE, len) ||
               match_kw(tc, PHON_KW_TRUE, len)) {
        tt = T_TRUE;
    } else if (match_kw(tc, EN_KW_FALSE, len) ||
               match_kw(tc, BN_KW_FALSE, len) ||
               match_kw(tc, PHON_KW_FALSE, len)) {
        tt = T_FALSE;
    } else if (match_kw(tc, EN_KW_FUNC, len) || match_kw(tc, BN_KW_FUNC, len) ||
               match_kw(tc, PHON_KW_FUNC, len)) {
        tt = T_FUNC;
    } else if (match_kw(tc, EN_KW_NIL, len) || match_kw(tc, BN_KW_NIL, len)) {
        tt = T_NIL;
    } else if (match_kw(tc, EN_KW_IMPORT, len) ||
               match_kw(tc, BN_KW_IMPORT, len) ||
               match_kw(tc, PHON_KW_IMPORT, len)) {
        tt = T_IMPORT;
    } else if (match_kw(tc, EN_KW_PANIC, len) ||
               match_kw(tc, BN_KW_PANIC, len) ||
               match_kw(tc, PHON_KW_PANIC, len)) {
        tt = T_MKERR;
    }

    return tt;
}

Token mk_id_tok(Lexer *lexer) {
    Token tok;
    tok.start = lexer->start;
    tok.length = (int)(lexer->current - lexer->start);
    tok.line = lexer->line;
    tok.type = get_ident_tok_type(tok.start, tok.length);
    tok.colpos = lexer->col;
    return tok;
}

Token err_tok(Lexer *lexer, char32_t *msg) {
    Token tk;
    tk.type = T_ERR;
    tk.start = msg;
    tk.length = (int)strlen32(msg);
    tk.line = lexer->line;
    tk.colpos = lexer->col;
    return tk;
}

Token unknown_tok(Lexer *lexer) {
    Token tk;
    tk.type = T_UNKNOWN;
    tk.start = lexer->start;
    tk.length = (int)(lexer->current - lexer->start);
    tk.colpos = lexer->col;
    tk.line = lexer->line;
    return tk;
}

bool is_en_num(char32_t c) { return c >= '0' && c <= '9'; }
bool is_en_alpha(char32_t c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

void skip_ws(Lexer *lexer) {
    for (;;) {
        char32_t c = peek(lexer);
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                next(lexer);
                break;
            case '\n':
                lexer->line++;
                lexer->col = 1;
                next(lexer);
                break;
            case '#':
                while (peek(lexer) != '\n' && !is_eof(lexer)) {
                    next(lexer);
                }
                break;
            default:
                return;
        }
    }
}

Token get_str_tok(Lexer *lexer) {
    while (peek(lexer) != '"' && !is_eof(lexer)) {
        if (peek(lexer) == '\n') {
            lexer->line++;
        }

        next(lexer);
    }

    if (is_eof(lexer)) {
        return err_tok(lexer, U"String is not terminated!");
    }
    next(lexer);
    return mktok(lexer, T_STR);
}

Token get_ident_tok(Lexer *lexer) {
    while ((is_en_alpha(peek(lexer)) || is_bn_char(peek(lexer))) ||
           is_en_num(peek(lexer))) {
        next(lexer);
    }

    return mk_id_tok(lexer);
}

Token get_num_tok(Lexer *lexer) {
    while (is_bn_num(peek(lexer)) || is_en_num(peek(lexer))) {
        next(lexer);
    }

    if (peek(lexer) == '.' &&
        (is_bn_num(peek_next(lexer)) || is_en_num(peek_next(lexer)))) {
        next(lexer);
        while (is_bn_num(peek(lexer)) || is_en_num(peek(lexer))) {
            next(lexer);
        }
    }

    return mk_num_tok(lexer);
}

Token get_tok(Lexer *lexer) {
    skip_ws(lexer);

    lexer->start = lexer->current;
    if (is_eof(lexer)) return mktok(lexer, T_EOF);

    char32_t c = next(lexer);
    if (is_bn_num(c) || is_en_num(c)) {
        return get_num_tok(lexer);
        // wprintf(L"is number ");
    }
    if (is_bn_char(c) || is_en_alpha(c)) {
        return get_ident_tok(lexer);
    }
    switch (c) {
        case '(':
            return mktok(lexer, T_LPAREN);
        case ')':
            return mktok(lexer, T_RPAREN);
        case '{':
            return mktok(lexer, T_LBRACE);
        case '}':
            return mktok(lexer, T_RBRACE);
        case '[':
            return mktok(lexer, T_LSBRACKET);
        case ']':
            return mktok(lexer, T_RSBRACKET);
        case '-':
            return mktok(lexer, T_MINUS);
        case '+':
            return mktok(lexer, T_PLUS);
        case '/':
            return mktok(lexer, T_DIV);
        case '*':
            return mktok(lexer, T_ASTR);
        case '.':
            return mktok(lexer, T_DOT);
        case ';':
            return mktok(lexer, T_SEMICOLON);
        case ':':
            return mktok(lexer, T_COLON);
        case ',':
            return mktok(lexer, T_COMMA);
        case '!':
            if (match_char(lexer, '=')) {
                return mktok(lexer, T_NOTEQ);
            } else {
                return mktok(lexer, T_BANG);
            }
        case '=':
            if (match_char(lexer, '=')) {
                return mktok(lexer, T_EQEQ);
            } else {
                return mktok(lexer, T_EQ);
            }
        case '>':
            if (match_char(lexer, '=')) {
                return mktok(lexer, T_GTE);
            } else {
                return mktok(lexer, T_GT);
            }
        case '<':
            if (match_char(lexer, '=')) {
                return mktok(lexer, T_LTE);
            } else {
                return mktok(lexer, T_LT);
            }

        case '"':
            return get_str_tok(lexer);
    }

    // wprintf(L"L-> %lc ", c);

    return unknown_tok(lexer);
}
