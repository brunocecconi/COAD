
#include "Core/allocator.h"

#define PRINT_ALLOCATIONS 0

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

void* Mimalloc::allocate(sz n, i32)
{
#if PRINT_ALLOCATIONS
	printf("%s: allocated '%llu' bytes\n", get_name(), n);
#endif
	return mi_malloc(n);
}

void* Mimalloc::allocate(sz n, sz alignment, sz alignmentOffset, i32)
{ 
	#if EASTL_ALIGNED_MALLOC_AVAILABLE
		if((alignmentOffset % alignment) == 0) // We check for (offset % alignmnent == 0) instead of (offset == 0) because any block which is aligned on e.g. 64 also is aligned at an offset of 64 by definition. 
			return memalign(alignment, n); // memalign is more consistently available than posix_memalign.
	#else
		if((alignment <= EASTL_SYSTEM_ALLOCATOR_MIN_ALIGNMENT) && ((alignmentOffset % alignment) == 0))
			return mi_malloc(n);
	#endif
	return NULL;
}

void Mimalloc::deallocate(void* p, sz n)
{
#if PRINT_ALLOCATIONS
	printf("%s: %p: deallocated '%llu' bytes\n", get_name(), p, n);
#else
	(void)n;
#endif
	mi_free(p);
}

}
