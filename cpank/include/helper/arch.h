#ifndef cpank_helper_arch_h
#define cpank_helper_arch_h
#ifdef __cplusplus
extern "C" {
#endif
// PANK_ARCH_64 --> x86_64 / 64bit
// PANK_ARCH_32 --> x86 / 32bit
// PANK_ARCH_ARM --> ARM (32bit / 64bit)

#if defined(__amd64) || defined(__amd64__) || defined(__x86_64) || \
    defined(__x86_64__) || defined(_MX64) || defined(_M_AMD64) ||  \
    defined(__aarch64__)
 #define PANK_ARCH_64
#elif defined(i386) || defined(__i386) || defined(__i386__) ||    \
    defined(_i486__) || defined(__i586__) || defined(__i686__) || \
    defined(_M_I86) || defined(_M_IX86) || defined(__X86__) ||    \
    defined(_X86_) || defined(__I86__) || defined(__INTEL__) || defined(__386)
 #define PANK_ARCH_32
#elif defined(__arm__) || defined(__thumb__) || defined(__ARM) || \
    defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) || \
    defined(_M_ARM) || defined(__arm) || defined(_M_ARMT) ||      \
    defined(__aarch64__)
 #define PANK_ARCH_ARM
#endif
#ifdef __cplusplus
}
#endif
#endif
