#include "include/lexer.h"

#include <bits/types/mbstate_t.h>
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

const char EN_KW_LET[] = "let";
const char EN_KW_SHOW[] = "show";
const char EN_KW_RETURN[] = "return";
const char EN_KW_IF[] = "if";
const char EN_KW_THEN[] = "then";
const char EN_KW_ELSE[] = "else";
const char EN_KW_END[] = "end";
const char EN_KW_WHILE[] = "while";
const char EN_KW_AND[] = "and";
const char EN_KW_OR[] = "or";
const char EN_KW_TRUE[] = "true";
const char EN_KW_FALSE[] = "false";
const char EN_KW_FUNC[] = "fun";
const char EN_KW_NIL[] = "nil";
const char EN_KW_IMPORT[] = "import";
const char EN_KW_PANIC[] = "panic";

const char BN_KW_LET[] = "ধরি";
const char BN_KW_SHOW[] = "দেখাও";
const char BN_KW_RETURN[] = "ফেরাও";
const char BN_KW_IF[] = "যদি";
const char BN_KW_THEN[] = "তাহলে";
const char BN_KW_ELSE[] = "নাহলে";
const char BN_KW_END[] = "শেষ";
const char BN_KW_WHILE[] = "যতক্ষণ";
const char BN_KW_AND[] = "এবং";
const char BN_KW_OR[] = "বা";
const char BN_KW_TRUE[] = "সত্যি";
const char BN_KW_FALSE[] = "মিথ্যা";
const char BN_KW_FUNC[] = "কাজ";
const char BN_KW_NIL[] = "নিল";
const char BN_KW_IMPORT[] = "আনয়ন";
const char BN_KW_PANIC[] = "গোলমাল";

const char PHON_KW_LET[] = "dhori";
const char PHON_KW_SHOW[] = "dekhao";
const char PHON_KW_RETURN[] = "fearo";
const char PHON_KW_IF[] = "jodi";
const char PHON_KW_THEN[] = "tahole";
const char PHON_KW_ELSE[] = "nahole";
const char PHON_KW_END[] = "sesh";
const char PHON_KW_WHILE[] = "jotokkhon";
const char PHON_KW_AND[] = "ebong";
const char PHON_KW_OR[] = "ba";
const char PHON_KW_TRUE[] = "sotti";
const char PHON_KW_FALSE[] = "mittha";
const char PHON_KW_FUNC[] = "kaj";
const char PHON_KW_NIL[] = "nil";
const char PHON_KW_IMPORT[] = "anoyon";
const char PHON_KW_PANIC[] = "golmal";

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
    };

    return "UNKNOWN_TOKEN";
}

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

// Lexer lexer;
char32_t next(Lexer *lexer) {
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
    lexer->line = 1;
}

bool is_eof(Lexer *lexer) { return *lexer->current == '\0'; }

Token mktok(Lexer *lexer, TokType type) {
    Token tok;
    tok.type = type;
    tok.start = lexer->start;
    tok.length = (int)(lexer->current - lexer->start);
    tok.line = lexer->line;
    return tok;
}

void btoe(char32_t *input, int len) {
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

Token mk_num_tok(Lexer *lexer) {
    Token tok;
    tok.type = T_NUM;
    tok.start = lexer->start;
    tok.length = (int)(lexer->current - lexer->start);
    tok.line = lexer->line;

    // btoe(tok.start, tok.length);
    //  wprintf(L"NUMBER-> %.*ls\n", tok.length, tok.start);
    //   convert_bn_num_to_en(tok.start, tok.length);
    return tok;
}

bool match_kw(const char *input, const char *kw) {
    return strncmp(input, kw, strlen(kw)) == 0;
}

TokType get_ident_tok_type(char32_t *input, int len) {
    TokType tt = T_ID;
    // wchar_t *tc = (wchar_t *)malloc(sizeof(wchar_t) * strlen16(input));
    // swprintf(tc, (size_t)len + 1, input);
    //  wprintf(L"TO_CHECK-> %ls\n" , tc);
    //
    char *tc = c_to_c(input, len);
    // wprintf(L"TC -> '%s' %d\n",tc , len);
    if (match_kw(tc, EN_KW_LET) || match_kw(tc, BN_KW_LET) || match_kw(tc, PHON_KW_LET)) {
        tt = T_LET;
    } else if (match_kw(tc, EN_KW_SHOW) || match_kw(tc, BN_KW_SHOW) || match_kw(tc, PHON_KW_SHOW)) {
        tt = T_SHOW;
    } else if (match_kw(tc, EN_KW_RETURN) || match_kw(tc, BN_KW_RETURN) || match_kw(tc, PHON_KW_RETURN)){
        tt = T_RETURN;
    } else if (match_kw(tc, EN_KW_IF) || match_kw(tc, BN_KW_IF) || match_kw(tc, PHON_KW_IF) ) {
        tt = T_IF;
    } else if (match_kw(tc, EN_KW_THEN) || match_kw(tc, BN_KW_THEN) || match_kw(tc, PHON_KW_THEN)) {
        tt = T_THEN;
    } else if (match_kw(tc, EN_KW_ELSE)|| match_kw(tc, BN_KW_ELSE) || match_kw(tc, PHON_KW_ELSE) ) {
        tt = T_ELSE;
    } else if (match_kw(tc, EN_KW_END) || match_kw(tc, BN_KW_END) || match_kw(tc, PHON_KW_END)) {
        tt = T_END;
    } else if (match_kw(tc, EN_KW_WHILE) || match_kw(tc, BN_KW_WHILE) || match_kw(tc, PHON_KW_WHILE)) {
        tt = T_WHILE;
    } else if (match_kw(tc, EN_KW_AND) || match_kw(tc, BN_KW_AND) || match_kw(tc, PHON_KW_AND)) {
        tt = T_AND;
    } else if (match_kw(tc, EN_KW_OR) || match_kw(tc, BN_KW_OR) || match_kw(tc, PHON_KW_OR)) {
        tt = T_OR;
    } else if (match_kw(tc, EN_KW_TRUE) || match_kw(tc, BN_KW_TRUE) || match_kw(tc, PHON_KW_TRUE)) {
        tt = T_TRUE;
    } else if (match_kw(tc, EN_KW_FALSE) || match_kw(tc, BN_KW_FALSE) || match_kw(tc, PHON_KW_FALSE)) {
        tt = T_FALSE;
    } else if (match_kw(tc, EN_KW_FUNC) || match_kw(tc, BN_KW_FUNC) || match_kw(tc, PHON_KW_FUNC)) {
        tt = T_FUNC;
    } else if (match_kw(tc, EN_KW_NIL) || match_kw(tc, BN_KW_NIL)) {
        tt = T_NIL;
    }else if (match_kw(tc, EN_KW_IMPORT) || match_kw(tc, BN_KW_IMPORT) || match_kw(tc, PHON_KW_IMPORT)) {
    tt = T_IMPORT;
    }else if (match_kw(tc, EN_KW_PANIC) || match_kw(tc, BN_KW_PANIC) || match_kw(tc, PHON_KW_PANIC)) {
    tt = T_MKERR;
    }
    /*if (strncmp(tc, EN_KW_LET, len) == 0 ||
        strncmp(tc, PHON_KW_LET, len) == 0 ||
        strncmp(tc, BN_KW_LET, len) == 0) {
        tt = T_LET;
    } else if (strncmp(tc, EN_KW_SHOW, len) == 0 ||
               strncmp(tc, PHON_KW_SHOW, len) == 0 ||
               strncmp(tc, BN_KW_SHOW, len) == 0) {
        tt = T_SHOW;
    } else if (strncmp(tc, EN_KW_RETURN, len) == 0 ||
               strncmp(tc, PHON_KW_RETURN, len) == 0 ||
               strncmp(tc, BN_KW_RETURN, len) == 0) {
        tt = T_RETURN;
    } else if (strncmp(tc, EN_KW_IF, len) == 0 ||
               strncmp(tc, PHON_KW_IF, len) == 0 ||
               strncmp(tc, BN_KW_IF, len) == 0) {
        tt = T_IF;
    } else if (strncmp(tc, EN_KW_THEN, len) == 0 ||
               strncmp(tc, PHON_KW_THEN, len) == 0 ||
               strncmp(tc, BN_KW_THEN, len) == 0) {
        tt = T_THEN;
    } else if (strncmp(tc, EN_KW_ELSE, len) == 0 ||
               strncmp(tc, PHON_KW_ELSE, len) == 0) {
        tt = T_ELSE;
    } else if (strncmp(tc, EN_KW_END, len) == 0 ||
               strncmp(tc, PHON_KW_END, len) == 0 ||
               strncmp(tc, BN_KW_END, len) == 0) {
        tt = T_END;

    } else if (strncmp(tc, EN_KW_WHILE, len) == 0 ||
               strncmp(tc, PHON_KW_WHILE, len) == 0 ||
               strncmp(tc, BN_KW_WHILE, len) == 0) {
        tt = T_WHILE;
    } else if (strncmp(tc, EN_KW_AND, len) == 0 ||
               strncmp(tc, PHON_KW_AND, len) == 0 ||
               strncmp(tc, BN_KW_AND, len) == 0) {
        tt = T_AND;
    } else if (strncmp(tc, EN_KW_OR, len) == 0 ||
               strncmp(tc, PHON_KW_OR, len) == 0 ||
               strncmp(tc, BN_KW_OR, len) == 0) {
        tt = T_OR;
    } else if (strncmp(tc, EN_KW_TRUE, len) == 0 ||
               strncmp(tc, PHON_KW_TRUE, len) == 0 ||
               strncmp(tc, BN_KW_TRUE, len) == 0) {
        tt = T_TRUE;
    } else if (strncmp(tc, EN_KW_FALSE, len) == 0 ||
               strncmp(tc, PHON_KW_FALSE, len) == 0 ||
               strncmp(tc, BN_KW_FALSE, len) == 0) {
        tt = T_FALSE;
    } else if (strncmp(tc, EN_KW_FUNC, len) == 0 ||
               strncmp(tc, PHON_KW_FUNC, len) == 0 ||
               strncmp(tc, BN_KW_FUNC, len) == 0) {
        tt = T_FUNC;
    } else if (strncmp(tc, EN_KW_NIL, len) == 0 ||
               strncmp(tc, BN_KW_NIL, len) == 0) {
        tt = T_NIL;
    } else if (strncmp(tc, EN_KW_IMPORT, len) == 0 ||
               strncmp(tc, PHON_KW_IMPORT, len) == 0 ||
               strncmp(tc, BN_KW_IMPORT, len) == 0) {
        tt = T_IMPORT;
    } else if (strncmp(tc, EN_KW_PANIC, len) == 0 ||
               strncmp(tc, PHON_KW_PANIC, len) == 0 ||
               strncmp(tc, BN_KW_PANIC, len) == 0) {
        tt = T_MKERR;

    } else {
        tt = T_ID;
    }*/

    free(tc);

    return tt;
}

Token mk_id_tok(Lexer *lexer) {
    Token tok;
    tok.start = lexer->start;
    tok.length = (int)(lexer->current - lexer->start);
    tok.line = lexer->line;
    tok.type = get_ident_tok_type(tok.start, tok.length);
    return tok;
}

Token err_tok(Lexer *lexer, char32_t *msg) {
    Token tk;
    tk.type = T_ERR;
    tk.start = msg;
    tk.length = (int)strlen16(msg);
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

    return err_tok(lexer, U"Unknown character");
}
