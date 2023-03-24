#ifndef cpank_utils_h
#define cpank_utils_h

#define ANSI_COLOR_BLACK L"\x1b[30m"
#define ANSI_COLOR_RED L"\x1b[31m"
#define ANSI_COLOR_GREEN L"\x1b[32m"
#define ANSI_COLOR_YELLOW L"\x1b[33m"
#define ANSI_COLOR_BLUE L"\x1b[34m"
#define ANSI_COLOR_PURPLE L"\x1b[35m"
#define ANSI_COLOR_CYAN L"\x1b[36m"
#define ANSI_COLOR_WHITE L"\x1b[37m"
#define ANSI_COLOR_RESET L"\x1b[0m"

#include <wchar.h>

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
