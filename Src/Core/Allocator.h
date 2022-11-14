
#ifndef CORE_ALLOCATOR_H
#define CORE_ALLOCATOR_H

#include "Core/Platform.h"
#include "Core/Types.h"

#include <vector>

#include <EASTL/memory.h>
#include <EASTL/allocator.h>
#include <EASTL/allocator_malloc.h>

#if PLATFORM_WINDOWS
#include <mimalloc.h>
#endif

namespace Allocators
{

class Mimalloc : public eastl::allocator
{
public:
	Mimalloc(const char* name = EASTL_NAME_VAL("Mimalloc"));
	Mimalloc(const Mimalloc& other);
	Mimalloc(const Mimalloc& other, const char* EASTL_NAME(name));

	Mimalloc& operator=(const Mimalloc& other);

	void* allocate(sz n, i32 /*flags*/ = 0);
	void* allocate(sz n, sz alignment, sz alignmentOffset, i32 /*flags*/ = 0);
	void deallocate(void* p, sz n);
};

#undef EASTLAllocatorType

#if PLATFORM_WINDOWS
#define EASTLAllocatorType	Allocators::Mimalloc
#else
#error Failed to set EASTLAllocatorType to default platform allocator. Create an allocator to the target platform.
#endif

using Default = EASTLAllocatorType;

}

#endif
