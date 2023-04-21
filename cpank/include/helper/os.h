#ifndef cpank_helper_os_h
#define cpank_helper_os_h

// PANK_OS_ANDROID --> Android
// PANK_OS_WIN  --> Windows
// PANK_OS_LINUX --> Linux (& unix like)
// PANK_OS_UNIX --> Unix or Unix like
// PANK_OS_MACOS --> Mac OS

#if defined(__ANDROID__) || defined(__ANDROID_API__)
 #define PANK_OS_ANDROID
#elif defined(__CYGWIN__) || defined(__WIN16) || defined(__WIN32) ||  \
    defined(__WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || \
    defined(__WINDOWS__) || defined(__WIN32_CE__) || defined(_WIN32)
 #define PANK_OS_WIN
#elif defined(__linux__) || defined(__linux) || defined(linux)
 #define PANK_OS_LINUX
 #define PANK_OS_UNIX
#elif defined(__unix__) || defined(__unix)
 #define PANK_OS_UNIX
#elif defined(macintosh) || defined(Macintosh) || defined(__APPLE__) || \
    defined(__MACH__)
 #define PANK_OS_MACOS
#endif

#endif
