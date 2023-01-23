
namespace Render::Api
{

/**
 * \brief Render api allocator.
 *
 */
class Allocator: public eastl::allocator
{
public:
	EXPLICIT Allocator(const char* Name = DEBUG_NAME_VAL("Render::Api::Vulkan"));

public:
	void* allocate(size_t N, int32_t /*flags*/ = 0);
	void* allocate(size_t N, size_t Alignment, size_t AlignmentOffset, int32_t /*flags*/ = 0);
	void* reallocate(void* Original, size_t N, int32_t /*flags*/ = 0);
	void* reallocate(void* Original, size_t N, size_t Alignment, size_t AlignmentOffset, int32_t /*flags*/ = 0);
	void  deallocate(void* P, size_t N);
};
using default_allocator_t = Allocator;

INLINE Allocator::Allocator(const char* Name) : eastl::allocator{Name}
{
}

INLINE void* Allocator::allocate(const size_t N, int32_t)
{
	return mi_malloc(N);
}

INLINE void* Allocator::allocate(const size_t N, const size_t Alignment, size_t, int32_t)
{
	return mi_malloc_aligned(N, Alignment);
}

INLINE void* Allocator::reallocate(void* Original, const size_t N, int32_t)
{
	return mi_realloc(Original, N);
}

INLINE void* Allocator::reallocate(void* Original, const size_t N, const size_t Alignment, size_t, int32_t)
{
	return mi_realloc_aligned(Original, N, Alignment);
}

INLINE void Allocator::deallocate(void* P, size_t)
{
	mi_free(P);
}

} // namespace Render::Api
