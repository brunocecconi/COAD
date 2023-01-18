
#ifndef CORE_RAW_BUFFER_H
#define CORE_RAW_BUFFER_H

#include "Core/Common.h"
#include "Core/Allocator.h"

/**
 * @brief Raw buffer class.
 *
 * Simple class to store typed data without default initialize memory.
 * Good when you need a buffer that does not need to be initialized and it will be handle by another function.
 *
 * @tparam T Target type.
 * @tparam TAllocator Target allocator.
 *
 */
template<typename T, typename TAllocator = EASTLAllocatorType>
class RawBuffer
{
public:
	using type_t	  = T;
	using size_t	  = size_t;
	using type_ptr_t  = T*;
	using type_ref_t  = T&;
	using allocator_t = TAllocator;

public:
	EXPLICIT RawBuffer(const allocator_t& Allocator);
	EXPLICIT RawBuffer(uint64_t Size, const allocator_t& Allocator);

	RawBuffer(RawBuffer&&) NOEXCEPT;
	RawBuffer& operator=(RawBuffer&&) NOEXCEPT;
	RawBuffer(const RawBuffer&)			   = delete;
	RawBuffer& operator=(const RawBuffer&) = delete;
	~RawBuffer();

public:
	NODISCARD type_ptr_t begin();
	NODISCARD type_ptr_t begin() const;
	NODISCARD type_ptr_t end();
	NODISCARD type_ptr_t end() const;

public:
	NODISCARD type_ptr_t Data();
	NODISCARD type_ptr_t Data() const;
	NODISCARD size_t	 Size() const;
	NODISCARD bool		 IsEmpty() const;
	const type_ref_t&	 At(uint64_t Index) const;

public:
	EXPLICIT   operator bool() const;
	type_ref_t operator[](uint64_t Index);

public:
	void Clear();
	void Resize(size_t NewSize);

	/**
	 * \brief This function just apply the size to a specific value lower than the current size.
	 *
	 * This function is useful to "clamp" the data to a lower buffer size. Can be,
	 * used when you need a local buffer but you don't need the full buffer to pass to another function, then you
	 * can clamp it since the buffer will the cleared in the end of the function anyway.
	 * The memory will not be modified.
	 *
	 * Obs: use it with caution, the current size of the buffer will be lost.  Don't use it if
	 * you need the full size after in another code section.
	 *
	 * \param Size : Target cursor size.
	 *
	 */
	void Clamp(size_t Size);

private:
	allocator_t mAllocator;
	type_t*		mValue{};
	size_t		mSize{};
};

template<typename T, typename TAllocator>
RawBuffer<T, TAllocator>::RawBuffer(const TAllocator& Allocator) : mAllocator{Allocator}
{
}

template<typename T, typename TAllocator>
RawBuffer<T, TAllocator>::RawBuffer(const uint64_t Size, const TAllocator& Allocator)
	: mAllocator{Allocator}, mValue{static_cast<T*>(mAllocator.allocate(
								 sizeof(T) * Size, alignof(T) < PLATFORM_ALIGNMENT ? PLATFORM_ALIGNMENT : alignof(T), 0,
								 0))},
	  mSize{Size}
{
}

template<typename T, typename TAllocator>
RawBuffer<T, TAllocator>::RawBuffer(RawBuffer&& Other) NOEXCEPT: mAllocator{Other.mAllocator},
																 mValue{Other.mValue},
																 mSize{Other.mSize}
{
	Other.mValue = nullptr;
	Other.mSize	 = 0ull;
}

template<typename T, typename TAllocator>
RawBuffer<T, TAllocator>& RawBuffer<T, TAllocator>::operator=(RawBuffer&& Other) NOEXCEPT
{
	mAllocator	 = Other.mAllocator;
	mValue		 = Other.mValue;
	mSize		 = Other.mSize;
	Other.mValue = nullptr;
	Other.mSize	 = 0ull;
	return *this;
}

template<typename T, typename TAllocator>
RawBuffer<T, TAllocator>::~RawBuffer()
{
	Clear();
}

template<typename T, typename TAllocator>
T* RawBuffer<T, TAllocator>::begin()
{
	return mValue;
}

template<typename T, typename TAllocator>
T* RawBuffer<T, TAllocator>::begin() const
{
	return mValue;
}

template<typename T, typename TAllocator>
T* RawBuffer<T, TAllocator>::end()
{
	return mValue + mSize;
}

template<typename T, typename TAllocator>
T* RawBuffer<T, TAllocator>::end() const
{
	return mValue + mSize;
}

template<typename T, typename TAllocator>
T* RawBuffer<T, TAllocator>::Data()
{
	return mValue;
}

template<typename T, typename TAllocator>
T* RawBuffer<T, TAllocator>::Data() const
{
	return mValue;
}

template<typename T, typename TAllocator>
size_t RawBuffer<T, TAllocator>::Size() const
{
	return mSize;
}

template<typename T, typename TAllocator>
bool RawBuffer<T, TAllocator>::IsEmpty() const
{
	return mValue == nullptr || mSize == 0ull;
}

template<typename T, typename TAllocator>
typename RawBuffer<T, TAllocator>::type_ref_t RawBuffer<T, TAllocator>::At(uint64_t Index) const
{
	return mValue[Index];
}

template<typename T, typename TAllocator>
RawBuffer<T, TAllocator>::operator bool() const
{
	return mValue;
}

template<typename T, typename TAllocator>
T& RawBuffer<T, TAllocator>::operator[](const uint64_t Index)
{
	return mValue[Index];
}

template<typename T, typename TAllocator>
void RawBuffer<T, TAllocator>::Clear()
{
	if (mValue)
	{
		mAllocator.deallocate(mValue, sizeof(T) * mSize);
		mValue = nullptr;
		mSize  = 0ull;
	}
}

template<typename T, typename TAllocator>
void RawBuffer<T, TAllocator>::Resize(const size_t NewSize)
{
	if (NewSize)
	{
		mAllocator.deallocate(mValue, sizeof(T) * mSize);
		mSize						= NewSize;
		constexpr size_t lAlignment = alignof(T) < PLATFORM_ALIGNMENT ? PLATFORM_ALIGNMENT : alignof(T);
		mValue						= static_cast<T*>(mAllocator.allocate(sizeof(T) * mSize, lAlignment, 0, 0));
	}
}

template<typename T, typename TAllocator>
void RawBuffer<T, TAllocator>::Clamp(const size_t Size)
{
	mSize = Size < mSize ? Size : mSize;
}

#endif
