#include "include/runfile.h"

#include <locale.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>
#include <wchar.h>

#include "include/common.h"
#include "include/openfile.h"
#include "include/pank.h"
#include "include/utils.h"
#include "include/vm.h"

int run_file(const char *filepath) {
    setlocale(LC_CTYPE, "");
    // char * path = c_to_c(filepath, strlen16(filepath));
    // char32_t * path = chto16(filepath);
    // WSrcfile raw = wread_file(path);
    Srcfile raw = read_file(filepath);

    if (raw.errcode != 0) {
        cp_println(L"Failed to read file '%s' -> error code %d", filepath,
                   raw.errcode);
        exit(1);
    }
    int errcode = 0;
    char32_t *src = chto16(raw.source);

    PankVm *vm = boot_vm();

    IResult res = interpret(vm, src);

    switch (res) {
        case INTRP_RUNTIME_ERR:
            cp_println(L"\nRuntime error occured!");
            errcode = ERC_RUNTIME;
            break;
            ;
        case INTRP_COMPILE_ERR:
            cp_println(L"\nCompiler error occured!");
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
