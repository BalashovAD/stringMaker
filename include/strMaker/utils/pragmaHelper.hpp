#pragma once

namespace mkr::details {


#if defined(_MSC_VER)
#define DISABLE_WARNING_PUSH           __pragma(warning( push ))
#define DISABLE_WARNING_POP            __pragma(warning( pop ))
#define DISABLE_WARNING(warningNumber) __pragma(warning( disable : warningNumber ))

#elif defined(__GNUC__) || defined(__clang__)
#define DO_PRAGMA(X) _Pragma(#X)
#define DISABLE_WARNING_PUSH           DO_PRAGMA(GCC diagnostic push)
#define DISABLE_WARNING_POP            DO_PRAGMA(GCC diagnostic pop)
#define DISABLE_WARNING(warningName)   DO_PRAGMA(GCC diagnostic ignored #warningName)

#else
#define DISABLE_WARNING_PUSH
#define DISABLE_WARNING_POP
#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER
#define DISABLE_WARNING_UNREFERENCED_FUNCTION
#endif


#ifndef __EXCEPTIONS
static inline constexpr bool no_exceptions = true;
#else
static inline constexpr bool no_exceptions = false;
#endif


#ifndef __GXX_RTTI
static inline constexpr bool no_rtti = true;
#else
static inline constexpr bool no_rtti = false;
#endif

}