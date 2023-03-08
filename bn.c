#include "bn.h"
#include <stdbool.h>
#include <string.h>
#include <wchar.h>

bool is_bn_num(wchar_t c) { return c >= BN_NUM_ZERO && c <= BN_NUM_NINE; }

bool is_bn_char(wchar_t c) { return c >= BN_RANGE_START && c <= BN_RANGE_END; }

void convert_bn_num_to_en(wchar_t *input, int len) {
  for (int i = 0; i < len; i++) {
    // wprintf(L"NUM_CHAR-> %lc\n", input[i]);

    switch (input[i]) {
    case BN_NUM_ONE:
      input[i] = '1';
    case BN_NUM_TWO:
      input[i] = '2';
    case BN_NUM_THREE:
      input[i] = '3';
    case BN_NUM_FOUR:
      input[i] = '4';
    case BN_NUM_FIVE:
      input[i] = '5';
    case BN_NUM_SIX:
      input[i] = '6';
    case BN_NUM_SEVEN:
      input[i] = '7';
    case BN_NUM_EIGHT:
      input[i] = '8';
    case BN_NUM_NINE:
      input[i] = '9';
    case BN_NUM_ZERO:
      input[i] = '0';
    }
  }

  // wprintf(L"NOW=> %ls" , input);
}
