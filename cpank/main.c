#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "include/runfile.h"
#include "include/utils.h"

const wchar_t help_msg[] =
    L"Pankti programming language; an interpreted bengali programming "
    L"language\n"
    "Made by, Palash Bauri\n"

    "Licensed under GPLv3\n"
    "usage: cpank [option] <File to Run>\n\n"
    "Options and Arguments\n"
    "-v, --version : show version info and exit\n"
    "-h, --help : show help and exit\n\n"
    "cpank v0.1.0\n";

const wchar_t version[] = L"v0.1.0";

int main(int argc, char** argv) {
    setlocale(LC_CTYPE, "");
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
