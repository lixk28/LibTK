#pragma once

/* ALWAYS_INLINE */

#if !defined(ALWAYS_INLINE) && defined(NDEBUG) && defined(__GNUC__)
#define ALWAYS_INLINE [[gnu::always_inline]] inline
#endif

#if !defined(ALWAYS_INLINE) && defined(NDEBUG) && defined(__clang__)
#define ALWAYS_INLINE [[clang::always_inline]] inline
#endif

#if !defined(ALWAYS_INLINE)
#define ALWAYS_INLINE inline
#endif

/* NEVER_INLINE */

#if !defined(NEVER_INLINE) && defined(NDEBUG) && defined(__GNUC__)
#define NEVER_INLINE __attribute__((noinline))
#endif

#if !defined(NEVER_INLINE) && defined(NDEBUG) && defined(__clang__)
#define NEVER_INLINE [[clang::noinline]]
#endif

#if !defined(NEVER_INLINE)
#define NEVER_INLINE
#endif
