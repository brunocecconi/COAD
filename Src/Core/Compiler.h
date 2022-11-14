
#ifndef CORE_COMPILER_H
#define CORE_COMPILER_H

#ifdef USE_EXPLICIT
#define EXPLICIT			explicit
#else
#define EXPLICIT
#endif

#define NOEXCEPT			noexcept
#define INLINE				inline
#define DEPRECATED(MSG)		[[deprecated(MSG)]]
#define NORETURN			[[noreturn]]
#define NODISCARD			[[nodiscard]]
#define MAYBEUNUSED			[[maybe_unused]]

#if _MSC_VER
#define FORCE_INLINE		__forceinline
#else
#define FORCE_INLINE		[[always_inline]]
#endif

#endif
