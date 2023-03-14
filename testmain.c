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
  // free_vm();
  return result;
}

bool test_vm(const char *test_name, wchar_t *src, Value expected) {
  Value got = fetch_last_pop(src);

  // print_val_type(vm.stack_top->type);
  // print_val(*got);
  // wprintf(L"---> %ls" ,get_obj_type_as_string(get_as_obj(*got)->type));

  bool result = is_equal(got, expected);

  if (!result) {
    wprintf(L"[X] failed test: %s\n", test_name);
    wprintf(L"---------\nExpected : ");
    print_val(expected);
    wprintf(L"\nGot : ");
    print_val(got);

    // wprintf(L"-> %d" , got.type );
    wprintf(L"\n---------\n");
    return false;
  } else {
    wprintf(L"[OK] passed test: %s\n", test_name);
    return true;
  }
}

int main() {
  bool fail = false;
  T testcases[] = {
      tc("1+2", L"show 1+2;", make_num(3)),
      tc("3+5", L"show 3+5;", make_num(8)),
      tc("-1 + 1 - 1", L"show -1 + 1 - 1;", make_num(-1)),
      tc("100+200", L"show 100+200;", make_num(300)),
      tc("3.14+1.24", L"show 3.14+1.24;", make_num(4.38)),

  };

  int tc_len = sizeof(testcases) / sizeof(T);
  for (int i = 0; i < tc_len; i++) {
    bool failed =
        !test_vm(testcases[i].name, testcases[i].src, testcases[i].expected);
    if (!fail) {
      fail = failed;
    }
  }

  free_vm();
  return fail ? 1 : 0;
}
