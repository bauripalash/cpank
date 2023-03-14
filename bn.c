#include "include/bn.h"
#include <stdbool.h>
#include <string.h>
#include <wchar.h>

bool is_bn_num(wchar_t c) { return c >= BN_NUM_ZERO && c <= BN_NUM_NINE; }

bool is_bn_char(wchar_t c) { return c >= BN_RANGE_START && c <= BN_RANGE_END; }


