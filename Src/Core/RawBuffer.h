
#ifndef CORE_RAW_BUFFER_H
#define CORE_RAW_BUFFER_H

#include "Core/Common.h"
#include "Core/Allocator.h"

template<typename T, typename Allocator = EASTLAllocatorType>
class RawBuffer
{
public:
	EXPLICIT RawBuffer(const Uint64 size, const Allocator& allocator = {DEBUG_NAME_VAL("RawBuffer")})
		: allocator_{allocator}, value_{static_cast<T*>(allocator_.allocate(sizeof(T) * size))}
	{
	}

	RawBuffer(RawBuffer&&) NOEXCEPT = default;
	RawBuffer& operator=(RawBuffer&&) NOEXCEPT = default;
	RawBuffer(const RawBuffer&) = delete;
	RawBuffer& operator=(const RawBuffer&) = delete;

	~RawBuffer()
	{
		if(value_)
		{
			allocator_.deallocate(value_, 0);
			value_ = nullptr;
		}
	}

public:
	NODISCARD T* Get()
	{
		return value_;
	}

	NODISCARD T* Get() const
	{
		return value_;
	}

	EXPLICIT operator bool() const
	{
		return value_;
	}

	T& operator[](const Uint64 index)
	{
		return value_[index];
	}

private:
	Allocator allocator_;
	T* value_;
};

#endif
