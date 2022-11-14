
#ifndef CORE_BASE_TYPES_H
#define CORE_BASE_TYPES_H

#include "Core/Platform.h"

#include <climits>
#include <cfloat>
 
using i8	= signed char;
using i16	= signed short;
using i32	= signed int;
using byte	= unsigned char;
using u8	= unsigned char;
using u16	= unsigned short;
using u32	= unsigned int;
using f32	= float;
using f64	= double;

#if PLATFORM_WINDOWS
using i64 = long long;
using u64 = unsigned long long;
#else
using i64 = long;
using u64 = unsigned long;
#endif

#if ARCH_64BIT == 64
using sz = u64;
using diffp = i64;
using iptr = i64;
#else
using sz = u32;
using diffp = i32;
using iptr = i32;
#endif

template < typename T >
static T g_type_default_value = T();

static constexpr i8 I8_MIN		= _I8_MIN;
static constexpr i16 I16_MIN	= _I16_MIN;
static constexpr i32 I32_MIN	= _I32_MIN;
static constexpr i64 I64_MIN	= _I64_MIN;
static constexpr f32 F32_MIN	= FLT_MIN;
static constexpr f64 F64_MIN	= DBL_MIN;
static constexpr i8 I8_MAX		= _I8_MAX;
static constexpr i16 I16_MAX	= _I16_MAX;
static constexpr i32 I32_MAX	= _I32_MAX;
static constexpr i64 I64_MAX	= _I64_MAX;
static constexpr u8 U8_MAX		= _UI8_MAX;
static constexpr u16 U16_MAX	= _UI16_MAX;
static constexpr u32 U32_MAX	= _UI32_MAX;
static constexpr u64 U64_MAX	= _UI64_MAX;
static constexpr f32 F32_MAX	= FLT_MAX;
static constexpr f64 F64_MAX	= DBL_MAX;

#endif
