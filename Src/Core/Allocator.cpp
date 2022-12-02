
/** \file Allocator.cpp
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#include "Core/Allocator.h"

#ifndef RELEASE
#define PRINT_ALLOCATIONS 0
#else
#define PRINT_ALLOCATIONS 0
#endif

namespace Allocators
{

Mimalloc::Mimalloc(const char* name) : eastl::allocator{name}
{
}
Mimalloc::Mimalloc(const Mimalloc& other)
{
#if EASTL_NAME_ENABLED
	mpName = other.mpName;
#else
	(void)other;
#endif
}
Mimalloc::Mimalloc(const Mimalloc&, const char* EASTL_NAME(name))
{
#if EASTL_NAME_ENABLED
	mpName = name ? name : "MimallocAlocator";
#endif
}
Mimalloc& Mimalloc::operator=(const Mimalloc& other)
{
#if EASTL_NAME_ENABLED
	mpName = other.mpName;
#else
	(void)other;
#endif
	return *this;
}

void* Mimalloc::allocate(Size n, Int32)
{
#if PRINT_ALLOCATIONS
	printf("%s: allocated '%f' KB\n", get_name(), static_cast<Float32>(n) / 1024.f);
#endif
	return mi_malloc(n);
}

void* Mimalloc::allocate(Size n, Size alignment, Size alignmentOffset, Int32)
{
#if EASTL_ALIGNED_MALLOC_AVAILABLE
	if ((alignmentOffset % alignment) ==
		0) // We check for (offset % alignmnent == 0) instead of (offset == 0) because any block which is aligned on
		   // e.g. 64 also is aligned at an offset of 64 by definition.
		return memalign(alignment, n); // memalign is more consistently available than posix_memalign.
#else
	if ((alignment <= EASTL_SYSTEM_ALLOCATOR_MIN_ALIGNMENT) && ((alignmentOffset % alignment) == 0))
		return mi_malloc(n);
#endif
	return NULL;
}

void Mimalloc::deallocate(void* p, Size n)
{
#if PRINT_ALLOCATIONS
	printf("%s: %p: deallocated '%f' KB\n", get_name(), p, static_cast<Float32>(n) / 1024.f);
#else
	(void)n;
#endif
	mi_free(p);
}

} // namespace Allocators
