
/** @file Memory.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef CORE_MEMORY_H
#define CORE_MEMORY_H

#include "Core/result.h"
#include "Core/allocator.h"

#include <EASTL/type_traits.h>
#include <EASTL/memory.h>

namespace Memory
{

template < Uint64 Index, Uint64 Size >
void CtClearMemory(Uint8* data)
{
	if constexpr(Index < Size)
	{
		data[Index] = 0;
		CtClearMemory<Index+1, Size>(data);
	}
}

template < typename T >
void ClearMemoryType(T* value)
{
	CtClearMemory<0, sizeof(T)>(reinterpret_cast<Uint8*>(value));
}

}

#endif