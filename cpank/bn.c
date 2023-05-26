/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#include "include/bn.h"

#include <stdbool.h>
#include <string.h>
#include <uchar.h>
#include <wchar.h>

// Check if `c` is in range of Bengali Numbers.
// Unicode Bengali `0` is stored in `BN_NUM_ZERO`.
// Unicode Bengali '9' is stored in `BN_NUM_NINE`.
bool is_bn_num(char32_t c) { return c >= BN_NUM_ZERO && c <= BN_NUM_NINE; }

// Check if `c` is in Unicode range of Bengali Characters
// It doesn't check of Reserved code points
bool is_bn_char(char32_t c) { return c >= BN_RANGE_START && c <= BN_RANGE_END; }

// Convert each bengali number to it's English counterpart.
// If `str` is not NULL, it will always return `true`
// Else returns `false`
bool conv_bn_to_en_num(char32_t *str, int len) {
    if (str == NULL) {
        return false;
    }
    for (int i = 0; i < len; i++) {
        if (str[i] == BN_NUM_ONE) {
            str[i] = U'1';
        } else if (str[i] == BN_NUM_TWO) {
            str[i] = U'2';
        } else if (str[i] == BN_NUM_THREE) {
            str[i] = U'3';
        } else if (str[i] == BN_NUM_FOUR) {
            str[i] = U'4';
        } else if (str[i] == BN_NUM_FIVE) {
            str[i] = U'5';
        } else if (str[i] == BN_NUM_SIX) {
            str[i] = U'6';
        } else if (str[i] == BN_NUM_SEVEN) {
            str[i] = U'7';
        } else if (str[i] == BN_NUM_EIGHT) {
            str[i] = U'8';
        } else if (str[i] == BN_NUM_NINE) {
            str[i] = U'9';
        } else if (str[i] == BN_NUM_ZERO) {
            str[i] = U'0';
        }
    }

    return true;
}
