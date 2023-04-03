#include "include/utils.h"

#include <bits/types/mbstate_t.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>
#include <wchar.h>

#include "include/value.h"

#ifdef WIN32
#include <io.h>
#define F_OK 0
#define access _access
#else
#include <unistd.h>
#endif

int copy_c16(char16_t *str, const char16_t *input, int len) {
    memcpy(str, input, sizeof(char16_t) * len);
    return (int)sizeof(char16_t) * len;
}

int strlen16(const char16_t *strarg) {
    if (!strarg) return -1;  // strarg is NULL pointer
    const char16_t *str = strarg;
    for (; *str; ++str)
        ;  // empty body
    return str - strarg;
}

char *c_to_c(const char16_t *input, int len) {
    mbstate_t state;
    memset(&state, 0, sizeof(mbstate_t));

    size_t insz = sizeof(char16_t) * (len);
    char *o = (char *)malloc(MB_CUR_MAX * insz);
    char *p = o;
    int rc = 0;

    for (int i = 0; i < insz; i++) {
        rc = c16rtomb(p, input[i], &state);
        p += rc;
    }
    return o;
}

bool str16cmp(const char16_t *str1, const char16_t *str2) {
    if (strlen16(str1) != strlen16(str2)) {
        return false;
    }

    for (int i = 0; i < strlen16(str1); i++) {
        if (str1[i] != str2[i]) {
            return false;
        }
    }
    return true;
}

char16_t *chto16(char *input) {
    mbstate_t state;
    memset(&state, 0, sizeof(mbstate_t));
    size_t insz = sizeof(char) * (strlen(input) + 1);
    char16_t *output = (char16_t *)malloc(insz);
    char *p_in = input, *end = input + insz;

    char16_t *p_out = output;

    size_t rc;
    while ((rc = mbrtoc16(p_out, p_in, end - p_in, &state))) {
        if (rc == (size_t)-1) {
            break;
        } else if (rc == (size_t)-2) {
            break;
        } else if (rc == (size_t)-3) {
            p_out += 1;
        } else {
            p_in += rc;
            p_out += 1;
        }
    }

    return output;
}

bool does_file_exist(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        return false;
        // fwprintf(stderr, L"Failed to open file %s\n", path);
        // cp_color_println('r' , L"Failed to open file %s" , path);
        // result.errcode = ERC_FAIL_TO_OPEN;

        // return result;
    } else {
        fclose(file);
        return true;
    }
}

static char nocolor_env[] = "CPANK_NO_COLOR";

static bool no_color(void) {
    char *has_nocolor = getenv(nocolor_env);
    if (has_nocolor != NULL) {
        if (strcmp(has_nocolor, "1") == 0 || strcmp(has_nocolor, "true") == 0) {
            return true;
        }
    }
    return false;
}

void _print_ansi_color(wchar_t colorcode, FILE *stream) {
    if (no_color()) {
        return;
    }
    switch (colorcode) {
        case 'r':
            fwprintf(stream, ANSI_COLOR_RED);
            break;
        case 'b':
            fwprintf(stream, ANSI_COLOR_BLUE);
            break;
        case 'g':
            fwprintf(stream, ANSI_COLOR_GREEN);
            break;
        case 'c':
            fwprintf(stream, ANSI_COLOR_CYAN);
            break;
        case 'w':
            fwprintf(stream, ANSI_COLOR_WHITE);
            break;
        case 'p':
            fwprintf(stream, ANSI_COLOR_PURPLE);
            break;
        case 'y':
            fwprintf(stream, ANSI_COLOR_YELLOW);
            break;
        case 'B':
            fwprintf(stream, ANSI_COLOR_BLACK);
            break;
        default: {
            break;
        }
    }
    return;
}

void color_reset(void) { wprintf(ANSI_COLOR_RESET); }

void cp_print(const wchar_t *format, ...) {
    va_list args;
    va_start(args, format);
    vfwprintf(stdout, format, args);
    va_end(args);
}

void cp_println(const wchar_t *format, ...) {
    va_list args;
    va_start(args, format);
    vfwprintf(stdout, format, args);
    va_end(args);
    fputwc('\n', stdout);
}

void cp_err_print(const wchar_t *format, ...) {
    va_list args;
    va_start(args, format);
    vfwprintf(stderr, format, args);
    va_end(args);
}

void cp_err_println(const wchar_t *format, ...) {
    va_list args;
    va_start(args, format);
    vfwprintf(stderr, format, args);
    va_end(args);
    fputwc('\n', stderr);
}

void cp_color_print(wchar_t colorcode, const wchar_t *format, ...) {
    va_list args;
    va_start(args, format);
    _print_ansi_color(colorcode, stdout);
    vfwprintf(stdout, format, args);
    color_reset();
    va_end(args);
}

void cp_err_color_print(wchar_t colorcode, const wchar_t *format, ...) {
    va_list args;
    va_start(args, format);
    _print_ansi_color(colorcode, stderr);
    vfwprintf(stderr, format, args);
    color_reset();
    va_end(args);
}

void cp_color_println(wchar_t colorcode, const wchar_t *format, ...) {
    va_list args;
    va_start(args, format);
    _print_ansi_color(colorcode, stdout);
    vfwprintf(stdout, format, args);
    color_reset();
    va_end(args);

    fputwc('\n', stdout);
}

void cp_err_color_println(wchar_t colorcode, const wchar_t *format, ...) {
    va_list args;
    va_start(args, format);
    _print_ansi_color(colorcode, stderr);
    vfwprintf(stderr, format, args);
    color_reset();
    va_end(args);

    fputwc('\n', stdout);
}
