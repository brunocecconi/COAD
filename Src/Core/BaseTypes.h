
/** \file BaseTypes.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef CORE_BASE_TYPES_H
#define CORE_BASE_TYPES_H

#include "Core/Platform.h"

#include <climits>
#include <cfloat>

using Int8	  = signed char;
using Int16	  = signed short;
using Int32	  = signed int;
using Byte	  = unsigned char;
using Uint8	  = unsigned char;
using Uint16  = unsigned short;
using Uint32  = unsigned int;
using Float32 = float;
using Float64 = double;

#if PLATFORM_WINDOWS
using Int64	 = long long;
using Uint64 = unsigned long long;
#else
using Int64	  = long;
using Uint64  = unsigned long;
#endif

#if ARCH_64BIT == 64
using Size	  = Uint64;
using DiffPtr = Int64;
using IntPtr  = Int64;
#else
using Size	  = Uint32;
using DiffPtr = Int32;
using IntPtr  = Int32;
#endif

template<typename T>
static T g_type_default_value = T();

#endif
