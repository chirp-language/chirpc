#pragma once

#define CHIRP_PLATFORMID_UNKNOWN 0
#define CHIRP_PLATFORMID_UNIX 1
#define CHIRP_PLATFORMID_WINNT 2
#define CHIRP_PLATFORMID_LINUX 3
#define CHIRP_PLATFORMID_APPLE 4
#define CHIRP_PLATFORMID_BSD 5

#if defined(__unix__) || defined(__APPLE_CC__)
#define CHIRP_PLATFORM_UNIX
#define CHIRP_PLATFORM CHIRP_PLATFORMID_UNIX

#if defined(__linux) || defined(linux) || defined(__linux__)
#define CHIRP_SUBPLATFORM CHIRP_PLATFORMID_LINUX
#elif defined(__APPLE__) || defined(macintosh) || defined(__MACH__)
#define CHIRP_SUBPLATFORM CHIRP_PLATFORMID_APPLE
#elif defined(__DragonFly__) || defined(__FreeBSD)
#define CHIRP_SUBPLATFORM CHIRP_PLATFORMID_BSD
#else
#define CHIRP_SUBPLATFORM CHIRP_PLATFORMID_UNIX
#endif

#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define CHIRP_PLATFORM_WINNT
#define CHIRP_PLATFORM CHIRP_PLATFORMID_WINNT
#define CHIRP_SUBPLATFORM CHIRP_PLATFORMID_WINNT
#else
#define CHIRP_PLATFORM_UNKNOWN
#define CHIRP_PLATFORM CHIRP_PLATFORMID_UNKNOWN
#define CHIRP_SUBPLATFORM CHIRP_PLATFORMID_UNKNOWN
#endif

#ifdef NDEBUG
#define chirp_unreachable(msg) __builtin_unreachable()
#else
#define ___STR(n) #n
#define __STR(n) ___STR(n)
#define chirp_unreachable(msg) __chirp_unreachable(__FILE__ ":" __STR(__LINE__) ": " msg)
#define __CHIRP_UNREACHABLE_AVAILABLE
[[noreturn]]extern void __chirp_unreachable(char const* message);
#endif

#define chirp_assert(cond, msg) do \
    { \
        if (!static_cast<bool>(cond)) \
            chirp_unreachable(msg); \
    } while(false)
