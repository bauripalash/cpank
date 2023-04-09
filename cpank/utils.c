#include "include/utils.h"

#include <locale.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>
#include <wchar.h>

#include "include/common.h"
#include "include/value.h"

#ifdef IS_WIN
 #include <io.h>
 #define F_OK   0
 #define access _access
#else
 #include <unistd.h>
#endif

int copy_c16(char32_t *str, const char32_t *input, int len) {
    memcpy(str, input, sizeof(char16_t) * len);
    return (int)sizeof(char16_t) * len;
}

int strlen16(const char32_t *strarg) {
    if (!strarg) return -1;  // strarg is NULL pointer
    int len = 0;
    while (strarg[len] != U'\0') {
        len++;
    }
    return len;
}

char *c_to_c(const char32_t *input, int len) {
    mbstate_t state = {0};
    setlocale(LC_ALL, "bn_IN.utf8");
    size_t in_size = strlen16(input) + 1;
    char *output = (char *)malloc(MB_CUR_MAX * in_size);
    if (output == NULL) return "";
    memset(output, 0, in_size);
    char *p = output;
    for (size_t n = 0; n < in_size; n++) {
        size_t rc = c32rtomb(p, input[n], &state);
        if (rc == (size_t)-1) break;
        p += rc;
    }

    // output = (char *)realloc(output, strlen(output));
    // size_t os = p - output;
    // cp_println(L"sz ->%S\n", output);
    return output;
}

bool str16cmp(const char32_t *str1, const char32_t *str2) {
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
bool str16cmp_n(const char32_t *big, const char32_t *small, int len) {
    if (strlen16(small) != len) {
        return false;
    }

    for (int i = 0; i < len; i++) {
        if ((uint32_t)big[i] != (uint16_t)small[i]) {
            return false; /* code */
        }

        /* code */
    }

    return true;
}

bool str16cmp_gen(const char32_t *big, const char32_t *small) {
    if (strlen16(small) != strlen16(big)) {
        return false;
    }

    for (int i = 0; i < strlen16(big); i++) {
        // cp_println(L"-->> %#x | %#x\n" , big[i] , small[i]);
        if ((uint32_t)big[i] != (uint16_t)small[i]) {
            return false; /* code */
        }

        /* code */
    }

    return true;
}

bool str16cmp_gen_n(const char32_t *big, const char32_t *small, int len) {
    for (int i = 0; i < len; i++) {
        // cp_println(L"-->> %#x | %#x\n" , big[i] , small[i]);
        if ((uint32_t)big[i] != (uint16_t)small[i]) {
            return false; /* code */
        }

        /* code */
    }

    return true;
}

char32_t *chto16(char *input) {
    mbstate_t state = {0};
    size_t osz = sizeof(char32_t) * (strlen(input) + 1);
    setlocale(LC_ALL, "bn_IN.utf8");
    char32_t *output = (char32_t *)malloc(osz);
    if (output == NULL) {
        return NULL;
    }
    memset(output, 0, osz);
    size_t x = 0;
    size_t y = 0;

    while (input[x] != '\0') {
        char32_t c3;
        size_t rs = mbrtoc32(&c3, &input[x], MB_CUR_MAX, &state);
        if (rs == (size_t)-1 || rs == (size_t)-2 || rs == (size_t)-3) {
            break;
        } else {
            output[y++] = c3;
            x += rs;
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
