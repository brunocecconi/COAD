
#ifndef CORE_RAW_BUFFER_H
#define CORE_RAW_BUFFER_H

#include "Core/Common.h"
#include "Core/Allocator.h"

/**
 * @brief Raw buffer class.
 *
 * Simple class to store typed data without default initialize memory.
 * Good when you need a buffer that doesn't need to be initialized and will be handle by another function.
 *
 * @tparam T Target type.
 * @tparam TAllocator Target allocator.
 *
 */
template<typename T, typename TAllocator = EASTLAllocatorType>
class RawBuffer
{
public:
	EXPLICIT RawBuffer(const TAllocator& Allocator);
	EXPLICIT RawBuffer(uint64_t Size, const TAllocator& Allocator);

	RawBuffer(RawBuffer&&) NOEXCEPT;
	RawBuffer& operator=(RawBuffer&&) NOEXCEPT;
	RawBuffer(const RawBuffer&)			   = delete;
	RawBuffer& operator=(const RawBuffer&) = delete;

	~RawBuffer();

public:
	NODISCARD T* begin();
	NODISCARD T* begin() const;
	NODISCARD T* end();
	NODISCARD T* end() const;

public:
	NODISCARD T*	 Data();
	NODISCARD T*	 Data() const;
	NODISCARD size_t Size() const;
	NODISCARD bool	 IsEmpty() const;

public:
	EXPLICIT operator bool() const;
	T&		 operator[](uint64_t Index);
	const T& operator[](uint64_t Index) const;

public:
	void Clear();
	void Resize(size_t NewSize);

private:
	TAllocator mAllocator;
	T*		   mValue;
	size_t	   mSize;
};

template<typename T, typename TAllocator>
RawBuffer<T, TAllocator>::RawBuffer(const TAllocator& Allocator) : mAllocator{Allocator}, mValue{}, mSize{}
{
}

template<typename T, typename TAllocator>
RawBuffer<T, TAllocator>::RawBuffer(const uint64_t Size, const TAllocator& Allocator)
	: mAllocator{Allocator}, mValue{static_cast<T*>(mAllocator.allocate(sizeof(T) * Size))}, mSize{Size}
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
const T& RawBuffer<T, TAllocator>::operator[](uint64_t Index) const
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
		mSize  = NewSize;
		mValue = static_cast<T*>(mAllocator.allocate(sizeof(T) * mSize));
	}
}
#endif
