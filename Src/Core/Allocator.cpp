
/** \file Allocator.cpp
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#include "Core/Allocator.h"
#include "Core/Assert.h"

#ifndef RELEASE
#ifndef PRINT_ALLOCATIONS
#define PRINT_ALLOCATIONS 1
#endif
#ifndef PRINT_DEALLOCATIONS
#define PRINT_DEALLOCATIONS 1
#endif
#else
#define PRINT_ALLOCATIONS	0
#define PRINT_DEALLOCATIONS 0
#endif

#if DEBUG
#include <EASTL/string_hash_map.h>
using string_hash_map_t = eastl::string_hash_map<int64_t, eastl::hash<const char*>, eastl::str_equal_to<const char*>,
												 Allocators::SimpleMimalloc>;
#endif

namespace Allocators
{

SimpleMimalloc::SimpleMimalloc(const char* Name) : eastl::allocator{Name}
{
}

SimpleMimalloc::SimpleMimalloc(const SimpleMimalloc& Other)
{
#if EASTL_NAME_ENABLED
	mpName = Other.mpName;
#else
	(void)Other;
#endif
}
SimpleMimalloc::SimpleMimalloc(const SimpleMimalloc&, const char* EASTL_NAME(Name))
{
#if EASTL_NAME_ENABLED
	mpName = Name ? Name : "MimallocAlocator";
#endif
}
SimpleMimalloc& SimpleMimalloc::operator=(const SimpleMimalloc& Other)
{
#if EASTL_NAME_ENABLED
	mpName = Other.mpName;
#else
	(void)Other;
#endif
	return *this;
}

void* SimpleMimalloc::allocate(size_t N, int32_t)
{
	return mi_malloc(N);
}

void* SimpleMimalloc::allocate(size_t N, size_t Alignment, size_t AlignmentOffset, int32_t)
{
#if EASTL_ALIGNED_MALLOC_AVAILABLE
	if ((alignmentOffset % alignment) ==
		0) // We check for (offset % alignmnent == 0) instead of (offset == 0) because any block which is aligned on
		   // e.g. 64 also is aligned at an offset of 64 by definition.
		return memalign(alignment, n); // memalign is more consistently available than posix_memalign.
#else
	if ((Alignment <= EASTL_SYSTEM_ALLOCATOR_MIN_ALIGNMENT) && ((AlignmentOffset % Alignment) == 0))
		return mi_malloc(N);
#endif
	return NULL;
}

void SimpleMimalloc::deallocate(void* P, size_t)
{
	mi_free(P);
}

#if DEBUG
void* gMemoryTrackHashMap = nullptr;
#define MEMORY_TRACK_HASH_MAP (*static_cast<string_hash_map_t*>(gMemoryTrackHashMap))

float32_t GetAllocatedSize(const char* Name)
{
	return static_cast<float32_t>(MEMORY_TRACK_HASH_MAP[Name]) / 1024.f;
}

void MemoryTrackCreateIfNotExist()
{
	if (!gMemoryTrackHashMap)
	{
		atexit([]() {
			eastl::destroy_at(&MEMORY_TRACK_HASH_MAP);
			SimpleMimalloc{"Allocator DebugMemoryTrack"}.deallocate(gMemoryTrackHashMap, sizeof(string_hash_map_t));
		});
		gMemoryTrackHashMap =
			new (mi_malloc(sizeof(string_hash_map_t))) string_hash_map_t{SimpleMimalloc{"Allocator DebugMemoryTrack"}};
		MEMORY_TRACK_HASH_MAP.reserve(64ull);
		ENFORCE_MSG(gMemoryTrackHashMap, "Failed to create memory track hash map.");
	}
}

#endif

Mimalloc::Mimalloc(const char* Name) : eastl::allocator{Name}
{
#if DEBUG
	MemoryTrackCreateIfNotExist();
#endif
}

Mimalloc::Mimalloc(const Mimalloc& Other)
{
#if EASTL_NAME_ENABLED
	mpName = Other.mpName;
#else
	(void)Other;
#endif
}
Mimalloc::Mimalloc(const Mimalloc&, const char* EASTL_NAME(Name))
{
#if EASTL_NAME_ENABLED
	mpName = Name ? Name : "MimallocAlocator";
#endif
}
Mimalloc& Mimalloc::operator=(const Mimalloc& Other)
{
#if EASTL_NAME_ENABLED
	mpName = Other.mpName;
#else
	(void)Other;
#endif
	return *this;
}

void* Mimalloc::allocate(size_t N, int32_t)
{
#if PRINT_ALLOCATIONS
	printf("%s: allocated '%f' KB\n", get_name(), static_cast<float32_t>(N) / 1024.f);
#endif
#if DEBUG
	MEMORY_TRACK_HASH_MAP[get_name()] += N;
#endif
	return mi_malloc(N);
}

void* Mimalloc::allocate(size_t N, size_t Alignment, size_t AlignmentOffset, int32_t)
{
#if EASTL_ALIGNED_MALLOC_AVAILABLE
	if ((alignmentOffset % alignment) ==
		0) // We check for (offset % alignmnent == 0) instead of (offset == 0) because any block which is aligned on
		   // e.g. 64 also is aligned at an offset of 64 by definition.
		return memalign(alignment, n); // memalign is more consistently available than posix_memalign.
#else
	if ((Alignment <= EASTL_SYSTEM_ALLOCATOR_MIN_ALIGNMENT) && ((AlignmentOffset % Alignment) == 0))
	{
#if DEBUG
		MEMORY_TRACK_HASH_MAP[get_name()] += N;
#endif
		return mi_malloc(N);
	}
#endif
	return NULL;
}

void Mimalloc::deallocate(void* P, size_t N)
{
#if PRINT_DEALLOCATIONS
	printf("%s: %p: deallocated '%f' KB\n", get_name(), P, static_cast<float32_t>(N) / 1024.f);
#else
	(void)N;
#endif
#if DEBUG
	MEMORY_TRACK_HASH_MAP[get_name()] -= N;
#endif
	mi_free(P);
}

} // namespace Allocators
