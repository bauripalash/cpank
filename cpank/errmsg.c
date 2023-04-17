#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>

#include "include/errmsgs.h"

char32_t ERR_MSG_UNKNOWN_CHAR[] = U"Unknown Character";
char32_t ERR_MSG_UNKNOWN_CHAR_BN[] =
    U"\u0985\u09aa\u09b0\u09bf\u099a\u09bf\u09a4 "
    U"\u0985\u0995\u09cd\u09b7\u09b0";

char32_t ERR_MSG_RBRAC_AFTER_ARRLIT[] = U"Expected ']' after array literal";
char32_t ERR_MSG_RBRAC_AFTER_ARRLIT_BN[] =
    U"\u09a4\u09be\u09b2\u09bf\u0995\u09be\u09b0 \u09aa\u09b0 "
    U"\u098f\u0995\u099f\u09bf "
    U"']' \u09aa\u09be\u0993\u09df\u09be "
    U"\u0989\u099a\u09bf\u09a4 \u099b\u09bf\u09b2";

char32_t ERR_MSG_RBRAC_AFTER_ARGS[] = U"Expected ')' after arguments";
char32_t ERR_MSG_RBRAC_AFTER_ARGS_BN[] =
    U"\u099a\u09b2\u09b0\u09be\u09b6\u09bf\u09b0 "
    U"\u09a4\u09be\u09b2\u09bf\u0995\u09be\u09b0 \u09aa\u09b0 "
    U"\u098f\u0995\u099f\u09bf ')' \u09aa\u09be\u0993\u09df\u09be "
    U"\u0989\u099a\u09bf\u09a4 \u099b\u09bf\u09b2";

char32_t ERR_MSG_NO_VAR_AFTER_LET[] = U"Expected variable name after let";
char32_t ERR_MSG_NO_VAR_AFTER_LET_BN[] =
    U"\u09a7\u09b0\u09bf -\u098f\u09b0 \u09aa\u09b0 \u098f\u0995\u099f\u09bf "
    U"\u099a\u09b2\u09b0\u09be\u09b6\u09bf\u09b0 \u09a8\u09be\u09ae "
    U"\u09aa\u09be\u0993\u09df\u09be \u0989\u099a\u09bf\u09a4 "
    U"\u099b\u09bf\u09b2";

char32_t ERR_MSG_LBRAC_AFTER_FUNC_NAME[] = U"Expected '(' after func name";
char32_t ERR_MSG_LBRAC_AFTER_FUNC_NAME_BN[] =
    U"\u0995\u09be\u099c\u09c7\u09b0 \u09a8\u09be\u09ae\u09c7\u09b0 "
    U"\u09aa\u09b0\u09c7 \u098f\u0995\u099f\u09bf '(' "
    U"\u09aa\u09be\u0993\u09df\u09be \u0989\u099a\u09bf\u09a4 "
    U"\u099b\u09bf\u09b2";
static inline bool is_bn(void) {
#if defined(MODE_BENGALI)
    const char* lc = getenv("ENMODE");
    if (lc == NULL) {
        return true;
    }
    if (strcmp(lc, "1") == 0 || strcmp(lc, "yes") == 0 ||
        strcmp(lc, "y") == 0 || strcmp(lc, "true") == 0) {
        return false;
    } else {
        return true;
    }
#else
    const char* lc = getenv("BNMODE");
    if (lc == NULL) {
        return false;
    }
    if (strcmp(lc, "1") == 0 || strcmp(lc, "yes") == 0 ||
        strcmp(lc, "y") == 0 || strcmp(lc, "true") == 0) {
        return true;
    } else {
        return false;
    }

#endif
}

#define GIVE_ERRMSG(msg) \
 if (is_bn()) {          \
  return msg##_BN;       \
 } else {                \
  return msg;            \
 }

char32_t* geterrmsg(int code) {
    switch (code) {
        case EMSG_UNKNOWN_CHAR:
            GIVE_ERRMSG(ERR_MSG_UNKNOWN_CHAR)
        case EMSG_RBRAC_AFTER_ARR:
            GIVE_ERRMSG(ERR_MSG_RBRAC_AFTER_ARRLIT)
        case EMSG_RBRAC_AFTER_ARG:
            GIVE_ERRMSG(ERR_MSG_RBRAC_AFTER_ARGS)
        case EMSG_NO_VAR_AFTER_LET:
            GIVE_ERRMSG(ERR_MSG_NO_VAR_AFTER_LET)
        case EMSG_LBRAC_AFTER_FUNC_NAME:
            GIVE_ERRMSG(ERR_MSG_LBRAC_AFTER_FUNC_NAME)
    }

    return U"Something weird error";
}
