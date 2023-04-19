#include <stdbool.h>
#include <stdlib.h>
#include <uchar.h>

#include "../cpank/include/utils.h"
#include "../cpank/include/vm.h"

char* comp_run(char* raw_source) {
    char* result = NULL;
    PankVm* vm = boot_vm(true);
    char32_t* source = char_to_32(raw_source);
    IResult vmres = interpret(vm, source);

    switch (vmres) {
        case INTRP_COMPILE_ERR:
        case INTRP_RUNTIME_ERR:
        case INTRP_OK: {
            char32_t* res = get_trimmed(&vm->buffer);
            result = c_to_c(res, strlen32(res));
            free(res);
        }
    }
    free(source);
    return result;
}

int main(int argc, char** argv) { return 0; }
