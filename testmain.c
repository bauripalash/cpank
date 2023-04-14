/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */
//go:build tmain
// +build tmain
#include <stdbool.h>
#include <uchar.h>
#include <wchar.h>

#include "cpank/include/obj.h"
#include "cpank/include/pank.h"
#include "cpank/include/utils.h"
#include "cpank/include/value.h"
#include "cpank/include/vm.h"

typedef struct {
    char *name;
    char32_t *src;
    Value expected;
} T;

T tc(char *n, char32_t *s, Value ex) {
    return (T){.name = n, .src = s, .expected = ex};
}

Value fetch_last_pop(char32_t *src) {
    PankVm *vm = boot_vm(false);
    interpret(vm, src);
    Value result = get_last_pop(vm);
    free_vm(vm);
    return result;
}

bool test_vm(const char *test_name, char32_t *src, Value expected) {
    Value got = fetch_last_pop(src);

    // print_val_type(vm.stack_top->type);
    // print_val(*got);
    // wprintf(L"---> %ls" ,get_obj_type_as_string(get_as_obj(*got)->type));

    bool result = is_equal(got, expected);

    if (!result) {
        cp_color_println('r', L"[X] failed test: %s", test_name);
        cp_print(L"---------\nExpected : ");
        print_val(expected);
        cp_print(L"\nGot : ");
        print_val(got);

        // wprintf(L"-> %d" , got.type );
        cp_print(L"\n---------\n");
        return false;
    } else {
        cp_color_println('g', L"[OK] passed test: %s", test_name);
        return true;
    }
}

bool test_basic_arithmetic(void) {
    bool fail = false;
    T testcases[] = {
        // Label    // Code     // Expected Output
        tc("1+2", U"show 1+2;", make_num(3)),
        tc("3+5", U"show 3+5;", make_num(8)),
        tc("-1 + 1 - 1", U"show -1 + 1 - 1;", make_num(-1)),
        tc("100+200", U"show 100+200;", make_num(300)),
        tc("3.14+1.24", U"show 3.14+1.24;", make_num(4.38)),
        tc("1>2", U"show 1>2;", make_bool(false)),
        tc("1<2", U"show 1<2;", make_bool(true)),
        tc("2<=2", U"show 2<=2;", make_bool(true)),
        tc("2>=2", U"show 2>=2;", make_bool(true)),

    };
    int tc_len = sizeof(testcases) / sizeof(T);
    for (int i = 0; i < tc_len; i++) {
        bool failed = !test_vm(testcases[i].name, testcases[i].src,
                               testcases[i].expected);
        if (!fail) {
            fail = failed;
        }
    }
    return fail;
}

int main(void) {
    bool fail = false;

    fail = test_basic_arithmetic();

    // free_vm();
    return fail ? 1 : 0;
}
