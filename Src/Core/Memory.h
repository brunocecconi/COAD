
/** @file Memory.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef CORE_MEMORY_H
#define CORE_MEMORY_H

#include "Core/Result.h"
#include "Core/Allocator.h"
#include "Core/Lang.h"
#include "Core/TypeTraits.h"
#include "Macros.h"

#include <EASTL/type_traits.h>
#include <EASTL/memory.h>
#include <EASTL/vector.h>
#include <EASTL/array.h>
#include <EASTL/string.h>
#include <EASTL/hash_map.h>

namespace Memory
{

/**
 * \brief Align function. Calculates aligned size.
 *
 * \param Size : Target size.
 * \param Alignment : Target alignment.
 * \return New size aligned.
 *
 */
constexpr size_t Align(const size_t Size, const size_t Alignment = PLATFORM_ALIGNMENT)
{
	return (Size + (Alignment-1)) & ~(Alignment-1);
}

template<uint64_t Index, uint64_t Size>
void CtClearMemory(uint8_t* data)
{
	if constexpr (Index < Size)
	{
		data[Index] = 0;
		CtClearMemory<Index + 1, Size>(data);
	}
}

template<typename T>
void ClearMemoryType(T* value)
{
	CtClearMemory<0, sizeof(T)>(reinterpret_cast<uint8_t*>(value));
}

} // namespace Memory

#endif
