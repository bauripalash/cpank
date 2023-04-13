/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#ifndef cpank_bn_h
#define cpank_bn_h
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

// check if `c` is a bengali numbers
// 0 to 9
bool is_bn_num(char32_t c);

// check if `c` is a bengali char
// including numbers
bool is_bn_char(char32_t c);

bool conv_bn_to_en_num(char32_t* str, int len);
#endif
