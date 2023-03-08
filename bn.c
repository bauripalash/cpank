#include "bn.h"
#include <stdbool.h>

#define BN_NUM_ZERO 0x09E6
#define BN_NUM_NINE 0x09EF
#define BN_RANGE_START 0x0980
#define BN_RANGE_END 0x09FE

bool is_bn_num(wchar_t c) { return c >= BN_NUM_ZERO && c <= BN_NUM_NINE; }

bool is_bn_char(wchar_t c) { return c >= BN_RANGE_START && c <= BN_RANGE_END; }
