/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#ifndef cpank_bn_h
#define cpank_bn_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <uchar.h>
#include <wchar.h>

// Bengali Numbers
#define BN_NUM_ZERO  U'\u09E6'
#define BN_NUM_ONE   U'\u09E7'
#define BN_NUM_TWO   U'\u09E8'
#define BN_NUM_THREE U'\u09E9'
#define BN_NUM_FOUR  U'\u09EA'
#define BN_NUM_FIVE  U'\u09EB'
#define BN_NUM_SIX   U'\u09EC'
#define BN_NUM_SEVEN U'\u09ED'
#define BN_NUM_EIGHT U'\u09EE'
#define BN_NUM_NINE  U'\u09EF'

// Bengali unicode range start
// including numbers
#define BN_RANGE_START 0x0980
// end of bengali unicode range
#define BN_RANGE_END    0x09FE

#define MOD_NAME_BN     U"মডিউল"
#define STR_NAME_BN     U"স্ট্রিং"
#define ERR_NAME_BN     U"গোলমাল"
#define FUNC_NAME_BN    U"কাজ"
#define HMAP_NAME_BN    U"ম্যাপ"
#define CLOSURE_NAME_BN U"ক্লোসার"
#define ARRAY_NAME_BN   U"তালিকা"
#define NATIVE_NAME_BN  U"নেটিভ"
#define UPVAL_NAME_BN   U"আপভ্যালু"
#define NUMBER_NAME_BN  U"সংখ্যা"
#define NIL_NAME_BN     U"নিল"
#define BOOL_NAME_BN    U"বুল"

// Check if `c` is in range of Bengali Numbers.
// Unicode Bengali `0` is stored in `BN_NUM_ZERO`.
// Unicode Bengali '9' is stored in `BN_NUM_NINE`.
bool is_bn_num(char32_t c);

// Check if `c` is in Unicode range of Bengali Characters
// It doesn't check of Reserved code points
bool is_bn_char(char32_t c);

// Convert each bengali number to it's English counterpart.
// If `str` is not NULL, it will always return `true`
// Else returns `false`
bool conv_bn_to_en_num(char32_t* str, int len);

#ifdef __cplusplus
}
#endif
#endif
