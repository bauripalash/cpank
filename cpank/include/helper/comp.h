#ifndef cpank_helper_compiler_h
#define cpank_helper_compiler_h
#ifdef __cplusplus
extern "C" {
#endif
#if defined(__MINGW32__) || defined(__MINGW64__)
 #define PANK_COMP_MINGW
#elif defined(__clang__)
 #define PANK_COMP_CLANG
#elif defined(_MSC_VER)
 #define PANK_COMP_MSVC
#elif defined(__GNUC__) || defined(__GNUC_MINOR__) || \
    defined(__GNUC_PATCHLEVEL__)
 #define PANK_COMP_GCC
#endif
#ifdef __cplusplus
}
#endif
#endif
