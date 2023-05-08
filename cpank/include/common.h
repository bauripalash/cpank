/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#ifndef cpank_common_h
#define cpank_common_h

// #define DEBUG
// #define DEBUG_TRACE
// #define DEBUG_STACK
// #define DEBUG_LEXER
// #define DEBUG_PRINT_CODE
// #define DEBUG_STRES_GC
// #define DEBUG_LOG_GC
#define UINT8_COUNT (UINT8_MAX + 1)
// #define NO_BIGNUM
// #define NO_DOUBLE_BIGNUM
// #define MODE_ENGLISH
// #define MODE_BENGALI
// #define NO_XXHASH
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "helper/arch.h"
// PANK_ARCH_64 --> x86_64 / 64bit
// PANK_ARCH_32 --> x86 / 32bit
// PANK_ARCH_ARM --> ARM (32bit / 64bit)
#include "helper/os.h"
// PANK_OS_ANDROID --> Android
// PANK_OS_WIN  --> Windows
// PANK_OS_LINUX --> Linux (& unix like)
// PANK_OS_UNIX --> Unix or Unix like
// PANK_OS_MACOS --> Mac OS

#define ERC_RUNTIME        10
#define ERC_COMPTIME       11
#define ERC_FAIL_TO_OPEN   12
#define ERC_FILE_NOT_EXIST 13
#define ERC_NO_MEM         14

// #define DEBUG_PRINT_CODE
#if defined(PANK_ARCH_64)
 #define NAN_BOXING
#endif
#ifdef PANK_OS_WIN
 #define IS_WIN
#endif

#if defined(PANK_OS_LINUX) || defined(PANK_OS_UNIX)
 #define IS_UNIX
#endif

#if defined(PANK_OS_MACOS)
 #define IS_MAC
#endif

#ifdef IS_WIN
 #ifndef _UNICODE
  #define _UNICODE
 #endif

 #ifndef UNICODE
  #define UNICODE
 #endif  // !UNICODE
#endif

#endif
