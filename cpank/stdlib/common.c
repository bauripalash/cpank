/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#include "../include/common.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <uchar.h>

#include "../include/stdlib.h"
#include "../include/utils.h"
#include "../include/value.h"

// Copied line by line from:
// https://stackoverflow.com/a/314422/7917825
char* getline_char(void) {
    char *line = malloc(100), *linep = line;
    size_t lenmax = 100, len = lenmax;
    int c;

    if (line == NULL) return NULL;

    for (;;) {
        c = fgetc(stdin);
        if (c == EOF) break;

        if (--len == 0) {
            len = lenmax;
            char* linen = realloc(linep, lenmax *= 2);

            if (linen == NULL) {
                free(linep);
                return NULL;
            }
            line = linen + (line - linep);
            linep = linen;
        }

        if ((*line++ = c) == '\n') break;
    }
    *line = '\0';
    return linep;
}

Value _common_readline(PankVm* vm, int argc, Value* args) {
    char* raw_line = getline_char();
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
