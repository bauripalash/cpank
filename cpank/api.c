#include <stdbool.h>
#include <stdlib.h>
#include <uchar.h>

#include "include/common.h"
#include "include/utils.h"
#include "include/vm.h"

#define RUNTIME_ERR U"RUNTIME ERROR OCCURED"
#define COMPILE_ERR U"COMPILER ERROR OCCURED"

// returned result must be freed.
// each call to `run_code` creates and frees the VM.
// which makes it very unoptimized
// we need to make it little more optimized
char* run_code(char* source) {
    PankVm* vm = boot_vm(true);
    char32_t* src = char_to_32(source);
    IResult result = interpret(vm, src);
    switch (result) {
        case INTRP_RUNTIME_ERR: {
            char32_t* raw_res = get_trimmed(&vm->buffer);
            char* buffer_result = c32_to_char(raw_res, strlen32(raw_res));
            free(raw_res);
            free_vm(vm);

            free(src);
            return buffer_result;
        }
        case INTRP_COMPILE_ERR: {
            char32_t* raw_res = get_trimmed(&vm->buffer);
            char* buffer_result = c32_to_char(raw_res, strlen32(raw_res));
            free(raw_res);
            free_vm(vm);

            free(src);
            return buffer_result;
        }
        case INTRP_OK: {
            char32_t* rawres = get_trimmed(&vm->buffer);
            char* buffer_result = c_to_c(rawres, strlen32(rawres));
            free(rawres);
            free_vm(vm);

            free(src);
            return buffer_result;
        }
    }

    return NULL;
}
