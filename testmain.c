#include "include/obj.h"
#include "include/value.h"
#include "include/vm.h"
#include <stdbool.h>
#include <wchar.h>

typedef struct {
  char *name;
  wchar_t *src;
  Value expected;
} T;

T tc(char *n, wchar_t *s, Value ex) {
  return (T){.name = n, .src = s, .expected = ex};
}

Value fetch_last_pop(wchar_t *src) {
  boot_vm();
  interpret(src);

  Value result = get_last_pop();
  free_vm();
  return result;
}

void test_vm(const char *test_name, wchar_t *src, Value expected) {
  Value got = fetch_last_pop(src);
  bool result = is_equal(got, expected);

  if (!result) {
    wprintf(L"[X] failed test: %s\n", test_name);
    wprintf(L"---------\nExpected : ");
    print_val(expected);
    wprintf(L"\nGot : ");
    print_val(got);
    wprintf(L"\n---------\n");
  } else {
    wprintf(L"[OK] passed test: %s\n", test_name);
  }
}

int main() {
  T testcases[] = {
      tc("1+2", L"show 1+2;", make_num(3)),
      tc("3+5", L"show 3+5;", make_num(8)),

      tc("100+200", L"show 100+200;", make_num(300)),

  };

  int tc_len = sizeof(testcases) / sizeof(T);
  for (int i = 0; i < tc_len; i++) {
    test_vm(testcases[i].name, testcases[i].src, testcases[i].expected);
  }
}
