#ifndef cpank_common_h
#define cpank_common_h

// #define DEBUG_TRACE
// #define DEBUG_STACK
// #define DEBUG_LEXER
// #define DEBUG_PRINT_CODE
// #define DEBUG_STRES_GC
// #define DEBUG_LOG_GC
#define UINT8_COUNT (UINT8_MAX + 1)
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ERC_RUNTIME 10
#define ERC_COMPTIME 11
#define ERC_FAIL_TO_OPEN 12
#define ERC_FILE_NOT_EXIST 13
#define ERC_NO_MEM 14

// #define DEBUG_PRINT_CODE
#define NAN_BOXING

#ifdef _WIN32
#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef UNICODE
#define UNICODE
#endif  // !UNICODE
#endif

#ifdef _WIN32
#define IS_WIN
#endif

#endif
