#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>

#include "include/errmsgs.h"

char32_t ERR_MSG_KW[] = U"Error";
char32_t ERR_MSG_KW_BN[] = U"\u0997\u09cb\u09b2\u09af\u09cb\u0997";

char32_t ERR_MSG_AT_END[] = U"at end";
char32_t ERR_MSG_AT_END_BN[] =
    U"\u098f\u0995\u09c7\u09ac\u09be\u09b0\u09c7 \u09b6\u09c7\u09b7\u09c7";

char32_t ERR_MSG_HERE[] = U"here";
char32_t ERR_MSG_HERE_BN[] = U"\u098f\u0996\u09be\u09a8\u09c7";

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

char32_t ERR_MSG_ARGLIST_TOO_MUCH_ARG[] =
    U"Too many arguments for me to handle";
char32_t ERR_MSG_ARGLIST_TOO_MUCH_ARG_BN[] =
    U"\u098f\u0995\u09c7\u09ac\u09be\u09b0\u09c7 \u09b6\u09c7\u09b7\u09c7";

char32_t ERR_MSG_RBRAC_AFTER_GROUP[] = U"Expected ')' after group expression";
char32_t ERR_MSG_RBRAC_AFTER_GROUP_BN[] =
    U"\u0997\u09cd\u09b0\u09c1\u09aa "
    U"\u098f\u0995\u09cd\u09b8\u09aa\u09cd\u09b0\u09c7\u09b6\u09a8\u09c7\u09b0 "
    U"\u09aa\u09b0 \u098f\u0995\u099f\u09bf ')' \u09aa\u09be\u0993\u09df\u09be "
    U"\u0989\u099a\u09bf\u09a4 \u099b\u09bf\u09b2";

char32_t ERR_MSG_TOO_MANY_ARR_ITEM[] =
    U"Array can't have more than 255 items (yet)";
char32_t ERR_MSG_TOO_MANY_ARR_ITEM_BN[] =
    U"\u09ac\u09b0\u09cd\u09a4\u09ae\u09be\u09a8\u09c7 "
    U"\u098f\u0995\u099f\u09bf "
    U"\u09a4\u09be\u09b2\u09bf\u0995\u09be\u09a4\u09c7 "
    U"\u09e8\u09eb\u09eb\u099f\u09bf\u09b0 \u09ac\u09c7\u09b6\u09bf "
    U"\u09b0\u09be\u09b6\u09bf \u09b0\u09be\u0996\u09be "
    U"\u09b8\u09ae\u09cd\u09ad\u09ac \u09a8\u09df (\u09aa\u09b0\u09c7 "
    U"\u09b8\u09ae\u09cd\u09ad\u09ac \u09b9\u09ac\u09c7)";

char32_t ERR_MSG_RBRAC_INDEX[] = U"Expected ']' after index expression";
char32_t ERR_MSG_RBRAC_INDEX_BN[] =
    U"\u09a4\u09be\u09b2\u09bf\u0995\u09be\u09b0 "
    U"\u09b8\u09c2\u099a\u0995\u09c0\u09df "
    U"\u098f\u0995\u09cd\u09b8\u09aa\u09cd\u09b0\u09c7\u09b6\u09a8\u09c7\u09b0 "
    U"\u09aa\u09b0 \u098f\u0995\u099f\u09bf ']' \u09aa\u09be\u0993\u09df\u09be "
    U"\u0989\u099a\u09bf\u09a4 \u099b\u09bf\u09b2";

char32_t ERR_MSG_TOO_MANY_LOCALVARS[] =
    U"Too many local variables have been created, more than I can handle";
char32_t ERR_MSG_TOO_MANY_LOCALVARS_BN[] =
    U"\u0985\u09a8\u09c7\u0995 \u09ac\u09c7\u09b6\u09bf "
    U"\u09b8\u09cd\u09a5\u09be\u09a8\u09c0\u09df "
    U"\u099a\u09b2\u09b0\u09be\u09b6\u09bf \u09a4\u09c8\u09b0\u09bf "
    U"\u09b9\u09df\u09c7\u099b\u09c7, \u09af\u09be \u0986\u09ae\u09be\u09b0 "
    U"\u0995\u09cd\u09b7\u09ae\u09a4\u09be\u09b0 "
    U"\u09ac\u09be\u0987\u09b0\u09c7";

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
        case EMSG_HERE:
            GIVE_ERRMSG(ERR_MSG_HERE)
        case EMSG_KW:
            GIVE_ERRMSG(ERR_MSG_KW)
        case EMSG_AT_END:
            GIVE_ERRMSG(ERR_MSG_AT_END)
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
        case EMSG_ARGLIST_TOO_MUCH_ARG:
            GIVE_ERRMSG(ERR_MSG_ARGLIST_TOO_MUCH_ARG)
        case EMSG_RBRAC_AFTER_GROUP:
            GIVE_ERRMSG(ERR_MSG_RBRAC_AFTER_GROUP)
        case EMSG_TOO_MANY_ARR_ITEM:
            GIVE_ERRMSG(ERR_MSG_TOO_MANY_ARR_ITEM)
        case EMSG_RBRAC_INDEX:
            GIVE_ERRMSG(ERR_MSG_RBRAC_INDEX)
        case EMSG_TOO_MANY_LOCALVARS:
            GIVE_ERRMSG(ERR_MSG_TOO_MANY_LOCALVARS)
    }

    return U"Something weird error";
}
