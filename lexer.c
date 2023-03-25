#include "include/lexer.h"

#include <locale.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#include "include/bn.h"
// #include "include/token.h"

static wchar_t BN_KW_LET[] = L"ধরি";
static wchar_t BN_KW_SHOW[] = L"দেখাও";
static wchar_t BN_KW_RETURN[] = L"ফেরাও";
static wchar_t BN_KW_AND[] = L"এবং";
static wchar_t BN_KW_OR[] = L"বা";
static wchar_t BN_KW_IF[] = L"যদি";
static wchar_t BN_KW_THEN[] = L"তাহলে";
static wchar_t BN_KW_ELSE[] = L"নাহলে";
static wchar_t BN_KW_END[] = L"শেষ";
static wchar_t BN_KW_WHILE[] = L"যতক্ষণ";
static wchar_t BN_KW_TRUE[] = L"সত্যি";
static wchar_t BN_KW_FALSE[] = L"মিথ্যা";
static wchar_t BN_KW_NIL[] = L"শূন্য";
static wchar_t BN_KW_FUNC[] = L"কাজ";

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
    };

    return "UNKNOWN_TOKEN";
}

char *token_to_string(Token *tk) {
    char *result = NULL;
    sprintf(result, "T[%s]", toktype_to_string(tk->type));
    return result;
}

Lexer lexer;

bool match_char(wchar_t c) {
    if (is_eof()) return false;
    if (*lexer.current != c) return false;
    lexer.current++;
    return true;
}

wchar_t peek() { return *lexer.current; }
wchar_t peek_next() {
    if (is_eof()) return '\0';
    return lexer.current[1];
}

void boot_lexer(wchar_t *src) {
    lexer.start = src;
    lexer.current = src;
    lexer.line = 1;
}

bool is_eof() { return *lexer.current == '\0'; }

Token mktok(TokType type) {
    Token tok;
    tok.type = type;
    tok.start = lexer.start;
    tok.length = (int)(lexer.current - lexer.start);
    tok.line = lexer.line;
    return tok;
}

void btoe(wchar_t *input, int len) {
    for (int i = 0; i < len; i++) {
        if (input[i] == BN_NUM_ONE) {
            input[i] = '1';
        } else if (input[i] == BN_NUM_TWO) {
            input[i] = '2';
        } else if (input[i] == BN_NUM_THREE) {
            input[i] = '3';
        } else if (input[i] == BN_NUM_FOUR) {
            input[i] = '4';
        } else if (input[i] == BN_NUM_FIVE) {
            input[i] = '5';
        } else if (input[i] == BN_NUM_SIX) {
            input[i] = '6';
        } else if (input[i] == BN_NUM_SEVEN) {
            input[i] = '7';
        } else if (input[i] == BN_NUM_EIGHT) {
            input[i] = '8';
        } else if (input[i] == BN_NUM_NINE) {
            input[i] = '9';
        } else if (input[i] == BN_NUM_ZERO) {
            input[i] = '0';
        }
    }
}

Token mk_num_tok() {
    Token tok;
    tok.type = T_NUM;
    tok.start = lexer.start;
    tok.length = (int)(lexer.current - lexer.start);
    tok.line = lexer.line;

    btoe(tok.start, tok.length);
    // wprintf(L"NUMBER-> %.*ls\n", tok.length, tok.start);
    //  convert_bn_num_to_en(tok.start, tok.length);
    return tok;
}

TokType get_ident_tok_type(wchar_t *input, int len) {
    TokType tt = T_LET;
    wchar_t *tc = (wchar_t *)malloc(sizeof(wchar_t) * wcslen(input));
    swprintf(tc, (size_t)len + 1, input);
    // wprintf(L"TO_CHECK-> %ls\n" , tc);
    //
    if (wcscmp(tc, L"let") == 0 || wcscmp(tc, L"dhori") == 0 ||
        wcscmp(tc, BN_KW_LET) == 0) {
        tt = T_LET;
    } else if (wcscmp(tc, L"show") == 0 || wcscmp(tc, L"dekhao") == 0 ||
               wcscmp(tc, BN_KW_SHOW) == 0) {
        tt = T_SHOW;
    } else if (wcscmp(tc, L"return") == 0 || wcscmp(tc, L"fearo") == 0 ||
               wcscmp(tc, BN_KW_RETURN) == 0) {
        tt = T_RETURN;
    } else if (wcscmp(tc, L"if") == 0 || wcscmp(tc, L"jodi") == 0 ||
               wcscmp(tc, BN_KW_IF) == 0) {
        tt = T_IF;
    } else if (wcscmp(tc, L"then") == 0 || wcscmp(tc, L"tahole") == 0 ||
               wcscmp(tc, BN_KW_THEN) == 0) {
        tt = T_THEN;
    } else if (wcscmp(tc, L"else") == 0 || wcscmp(tc, L"nahole") == 0 ||
               wcscmp(tc, BN_KW_ELSE) == 0) {
        tt = T_ELSE;
    } else if (wcscmp(tc, L"end") == 0 || wcscmp(tc, L"sesh") == 0 ||
               wcscmp(tc, BN_KW_END) == 0) {
        tt = T_END;

    } else if (wcscmp(tc, L"while") == 0 || wcscmp(tc, L"jotokkhon") == 0 ||
               wcscmp(tc, BN_KW_WHILE) == 0) {
        tt = T_WHILE;
    } else if (wcscmp(tc, L"and") == 0 || wcscmp(tc, L"ebong") == 0 ||
               wcscmp(tc, BN_KW_AND) == 0) {
        tt = T_AND;
    } else if (wcscmp(tc, L"or") == 0 || wcscmp(tc, L"ba") == 0 ||
               wcscmp(tc, BN_KW_OR) == 0) {
        tt = T_OR;
    } else if (wcscmp(tc, L"true") == 0 || wcscmp(tc, L"sotti") == 0 ||
               wcscmp(tc, BN_KW_TRUE) == 0) {
        tt = T_TRUE;
    } else if (wcscmp(tc, L"false") == 0 || wcscmp(tc, L"mittha") == 0 ||
               wcscmp(tc, BN_KW_FALSE) == 0) {
        tt = T_FALSE;
    } else if (wcscmp(tc, L"fun") == 0 || wcscmp(tc, L"kaj") == 0 ||
               wcscmp(tc, BN_KW_FUNC) == 0) {
        tt = T_FUNC;
    } else if (wcscmp(tc, L"nil") == 0 || wcscmp(tc, L"nil") == 0 ||
               wcscmp(tc, BN_KW_NIL) == 0) {
        tt = T_NIL;
    } else if (wcscmp(tc, L"import") == 0) {
        tt = T_IMPORT;

    } else {
        tt = T_ID;
    }

    free(tc);

    return tt;
}

Token mk_id_tok() {
    Token tok;
    tok.start = lexer.start;
    tok.length = (int)(lexer.current - lexer.start);
    tok.line = lexer.line;
    tok.type = get_ident_tok_type(tok.start, tok.length);
    return tok;
}

Token err_tok(wchar_t *msg) {
    Token tk;
    tk.type = T_ERR;
    tk.start = msg;
    tk.length = (int)wcslen(msg);
    tk.line = lexer.line;
    return tk;
}

bool is_en_num(wchar_t c) { return c >= '0' && c <= '9'; }
bool is_en_alpha(wchar_t c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

void skip_ws() {
    for (;;) {
        wchar_t c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                next();
                break;
            case '\n':
                lexer.line++;
                next();
                break;
            case '#':
                while (peek() != '\n' && !is_eof()) {
                    next();
                }
                break;
            default:
                return;
        }
    }
}

wchar_t next() {
    lexer.current++;
    return lexer.current[-1];
}

Token get_str_tok() {
    while (peek() != '"' && !is_eof()) {
        if (peek() == '\n') {
            lexer.line++;
        }

        next();
    }

    if (is_eof()) {
        return err_tok(L"String is not terminated!");
    }
    next();
    return mktok(T_STR);
}

Token get_ident_tok() {
    while ((is_en_alpha(peek()) || is_bn_char(peek())) || is_en_num(peek())) {
        next();
    }

    return mk_id_tok();
}

Token get_num_tok() {
    while (is_bn_num(peek()) || is_en_num(peek())) {
        next();
    }

    if (peek() == '.' && (is_bn_num(peek_next()) || is_en_num(peek_next()))) {
        next();
        while (is_bn_num(peek()) || is_en_num(peek())) {
            next();
        }
    }

    return mk_num_tok();
}

Token get_tok() {
    skip_ws();

    lexer.start = lexer.current;
    if (is_eof()) return mktok(T_EOF);

    wchar_t c = next();
    if (is_bn_num(c) || is_en_num(c)) {
        return get_num_tok();
        // wprintf(L"is number ");
    }
    if (is_bn_char(c) || is_en_alpha(c)) {
        return get_ident_tok();
    }
    switch (c) {
        case '(':
            return mktok(T_LPAREN);
        case ')':
            return mktok(T_RPAREN);
        case '{':
            return mktok(T_LBRACE);
        case '}':
            return mktok(T_RBRACE);
        case '-':
            return mktok(T_MINUS);
        case '+':
            return mktok(T_PLUS);
        case '/':
            return mktok(T_DIV);
        case '*':
            return mktok(T_ASTR);
        case '.':
            return mktok(T_DOT);
        case ';':
            return mktok(T_SEMICOLON);
        case ',':
            return mktok(T_COMMA);
        case '!':
            if (match_char('=')) {
                return mktok(T_NOTEQ);
            } else {
                return mktok(T_BANG);
            }
        case '=':
            if (match_char('=')) {
                return mktok(T_EQEQ);
            } else {
                return mktok(T_EQ);
            }
        case '>':
            if (match_char('=')) {
                return mktok(T_GTE);
            } else {
                return mktok(T_GT);
            }
        case '<':
            if (match_char('=')) {
                return mktok(T_LTE);
            } else {
                return mktok(T_LT);
            }

        case '"':
            return get_str_tok();
    }

    // wprintf(L"L-> %lc ", c);

    return err_tok(L"Unknown character");
}
