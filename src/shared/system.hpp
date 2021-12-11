#pragma once

#ifdef NDEBUG
#define chirp_unreachable(msg) __builtin_unreachable()
#else
#define ___STR(n) #n
#define __STR(n) ___STR(n)
#define chirp_unreachable(msg) __chirp_unreachable(__FILE__ ":" __STR(__LINE__) ": " msg)
#define __CHIRP_UNREACHABLE_AVAILABLE
[[noreturn]]extern void __chirp_unreachable(char const* message);
#endif
