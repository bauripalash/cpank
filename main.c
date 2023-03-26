#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "include/runfile.h"
#include "include/utils.h"

const wchar_t help_msg[] =
    L"usage: cpank [option] <File to Run>\n"
    "Options and Arguments\n"
    "-v, --version : show version info and exit\n"
    "-h, --help : show help and exit\n\n"
    "Pankti Programming Language\n"
    "cpank v0.1.0\n\n"
    "(C) Palash Bauri\n"
    "Licensed under GPLv3\n";

const wchar_t version[] = L"v0.1.0";

int main(int argc, char** argv) {
    setlocale(LC_CTYPE, "");
    if (argc != 2) {
        cp_color_println('p', help_msg);
        return 0;
    } else if (argc == 2) {
        char* ag = argv[1];
        if (strcmp(ag, "-h") == 0 || strcmp(ag, "--help") == 0) {
            cp_color_println('p', help_msg);
        } else if (strcmp(ag, "-v") == 0 || strcmp(ag, "--version") == 0) {
            cp_color_println('g', L"cpank %ls", version);
        } else {
            int result = run_file(ag);
            return result;
        }
    }
    // cp_color_println('g', L"start program");
    // cp_color_println('g', L"end program");
}
