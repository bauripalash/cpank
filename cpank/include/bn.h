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
#define BN_RANGE_END 0x09FE

// check if `c` is a bengali numbers
// 0 to 9
bool is_bn_num(char32_t c);

// check if `c` is a bengali char
// including numbers
bool is_bn_char(char32_t c);

bool conv_bn_to_en_num(char32_t* str, int len);
