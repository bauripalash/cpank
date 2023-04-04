#include <assert.h>
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
    setlocale(LC_CTYPE, "en_US.utf8");
    /*char * s = u8"hello world";
    //const size_t insz = sizeof s / sizeof *s;
    const size_t insz = sizeof(char) * (strlen(s) + 1);
    mbstate_t mb = {0};
    char * pin = s;
    char * end = s + insz;

    char32_t * out = (char32_t*)malloc(sizeof(char32_t) * (strlen(s) + 1));
    char32_t * pot = out;

    size_t rc ;//= mbrtoc32(pot, pin, end - pin , &mb);
    size_t i = 0;
    size_t j = 0;
    while (s[i] != '\0'){
        char32_t c32;
        size_t result = mbrtoc32(&c32, &s[i], MB_CUR_MAX, &mb);
        if (result == (size_t)-1 || result == (size_t)-2) {
        break;
        }else{
            out[j++] = c32;
            i += result;
        }
    }

    while ((rc = mbrtoc32(pot, pin, MB_CUR_MAX , &mb))){
        assert(rc != (size_t)-1);
        if (rc == (size_t)-1) break; // invalid input
        if (rc == (size_t)-2) break;
        pin += rc;
        ++pot;
    }

    printf("-> %s | %ls\n" , s , out);
    printf("-> %zu | %zu\n" , insz , (size_t)1);
    printf("-> %zu\n" , end - pin);
    free(out);
    //printf("-> %zu -> %s\n-> %s\n" , rc , pot , out);
    //free(out);

    //Lexer lexer;
    //char32_t* s = U"দেখাও ১+2;";
    // wprintf(L"%s | LEN -> %d\n" , s , strlenx16(s));

    //boot_lexer(&lexer, s);
    //Token tok = get_tok(&lexer);
    //while (tok.type != T_EOF) {
    //  wprintf(L"%s \n", toktype_to_string(tok.type));
     // free(x);

    //  tok = get_tok(&lexer);
    //}
    // PankVm* vm = boot_vm();
    // compile(vm, s);
*/
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
