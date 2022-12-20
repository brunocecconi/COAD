
#include "Core/Stream.h"

namespace Stream
{

Dynamic::Dynamic(const Allocators::Default& allocator, RESULT_PARAM_IMPL) : data_{allocator}
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_OK();
}

Dynamic::Dynamic(const void* data, const container_t::size_type size, const Allocators::Default& allocator,
				 RESULT_PARAM_IMPL)
	: data_{allocator}
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(data, eResultErrorNullPtr);
	RESULT_CONDITION_ENSURE_NOLOG(size, eResultErrorZeroSize);
	data_.reserve(size);
	eastl::uninitialized_move_n(static_cast<const byte_t*>(data), size, data_.data());
	RESULT_OK();
}

Dynamic::Dynamic(const container_t::size_type size, const Allocators::Default& allocator, RESULT_PARAM_IMPL)
	: data_{allocator}
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(size, eResultErrorZeroSize);
	data_.resize(size);
	RESULT_OK();
}

Dynamic::Dynamic(container_t&& container, const Allocators::Default& allocator, RESULT_PARAM_IMPL)
	: data_{eastl::move(container), allocator}
{
}

bool Dynamic::BufferIsEmpty() const NOEXCEPT
{
	return data_.empty();
}

Dynamic::container_t::size_type Dynamic::BufferSize() const NOEXCEPT
{
	return data_.size();
}

Dynamic::container_t::size_type Dynamic::BufferCapacity() const NOEXCEPT
{
	return data_.capacity();
}

void Dynamic::Resize(const container_t::size_type size, const container_t::value_type& value, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(size, eResultErrorZeroSize);
	data_.resize(size, value);
	RESULT_OK();
}

void Dynamic::Resize(const container_t::size_type size, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(size, eResultErrorZeroSize);
	data_.resize(size);
	RESULT_OK();
}

void Dynamic::Reserve(const container_t::size_type size, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(size, eResultErrorZeroSize);
	data_.resize(size);
	RESULT_OK();
}

void Dynamic::ShrinkToFit()
{
	data_.shrink_to_fit();
}

Dynamic::container_t& Dynamic::Container()
{
	return data_;
}

Dynamic::container_t::pointer Dynamic::Data() NOEXCEPT
{
	return data_.data();
}

Dynamic::container_t::const_pointer Dynamic::Data() const NOEXCEPT
{
	return data_.data();
}

Dynamic::container_t::reference Dynamic::operator[](const container_t::size_type index)
{
	return data_.operator[](index);
}

Dynamic::container_t::const_reference Dynamic::operator[](const container_t::size_type index) const
{
	return data_.operator[](index);
}

Dynamic::container_t::reference Dynamic::At(const container_t::size_type index)
{
	return data_.at(index);
}

Dynamic::container_t::const_reference Dynamic::At(const container_t::size_type index) const
{
	return data_.at(index);
}

void Dynamic::Clear() NOEXCEPT
{
	data_.clear();
}

bool Dynamic::Validate() const NOEXCEPT
{
	return data_.validate();
}

uint64_t Dynamic::WriteCursor() const
{
	return data_.size();
}

uint64_t Dynamic::ReadCursor() const
{
	return read_cursor_;
}

bool Dynamic::WriteGeneric(const void* data, const container_t::size_type size, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	RESULT_CONDITION_ENSURE_NOLOG(data, eResultErrorNullPtr, false);
	RESULT_CONDITION_ENSURE_NOLOG(size, eResultErrorZeroSize, false);
	data_.reserve(data_.size() + size);
	data_.insert(data_.cend(), static_cast<const byte_t*>(data), static_cast<const byte_t*>(data) + size);
	RESULT_OK();
	return true;
}

bool Dynamic::ReadGeneric(void* data, const container_t::size_type size, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	RESULT_CONDITION_ENSURE_NOLOG(data, eResultErrorNullPtr, false);
	RESULT_CONDITION_ENSURE_NOLOG(size, eResultErrorZeroSize, false);
	RESULT_CONDITION_ENSURE_NOLOG(read_cursor_ + size <= data_.size(), eResultErrorMemoryOutOfMemory, false);
	memcpy(data, data_.data() + read_cursor_, size);
	read_cursor_ += size;
	RESULT_OK();
	return true;
}

File::File(const char* file_path, const uint32_t flags, RESULT_PARAM_IMPL) : file_{file_path, flags, RESULT_ARG_PASS}
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_OK();
}

bool File::Open(const char* file_path, const uint32_t flags, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	RESULT_ENSURE_CALL_NOLOG(file_.Open(file_path, flags, RESULT_ARG_PASS), false);
	RESULT_OK();
	return true;
}

bool File::OpenWrite(const char* file_path, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	RESULT_ENSURE_CALL_NOLOG(file_.OpenWrite(file_path, RESULT_ARG_PASS), false);
	RESULT_OK();
	return true;
}

bool File::OpenRead(const char* file_path, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	RESULT_ENSURE_CALL_NOLOG(file_.OpenRead(file_path, RESULT_ARG_PASS), false);
	RESULT_OK();
	return true;
}

bool File::OpenReadWrite(const char* file_path, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	RESULT_ENSURE_CALL_NOLOG(file_.OpenReadWrite(file_path, RESULT_ARG_PASS), false);
	RESULT_OK();
	return true;
}

bool File::Flush(RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG(false);
	RESULT_ENSURE_CALL_NOLOG(file_.Flush(RESULT_ARG_PASS), false);
	RESULT_OK();
	return true;
}

uint64_t File::WriteCursor(RESULT_PARAM_IMPL) const
{
	return file_.Tell(RESULT_ARG_PASS);
}

uint64_t File::ReadCursor(RESULT_PARAM_IMPL) const
{
	return file_.Tell(RESULT_ARG_PASS);
}

bool File::WriteGeneric(const void* data, const size_t size, RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG(false);
	RESULT_ENSURE_CALL_NOLOG(file_.Write(data, size, RESULT_ARG_PASS), false);
	RESULT_OK();
	return true;
}

bool File::ReadGeneric(void* data, const size_t size, RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG(false);
	RESULT_ENSURE_CALL_NOLOG(file_.Read(data, size, RESULT_ARG_PASS), false);
	RESULT_OK();
	return true;
}

} // namespace Stream
