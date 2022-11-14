
#ifndef CORE_MEMORY_H
#define CORE_MEMORY_H

#include "core/result.h"
#include "core/allocator.h"

#include <EASTL/type_traits.h>
#include <EASTL/memory.h>

template < typename T, typename TAllocator >
Result allocateArray(TAllocator& allocator, T** ptr, u64 size)
{
	if(!ptr)
	{
		return eResultErrorNullPtr;
	}
	if(*ptr)
	{
		return eResultErrorPtrIsNotNull;
	}
	if(size == 0)
	{
		return eResultErrorZeroSize;
	}
	*ptr = (T*)allocator.allocate(sizeof(T)*size);
	return *ptr ? eResultOk : eResultErrorMemoryOutOfMemory;
}

template < typename T, typename TAllocator, typename ... TArgs >
Result create(TAllocator& allocator, T** ptr, TArgs&&... args)
{
	if(!ptr)
	{
		return eResultErrorNullPtr;
	}
	if(*ptr)
	{
		return eResultErrorPtrIsNotNull;
	}
	*ptr = (T*)allocator.allocate(sizeof(T));
	if(*ptr == nullptr)
	{
		return eResultErrorMemoryOutOfMemory;
	}
	new (*ptr) T{eastl::forward<TArgs>(args)...};
	return eResultOk;
}

template < typename T, typename TAllocator, typename ... TArgs >
Result destroy(TAllocator& allocator, T** ptr)
{
	if(!(ptr && *ptr))
	{
		return eResultErrorNullPtr;
	}
	(*ptr)->~T();
	allocator.deallocate(*ptr, sizeof(T));
	*ptr = nullptr;
	return eResultOk;
}

#endif