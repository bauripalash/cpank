#ifndef cpank_utils_h
#define cpank_utils_h

#include <stdbool.h>
#include <uchar.h>
#include <wchar.h>

#define ANSI_COLOR_BLACK  L"\x1b[30m"
#define ANSI_COLOR_RED    L"\x1b[31m"
#define ANSI_COLOR_GREEN  L"\x1b[32m"
#define ANSI_COLOR_YELLOW L"\x1b[33m"
#define ANSI_COLOR_BLUE   L"\x1b[34m"
#define ANSI_COLOR_PURPLE L"\x1b[35m"
#define ANSI_COLOR_CYAN   L"\x1b[36m"
#define ANSI_COLOR_WHITE  L"\x1b[37m"
#define ANSI_COLOR_RESET  L"\x1b[0m"

int copy_c32(char32_t *str, const char32_t *input, int len);

int strlen32(const char32_t *strarg);

char *c_to_c(const char32_t *input, int len);

bool str32cmp(const char32_t *str1, const char32_t *str2);

bool str32cmp_n(const char32_t *big, const char32_t *small, int len);

bool str32cmp_gen(const char32_t *big, const char32_t *small);

bool str32cmp_gen_n(const char32_t *big, const char32_t *small, int len);
char32_t *char_to_32(char *input);

bool is_int(double val);
// use must free return value;
// as well as each token inside return value;
// should do NULL check for return value;
// which suggests memory allocation failed;
char32_t **split32(char32_t *str, char32_t *delimiter, int *len);
// check if `filepath` exists
bool does_file_exist(const char *filepath);

// print widechars to stdout without newline
void cp_print(const wchar_t *format, ...);

// print widechars to stdout with newline
void cp_println(const wchar_t *format, ...);

// print widechars to stderr without newline
void cp_err_print(const wchar_t *format, ...);

// print widechars to stderr without newline
void cp_err_println(const wchar_t *format, ...);

// print widechars to stdout without newline with colors
//  colorcode ->
//  			   r -> Red
//  			   b -> Blue
//  			   g -> Green
//  			   c -> Cyan
//  			   w -> White
//  			   p -> Purple
//  			   y -> Yellow
//  			   B -> Black
//  			   none -> default colors
void cp_color_print(wchar_t colorcode, const wchar_t *format, ...);

// print widechars to stdout with newline with colors
//  colorcode ->
//  			   r -> Red
//  			   b -> Blue
//  			   g -> Green
//  			   c -> Cyan
//  			   w -> White
//  			   p -> Purple
//  			   y -> Yellow
//  			   B -> Black
//  			   none -> default colors
void cp_color_println(wchar_t colorcode, const wchar_t *format, ...);

// print widechars to stderr without newline with colors
//  colorcode ->
//  			   r -> Red
//  			   b -> Blue
//  			   g -> Green
//  			   c -> Cyan
//  			   w -> White
//  			   p -> Purple
//  			   y -> Yellow
//  			   B -> Black
//  			   none -> default colors
void cp_err_color_print(wchar_t colorcode, const wchar_t *format, ...);

// print widechars to stdout with newline with colors
//  colorcode ->
//  			   r -> Red
//  			   b -> Blue
//  			   g -> Green
//  			   c -> Cyan
//  			   w -> White
//  			   p -> Purple
//  			   y -> Yellow
//  			   B -> Black
//  			   none -> default colors
void cp_err_color_println(wchar_t colorcode, const wchar_t *format, ...);

#endif
