#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "include/debug.h"
#include "include/instruction.h"
#include "include/runfile.h"
#include "include/utils.h"
#include "include/vm.h"
int main() {
    setlocale(LC_CTYPE, "");
    cp_color_println('g', L"start program");
    int result = run_file("sample/x.txt");
    cp_color_println('g', L"end program");
    return result;
}
