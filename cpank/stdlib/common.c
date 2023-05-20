/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#include "../include/common.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>

#include "../include/stdlib.h"
#include "../include/utils.h"
#include "../include/value.h"

char* read_line(void) {
    size_t cap = 128;
    char* line = (char*)malloc(cap);
    if (line == NULL) {
        return NULL;
    }
    int len = 0;

    for (;;) {
        int c = fgetc(stdin);
        if (c == EOF || c == '\n') {
            break;
        }
        line[len++] = (char)c;

        if (len == cap) {
            cap *= 2;
            line = (char*)realloc(line, cap);
            if (line == NULL) {
                free(line);
                return NULL;
            }
        }
    }
    line[len] = '\0';

    return line;
}

Value _common_readline(PankVm* vm, int argc, Value* args) {
    if (get_os_code() == OS_ANDROID_CODE) {
        return make_error(vm, U"readline does not work on android yet!");
    }
    char* raw_line = read_line();

    if (raw_line == NULL) {
        return make_str(vm, U"");
    } else {
        char32_t* ln = char_to_32(raw_line);
        Value result = make_str(vm, ln);
        free(ln);
        free(raw_line);
        return result;
    }
}

void push_stdlib_common(PankVm* vm) {
    SL sls[] = {
        msl(U"readline", _common_readline),
    };

    _push_stdlib(vm, U"common", sls, 1);
}
