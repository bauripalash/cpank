/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#include "include/runfile.h"

#include <locale.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>
#include <wchar.h>

#include "include/common.h"
#include "include/helper/os.h"
#include "include/openfile.h"
#include "include/pank.h"
#include "include/utils.h"
#include "include/vm.h"

int run_file(const char *filepath) {
    setlocale(LC_CTYPE, "");
    Srcfile raw = read_file(filepath);

    if (raw.errcode != 0) {
#if defined(PANK_OS_WIN)
        cp_println(L"Failed to read file '%S' -> error code %d", filepath,
                   raw.errcode);
#else
        cp_println(L"Failed to read file '%s' -> error code %d", filepath,
                   raw.errcode);
#endif
        exit(1);
    }
    int errcode = 0;
    char32_t *src = char_to_32(raw.source);
    

    PankVm *vm = boot_vm(false);

    IResult res = interpret(vm, src);

    switch (res) {
        case INTRP_RUNTIME_ERR:
            if (vm->need_buffer) {
                write_pbuffer(&vm->buffer, "\nRuntime error occured\n");
            } else {
                cp_err_println(L"\nRuntime error occured!");
            }
            errcode = ERC_RUNTIME;
            break;
            ;
        case INTRP_COMPILE_ERR:
            if (vm->need_buffer) {
                write_pbuffer(&vm->buffer, "\nCompiler error occured\n");
            } else {
                cp_err_println(L"\nCompiler error occured!");
            }
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
