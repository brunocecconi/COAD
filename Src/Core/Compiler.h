
/** \file Compiler.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef CORE_COMPILER_H
#define CORE_COMPILER_H

#if USE_EXPLICIT
#define EXPLICIT explicit
#else
#define EXPLICIT
#endif

#define NOEXCEPT		noexcept
#define INLINE			inline
#define DEPRECATED(MSG) [[deprecated(MSG)]]
#define NORETURN		[[noreturn]]
#define NODISCARD		[[nodiscard]]
#define MAYBEUNUSED		[[maybe_unused]]
#define ALIGNAS(VALUE)	alignas(VALUE)

#if _MSC_VER
#define FORCE_INLINE		__forceinline
#define __PRETTY_FUNCTION__ __FUNCSIG__
#else
#define FORCE_INLINE [[always_inline]]
#endif

#endif
