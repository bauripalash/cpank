#include "include/utils.h"
#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>

void _print_ansi_color(wchar_t colorcode, FILE *stream) {
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

void color_reset() { wprintf(ANSI_COLOR_RESET); }

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
