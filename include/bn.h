#include <stdbool.h>
#include <wchar.h>

// Bengali Numbers
#define BN_NUM_ZERO 0x09E6
#define BN_NUM_ONE 0x09E7
#define BN_NUM_TWO 0x09E8
#define BN_NUM_THREE 0x09E9
#define BN_NUM_FOUR 0x09EA
#define BN_NUM_FIVE 0x09EB
#define BN_NUM_SIX 0x09EC
#define BN_NUM_SEVEN 0x09ED
#define BN_NUM_EIGHT 0x09EE
#define BN_NUM_NINE 0x09EF


// Bengali unicode range start 
// including numbers 
#define BN_RANGE_START 0x0980
//end of bengali unicode range 
#define BN_RANGE_END 0x09FE

// check if `c` is a bengali numbers 
// 0 to 9
bool is_bn_num(wchar_t c);

// check if `c` is a bengali char 
// including numbers 
bool is_bn_char(wchar_t c);
