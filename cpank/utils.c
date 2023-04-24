/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#include "include/utils.h"

#include <locale.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>
#include <wchar.h>

#include "include/common.h"
#include "include/helper/os.h"
#include "include/value.h"

#if defined(PANK_OS_UNIX) || defined(PANK_OS_LINUX) || \
    defined(PANK_OS_ANDROID) || defined(PANK_OS_MACOS)
 #include <unistd.h>
#else
 #include <direct.h>
#endif

bool dump_instruction(Instruction *ins, char *filename) {
    FILE *outfile = fopen(filename, "wb");
    if (outfile == NULL) {
        return false;
    }
    size_t len = (size_t)ins->len;
    size_t br = fwrite(ins, sizeof(uint8_t), len, outfile);
    if (br != len) {
        fclose(outfile);
        return false;
    }

    fclose(outfile);
    return true;
}

char *get_cur_dir(void) {
    char *p;

#if !defined (PANK_OS_WIN)
    p = getcwd(NULL, 0);
#else
    p = _getcwd(NULL, 0);
#endif
    if (p == NULL) {
        return c_to_c(U"", 0);
    }
    return p;
}

#if defined(PANK_OS_WIN)
 #include <io.h>
 #define F_OK   0
 #define access _access
#else
 #include <unistd.h>
#endif

uint8_t get_os_code(void) {
#if defined(PANK_OS_ANDROID)
    return OS_ANDROID_CODE;
#elif defined(PANK_OS_MACOS)
    return OS_MACOS_CODE;
#elif defined(PANK_OS_WIN)
    return OS_WIN_CODE;
#elif defined(PANK_OS_UNIX) || defined(PANK_OS_LINUX)
    return OS_LINUX_CODE;
#else
    return OS_UNKNOWN_CODE;
#endif
}

int copy_c32(char32_t *str, const char32_t *input, int len) {
    memcpy(str, input, sizeof(char32_t) * len);
    return (int)sizeof(char32_t) * len;
}

int strlen32(const char32_t *strarg) {
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
    size_t in_size = strlen32(input) + 1;
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

bool str32cmp(const char32_t *str1, const char32_t *str2) {
    if (strlen32(str1) != strlen32(str2)) {
        return false;
    }

    for (int i = 0; i < strlen32(str1); i++) {
        if (str1[i] != str2[i]) {
            return false;
        }
    }
    return true;
}
bool str32cmp_n(const char32_t *big, const char32_t *small, int len) {
    if (strlen32(small) != len) {
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

// cppcheck-suppress unusedFunction
bool str32cmp_gen(const char32_t *big, const char32_t *small) {
    if (strlen32(small) != strlen32(big)) {
        return false;
    }

    for (int i = 0; i < strlen32(big); i++) {
        // cp_println(L"-->> %#x | %#x\n" , big[i] , small[i]);
        if ((uint32_t)big[i] != (uint16_t)small[i]) {
            return false; /* code */
        }

        /* code */
    }

    return true;
}

bool str32cmp_gen_n(const char32_t *big, const char32_t *small, int len) {
    for (int i = 0; i < len; i++) {
        // cp_println(L"-->> %#x | %#x\n" , big[i] , small[i]);
        if ((uint32_t)big[i] != (uint16_t)small[i]) {
            return false; /* code */
        }

        /* code */
    }

    return true;
}

char32_t *char_to_32(char *input) {
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

// Got help from a stackoverflow answer which I asked myself
// https://stackoverflow.com/a/75982886/7917825
// The commented code is my origin solution which had some
// memory errors;;
char32_t **split32(char32_t *str, const char32_t *delimiter, int *len) {
    *len = 1;
    char32_t delim = delimiter[0];
    char32_t *s = str;
    while (*s != U'\0') {
        if (*s == delim) {
            (*len)++;
        }
        s++;
    }
    char32_t **tokens = (char32_t **)calloc((*len), sizeof(char32_t *));
    if (tokens == NULL) {
        return NULL;
    }
    int start = 0, end = 0, i = 0;
    do {
        if ((str[end] == delim) || (str[end] == U'\0')) {
            tokens[i] =
                (char32_t *)malloc(sizeof(char32_t) * (end - start + 1));
            if (tokens[i] == NULL) {
                return NULL;
            }
            memcpy(tokens[i], &str[start], sizeof(char32_t) * (end - start));
            tokens[i][end - start] = U'\0';
            start = end + 1;
            i++;
        }
    } while (str[end++] != U'\0');
    // char32_t *p = str;
    // int i = 0;
    /*while (*p) {
        int tok_len = 0;
        while (p[tok_len] && p[tok_len] != delim) {
            tok_len++;
        }
        tokens[i] = (char32_t *)malloc(sizeof(char32_t) * (tok_len + 1));
        if (tokens[i] == NULL) {
            return NULL;
        }
        memcpy(tokens[i], p, tok_len * sizeof(char32_t));
        tokens[i][tok_len] = '\0';
        p += tok_len;
        p += (*p != '\0') ? 1 : 0;

        i++;
    }

    if (tokens[*len - 1] == NULL) {
        tokens[i] = malloc(1 * sizeof(char32_t));
        tokens[i][0] = U'\0';
    }*/
    return tokens;
}

bool is_int(double val) {
    if (ceil(val) != val) {
        return false;
    }
    return true;
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

// cppcheck-suppress unusedFunction
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

// cppcheck-suppress unusedFunction
void cp_err_color_println(wchar_t colorcode, const wchar_t *format, ...) {
    va_list args;
    va_start(args, format);
    _print_ansi_color(colorcode, stderr);
    vfwprintf(stderr, format, args);
    color_reset();
    va_end(args);

    fputwc('\n', stdout);
}
