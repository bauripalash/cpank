#include "include/runfile.h"

#include <locale.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "include/common.h"
#include "include/openfile.h"
#include "include/pank.h"
#include "include/utils.h"
#include "include/vm.h"

int run_file(const char *filepath) {
    Srcfile raw = read_file(filepath);
    if (raw.errcode != 0) {
        cp_println(L"Failed to read file '%s' -> error code %d", filepath,
                   raw.errcode);
        exit(1);
    }
    //  wprintf(L"SOURCE -> %s", raw.source);
    int errcode = 0;
    wchar_t *src = (wchar_t *)malloc(sizeof(wchar_t) * raw.size);
    //  wprintf(L"sizes: wchar_t -> %d | char -> %d\n" , sizeof(wchar_t) ,
    setlocale(LC_CTYPE, "");
    mbstowcs(src, raw.source, raw.size + 1);

    PankVm *vm = boot_vm();

    // wprintf(L"SOURCE_W -> %ls", src);
    IResult res = interpret(vm, src);

    switch (res) {
        case INTRP_RUNTIME_ERR:
            wprintf(L"Runtime error occured!\n");
            errcode = ERC_RUNTIME;
            break;
            ;
        case INTRP_COMPILE_ERR:
            wprintf(L"Compiler error occured!\n");
            errcode = ERC_COMPTIME;
            break;
            ;
        // case INTRP_OK:
        //   wprintf(L"OK\n");
        //   break;
        default: {
        }
    }

    // free(src);
    free_vm(vm);

    free(src);

    free(raw.source);
    return errcode;
}
