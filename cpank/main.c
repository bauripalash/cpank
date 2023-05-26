/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */
// go:build tmain
//  +build tmain
#if !defined(APILIB)
 #include <locale.h>
 #include <stddef.h>
 #include <stdlib.h>
 #include <string.h>
 #include <wchar.h>

 #include "include/common.h"
 #include "include/helper/comp.h"
 #include "include/helper/os.h"
 #include "include/runfile.h"
 #include "include/utils.h"

 #if defined(IS_WIN) || defined(_WIN32)
  #include <fcntl.h>
  #include <io.h>
  #include <windows.h>

  #define CP_UTF8 65001

 #endif

const wchar_t help_msg[] =
    L"Pankti programming language; an interpreted bengali programming "
    "language\n"
    "Made by, Palash Bauri\n"
    "Licensed under GPLv3\n"
    "usage: cpank [option] <File to Run>\n\n"
    "Options and Arguments\n"
    "-v, --version : show version info and exit\n"
    "-h, --help : show help and exit\n\n"
    "cpank v0.3.0-beta\n";

const wchar_t version[] = L"v0.1.0";

int main(int argc, char** argv) {
    setlocale(LC_CTYPE, "");

 #if defined(PANK_OS_WIN) && defined(PANK_COMP_MSVC)
    SetConsoleOutputCP(CP_UTF8);
    _setmode(_fileno(stdout), _O_U16TEXT);
 #endif  // _WIN32

    if (argc == 2) {
        char* ag = argv[1];
        if (strcmp(ag, "-h") == 0 || strcmp(ag, "--help") == 0) {
            cp_println(help_msg);
        } else if (strcmp(ag, "-v") == 0 || strcmp(ag, "--version") == 0) {
            cp_println(L"cpank %ls", version);
        } else {
            if (does_file_exist(ag)) {
                int result = run_file(ag);
 #ifdef DEBUG
                cp_println(L"");
 #endif
                return result;
            } else {
 #ifdef PANK_OS_WIN

                cp_println(L"Error! file '%S' does not exist!", ag);
 #else
                cp_println(L"Error! file '%s' does not exist!", ag);
 #endif
                return 1;
            }
        }
    } else {
        cp_println(help_msg);

        return 0;
    }
}
#endif
