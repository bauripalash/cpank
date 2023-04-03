#include <locale.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>
#include <wchar.h>

#include "include/compiler.h"
#include "include/lexer.h"
#include "include/pank.h"
#include "include/runfile.h"
#include "include/utils.h"
#include "include/vm.h"

const wchar_t help_msg[] =
    L"Pankti programming language; an interpreted bengali programming "
    "language\n"
    "Made by, Palash Bauri\n"
    "Licensed under GPLv3\n"
    "usage: cpank [option] <File to Run>\n\n"
    "Options and Arguments\n"
    "-v, --version : show version info and exit\n"
    "-h, --help : show help and exit\n\n"
    "cpank v0.1.0\n";

const wchar_t version[] = L"v0.1.0";

int strlenx16(const char16_t* strarg) {
    if (!strarg) return -1;  // strarg is NULL pointer
    const char16_t* str = strarg;
    for (; *str; ++str)
        ;  // empty body
    return str - strarg;
}
int main(int argc, char** argv) {
    setlocale(LC_CTYPE, "");
    // mbstate_t mb;
    // memset(&mb, 0, sizeof(mb));
    // Lexer lexer;
    // char16_t* s = u"show 1+2;";
    // wprintf(L"%s | LEN -> %d\n" , s , strlenx16(s));

    // boot_lexer(&lexer, s);
    /*Token tok = get_tok(&lexer);
    while (tok.type != T_EOF) {
      wprintf(L"%s \n", toktype_to_string(tok.type));
     // free(x);

      tok = get_tok(&lexer);
    }*/
    // PankVm* vm = boot_vm();
    // compile(vm, s);

    if (argc != 2) {
        cp_println(help_msg);
        return 0;
    } else if (argc == 2) {
        char* ag = argv[1];
        if (strcmp(ag, "-h") == 0 || strcmp(ag, "--help") == 0) {
            cp_println(help_msg);
        } else if (strcmp(ag, "-v") == 0 || strcmp(ag, "--version") == 0) {
            cp_println(L"cpank %ls", version);
        } else {
            if (does_file_exist(ag)) {
                int result = run_file(ag);
                return result;
            } else {
                cp_println(L"Error! file '%s' does not exist!", ag);
                return 1;
            }
        }
    }
    // cp_color_println('g', L"start program");
    // cp_color_println('g', L"end program");
}
