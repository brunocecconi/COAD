
#include "Core/Memory.h"

namespace Memory
{

Stream::Stream(const Allocators::Default& allocator) : data_{allocator}
{
}

Stream::Stream(const void* data, const container_t::size_type size,
			   const Allocators::Default& allocator RESULT_PARAM_IMPL)
	: data_{allocator}
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(data, eResultErrorNullPtr);
	RESULT_CONDITION_ENSURE_NOLOG(size, eResultErrorZeroSize);
	data_.reserve(size);
	eastl::uninitialized_move_n(static_cast<const Byte*>(data), size, data_.data());
	RESULT_OK();
}

Stream::Stream(const container_t::size_type size, const Allocators::Default& allocator RESULT_PARAM_IMPL)
	: data_{allocator}
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(size, eResultErrorZeroSize);
	data_.resize(size);
	RESULT_OK();
}

Stream::Stream(container_t&& container, const Allocators::Default& allocator RESULT_PARAM_IMPL)
	: data_{eastl::move(container), allocator}
{
}

bool Stream::BufferIsEmpty() const NOEXCEPT
{
	return data_.empty();
}

Stream::container_t::size_type Stream::BufferSize() const NOEXCEPT
{
	return data_.size();
}

Stream::container_t::size_type Stream::BufferCapacity() const NOEXCEPT
{
	return data_.capacity();
}

void Stream::Resize(const container_t::size_type size, const container_t::value_type& value RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(size, eResultErrorZeroSize);
	data_.resize(size, value);
	RESULT_OK();
}

void Stream::Resize(const container_t::size_type size RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(size, eResultErrorZeroSize);
	data_.resize(size);
	RESULT_OK();
}

void Stream::Reserve(const container_t::size_type size RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(size, eResultErrorZeroSize);
	data_.resize(size);
	RESULT_OK();
}

void Stream::ShrinkToFit()
{
	data_.shrink_to_fit();
}

Stream::container_t::pointer Stream::Data() NOEXCEPT
{
	return data_.data();
}

Stream::container_t::const_pointer Stream::Data() const NOEXCEPT
{
	return data_.data();
}

Stream::container_t::reference Stream::operator[](const container_t::size_type index)
{
	return data_.operator[](index);
}

Stream::container_t::const_reference Stream::operator[](const container_t::size_type index) const
{
	return data_.operator[](index);
}

Stream::container_t::reference Stream::At(const container_t::size_type index)
{
	return data_.at(index);
}

Stream::container_t::const_reference Stream::At(const container_t::size_type index) const
{
	return data_.at(index);
}

void Stream::Clear() NOEXCEPT
{
	data_.clear();
}

bool Stream::Validate() const NOEXCEPT
{
	return data_.validate();
}

Uint64 Stream::WriteCursor() const
{
	return data_.size();
}

Uint64 Stream::ReadCursor() const
{
	return read_cursor_;
}

bool Stream::Write(const void* data, const container_t::size_type size RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	RESULT_CONDITION_ENSURE_NOLOG(data, eResultErrorNullPtr, false);
	RESULT_CONDITION_ENSURE_NOLOG(size, eResultErrorZeroSize, false);
	data_.reserve(data_.size() + size);
	data_.insert(data_.cend(), static_cast<const Byte*>(data), static_cast<const Byte*>(data) + size);
	RESULT_OK();
	return true;
}

bool Stream::Read(void* data, const container_t::size_type size RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	RESULT_CONDITION_ENSURE_NOLOG(data, eResultErrorNullPtr, false);
	RESULT_CONDITION_ENSURE_NOLOG(size, eResultErrorZeroSize, false);
	if(read_cursor_ + size > data_.size())
	{
		RESULT_ERROR(eResultErrorMemoryOutOfMemory, false);
	}
	memcpy(data, data_.data() + read_cursor_, size);
	read_cursor_ += size;
	RESULT_OK();
	return true;
}

} // namespace Memory
