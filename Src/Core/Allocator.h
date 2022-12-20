
/** \file Allocator.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef CORE_ALLOCATOR_H
#define CORE_ALLOCATOR_H

#include "Core/Compiler.h"
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

class SimpleMimalloc : public eastl::allocator
{
public:
	SimpleMimalloc(const char* Name = EASTL_NAME_VAL("SimpleMimalloc"));
	SimpleMimalloc(const SimpleMimalloc& Other);
	SimpleMimalloc(const SimpleMimalloc& Other, const char* EASTL_NAME(Name));

	SimpleMimalloc& operator=(const SimpleMimalloc& Other);

	void* allocate(size_t N, int32_t /*flags*/ = 0);
	void* allocate(size_t N, size_t Alignment, size_t AlignmentOffset, int32_t /*flags*/ = 0);
	void  deallocate(void* P, size_t N);
};

class Mimalloc: public eastl::allocator
{
public:
	Mimalloc(const char* Name = EASTL_NAME_VAL("Mimalloc"));
	Mimalloc(const Mimalloc& Other);
	Mimalloc(const Mimalloc& Other, const char* EASTL_NAME(Name));

	Mimalloc& operator=(const Mimalloc& Other);

	void* allocate(size_t N, int32_t /*flags*/ = 0);
	void* allocate(size_t N, size_t Alignment, size_t AlignmentOffset, int32_t /*flags*/ = 0);
	void  deallocate(void* P, size_t N);
};

#undef EASTLAllocatorType

#if PLATFORM_WINDOWS
#define EASTLAllocatorType Allocators::Mimalloc
#else
#error Failed to set EASTLAllocatorType to default platform allocator. Create an allocator to the target platform.
#endif

using Default = EASTLAllocatorType;

#if DEBUG
NODISCARD float32_t GetAllocatedSize(const char* Name);
#endif

} // namespace Allocators

#endif
