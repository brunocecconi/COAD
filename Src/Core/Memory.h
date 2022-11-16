
#ifndef CORE_MEMORY_H
#define CORE_MEMORY_H

#include "Core/result.h"
#include "Core/allocator.h"

#include <EASTL/type_traits.h>
#include <EASTL/memory.h>

namespace Memory
{

template < u64 Index, u64 Size >
void CtClearMemory(u8* data)
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
	CtClearMemory<0, sizeof(T)>(reinterpret_cast<u8*>(value));
}

}

#endif