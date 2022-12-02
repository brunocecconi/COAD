
#ifndef CORE_STREAM_H
#define CORE_STREAM_H

#include "Core/Common.h"
#include "Core/Io.h"

namespace Stream
{

template<typename T>
struct Impl;

/**
 * @brief Stack stream.
 *
 * @tparam Size Target stream size.
 *
 */
template<Size Size>
class Static
{
public:
	using container_t = eastl::array<Uint8, Size>;

	static constexpr auto SIZE = Size;

public:
	Static()							 = default;
	Static(Static&&) NOEXCEPT			 = default;
	Static& operator=(Static&&) NOEXCEPT = default;
	Static(const Static&)				 = default;
	Static& operator=(const Static&)	 = default;
	~Static()							 = default;

public:
	NODISCARD typename container_t::value_type*		  Data() NOEXCEPT;
	NODISCARD const typename container_t::value_type* Data() const NOEXCEPT;

	NODISCARD typename container_t::reference		operator[](typename container_t::size_type index);
	NODISCARD typename container_t::const_reference operator[](typename container_t::size_type index) const;

	NODISCARD typename container_t::reference		At(typename container_t::size_type index);
	NODISCARD typename container_t::const_reference At(typename container_t::size_type index) const;

public:
	NODISCARD Uint64 WriteCursor() const;
	NODISCARD Uint64 ReadCursor() const;

public:
	MAYBEUNUSED bool WriteGeneric(const void* data, typename container_t::size_type size, RESULT_PARAM_DEFINE);
	MAYBEUNUSED bool ReadGeneric(void* data, typename container_t::size_type size, RESULT_PARAM_DEFINE);

	template<typename T>
	MAYBEUNUSED bool Write(const T* data, typename container_t::size_type size, RESULT_PARAM_DEFINE);
	template<typename T>
	MAYBEUNUSED bool Read(T* data, typename container_t::size_type size, RESULT_PARAM_DEFINE);

	template<typename T>
	MAYBEUNUSED bool Write(const T& data, RESULT_PARAM_DEFINE);
	template<typename T>
	MAYBEUNUSED bool Read(T& data, RESULT_PARAM_DEFINE);

public:
	template<typename T>
	Static& operator<<(const T& data);

	template<typename T>
	Static& operator>>(T& data);

private:
	container_t data_{};
	Uint64		write_cursor_{}, read_cursor_{};
};

/**
 * @brief Archive class.
 *
 * Used as memory stream to serialization/deserialization.
 *
 */
class Dynamic
{
public:
	using container_t = eastl::vector<Uint8>;

public:
	EXPLICIT Dynamic(const Allocators::Default& allocator = DEBUG_NAME_VAL("Memory::Stream"), RESULT_PARAM_DEFINE);
	EXPLICIT Dynamic(const void* data, container_t::size_type size,
					 const Allocators::Default& allocator = DEBUG_NAME_VAL("Memory::Stream"), RESULT_PARAM_DEFINE);
	EXPLICIT Dynamic(container_t::size_type		size,
					 const Allocators::Default& allocator = DEBUG_NAME_VAL("Memory::Stream"), RESULT_PARAM_DEFINE);
	EXPLICIT Dynamic(container_t&& container, const Allocators::Default& allocator = DEBUG_NAME_VAL("Memory::Stream"),
					 RESULT_PARAM_DEFINE);
	Dynamic(Dynamic&&) NOEXCEPT			   = default;
	Dynamic& operator=(Dynamic&&) NOEXCEPT = default;
	Dynamic(const Dynamic&)				   = default;
	Dynamic& operator=(const Dynamic&)	   = default;
	~Dynamic()							   = default;

public:
	NODISCARD bool BufferIsEmpty() const NOEXCEPT;
	NODISCARD container_t::size_type BufferSize() const NOEXCEPT;
	NODISCARD container_t::size_type BufferCapacity() const NOEXCEPT;

	void Resize(container_t::size_type size, const container_t::value_type& value, RESULT_PARAM_DEFINE);
	void Resize(container_t::size_type size, RESULT_PARAM_DEFINE);
	void Reserve(container_t::size_type size, RESULT_PARAM_DEFINE);
	void ShrinkToFit();

	NODISCARD container_t& Container();
	NODISCARD container_t::pointer Data() NOEXCEPT;
	NODISCARD container_t::const_pointer Data() const NOEXCEPT;

	NODISCARD container_t::reference operator[](container_t::size_type index);
	NODISCARD container_t::const_reference operator[](container_t::size_type index) const;

	NODISCARD container_t::reference At(container_t::size_type index);
	NODISCARD container_t::const_reference At(container_t::size_type index) const;

	void Clear() NOEXCEPT;

	NODISCARD bool Validate() const NOEXCEPT;

public:
	NODISCARD Uint64 WriteCursor() const;
	NODISCARD Uint64 ReadCursor() const;

public:
	MAYBEUNUSED bool WriteGeneric(const void* data, container_t::size_type size, RESULT_PARAM_DEFINE);
	MAYBEUNUSED bool ReadGeneric(void* data, container_t::size_type size, RESULT_PARAM_DEFINE);

	template<typename T>
	MAYBEUNUSED bool Write(const T* data, container_t::size_type size, RESULT_PARAM_DEFINE);
	template<typename T>
	MAYBEUNUSED bool Read(T* data, container_t::size_type size, RESULT_PARAM_DEFINE);

	template<typename T>
	MAYBEUNUSED bool Write(const T& data, RESULT_PARAM_DEFINE);
	template<typename T>
	MAYBEUNUSED bool Read(T& data, RESULT_PARAM_DEFINE);

public:
	template<typename T>
	Dynamic& operator<<(const T& data);

	template<typename T>
	Dynamic& operator>>(T& data);

private:
	container_t data_{};
	Uint64		read_cursor_{};
};

template<Size Size>
typename Static<Size>::container_t::value_type* Static<Size>::Data() NOEXCEPT
{
	return data_.data();
}

template<Size Size>
const typename Static<Size>::container_t::value_type* Static<Size>::Data() const NOEXCEPT
{
	return data_.data();
}

template<Size Size>
typename Static<Size>::container_t::reference Static<Size>::operator[](typename container_t::size_type index)
{
	return data_[index];
}

template<Size Size>
typename Static<Size>::container_t::const_reference Static<Size>::operator[](
	typename container_t::size_type index) const
{
	return data_[index];
}

template<Size Size>
typename Static<Size>::container_t::reference Static<Size>::At(typename container_t::size_type index)
{
	return data_.at(index);
}

template<Size Size>
typename Static<Size>::container_t::const_reference Static<Size>::At(typename container_t::size_type index) const
{
	return data_.at(index);
}

template<Size Size>
Uint64 Static<Size>::WriteCursor() const
{
	return write_cursor_;
}

template<Size Size>
Uint64 Static<Size>::ReadCursor() const
{
	return read_cursor_;
}

template<Size Size>
bool Static<Size>::WriteGeneric(const void* data, typename container_t::size_type size, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	if (write_cursor_ + size > Size)
	{
		RESULT_ERROR(eResultErrorMemoryOutOfBuffer, false);
	}
	memcpy(data_.data() + write_cursor_, data, size);
	write_cursor_ += size;
	RESULT_OK();
	return true;
}

template<Size Size>
bool Static<Size>::ReadGeneric(void* data, typename container_t::size_type size, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	if (read_cursor_ + size > Size)
	{
		RESULT_ERROR(eResultErrorMemoryOutOfBuffer, false);
	}
	memcpy(data, data_.data(), size);
	read_cursor_ += size;
	RESULT_OK();
	return true;
}

template<Size Size>
template<typename T>
bool Static<Size>::Write(const T* data, const typename container_t::size_type size, RESULT_PARAM_IMPL)
{
	const auto l_sizeof = sizeof T;
	UNUSED(l_sizeof);
	RESULT_CONDITION_ENSURE_NOLOG(WriteGeneric(static_cast<const void*>(&l_sizeof), sizeof l_sizeof, RESULT_ARG_PASS),
								  eResultErrorStreamFailedToWrite, false);
	RESULT_CONDITION_ENSURE_NOLOG(WriteGeneric(static_cast<const void*>(&size), sizeof size, RESULT_ARG_PASS),
								  eResultErrorStreamFailedToWrite, false);
	return Impl<T>::Write(*this, data, size, RESULT_ARG_PASS);
}

template<Size N>
template<typename T>
bool Static<N>::Read(T* data, const typename container_t::size_type size, RESULT_PARAM_IMPL)
{
	Size l_sizeof{}, l_size{};
	RESULT_CONDITION_ENSURE_NOLOG(ReadGeneric(static_cast<void*>(&l_sizeof), sizeof l_sizeof, RESULT_ARG_PASS),
								  eResultErrorStreamFailedToWrite, false);
	RESULT_CONDITION_ENSURE_NOLOG(l_sizeof == sizeof T, eResultErrorStreamFailedToWrite, false);
	RESULT_CONDITION_ENSURE_NOLOG(ReadGeneric(static_cast<void*>(&l_size), sizeof l_size, RESULT_ARG_PASS),
								  eResultErrorStreamFailedToWrite, false);
	RESULT_CONDITION_ENSURE_NOLOG(l_size == size, eResultErrorStreamFailedToWrite, false);
	return Impl<T>::Read(*this, data, size, RESULT_ARG_PASS);
}

template<Size Size>
template<typename T>
bool Static<Size>::Write(const T& data, RESULT_PARAM_IMPL)
{
	const auto l_sizeof = sizeof T;
	RESULT_CONDITION_ENSURE_NOLOG(WriteGeneric(static_cast<const void*>(&l_sizeof), sizeof l_sizeof, RESULT_ARG_PASS),
								  eResultErrorStreamFailedToWrite, false);
	return Impl<T>::Write(*this, data, RESULT_ARG_PASS);
}

template<Size N>
template<typename T>
bool Static<N>::Read(T& data, RESULT_PARAM_IMPL)
{
	Size l_sizeof{};
	RESULT_CONDITION_ENSURE_NOLOG(ReadGeneric(static_cast<void*>(&l_sizeof), sizeof l_sizeof, RESULT_ARG_PASS),
								  eResultErrorStreamFailedToWrite, false);
	RESULT_CONDITION_ENSURE_NOLOG(l_sizeof == sizeof T, eResultErrorStreamFailedToWrite, false);
	return Impl<T>::Read(*this, data, RESULT_ARG_PASS);
}

template<Size Size>
template<typename T>
Static<Size>& Static<Size>::operator<<(const T& data)
{
	Write(data);
	return *this;
}

template<Size Size>
template<typename T>
Static<Size>& Static<Size>::operator>>(T& data)
{
	Read(data);
	return *this;
}

template<typename T>
bool Dynamic::Write(const T* data, container_t::size_type size, RESULT_PARAM_IMPL)
{
	const auto l_sizeof = sizeof(T);
	UNUSED(l_sizeof);
	RESULT_CONDITION_ENSURE_NOLOG(WriteGeneric(&l_sizeof, sizeof l_sizeof, RESULT_ARG_PASS),
								  eResultErrorStreamFailedToWrite, false);
	RESULT_CONDITION_ENSURE_NOLOG(WriteGeneric(&size, sizeof size, RESULT_ARG_PASS), eResultErrorStreamFailedToWrite,
								  false);
	return Impl<T>::Write(*this, data, size, RESULT_ARG_PASS);
}

template<typename T>
bool Dynamic::Read(T* data, container_t::size_type size, RESULT_PARAM_IMPL)
{
	Size l_sizeof{}, l_size{};
	RESULT_CONDITION_ENSURE_NOLOG(ReadGeneric(&l_sizeof, sizeof l_sizeof, RESULT_ARG_PASS),
								  eResultErrorStreamFailedToRead, false);
	RESULT_CONDITION_ENSURE_NOLOG(l_sizeof == sizeof(T), eResultErrorStreamFailedToRead, false);
	RESULT_CONDITION_ENSURE_NOLOG(ReadGeneric(&l_size, sizeof l_size, RESULT_ARG_PASS), eResultErrorStreamFailedToRead,
								  false);
	RESULT_CONDITION_ENSURE_NOLOG(l_size == size, eResultErrorStreamFailedToRead, false);
	return Impl<T>::Read(*this, data, size, RESULT_ARG_PASS);
}

template<typename T>
bool Dynamic::Write(const T& data, RESULT_PARAM_IMPL)
{
	const auto l_sizeof = sizeof(T);
	RESULT_CONDITION_ENSURE_NOLOG(WriteGeneric(&l_sizeof, sizeof l_sizeof, RESULT_ARG_PASS),
								  eResultErrorStreamFailedToWrite, false);
	return Impl<T>::Write(*this, data, RESULT_ARG_PASS);
}

template<typename T>
bool Dynamic::Read(T& data, RESULT_PARAM_IMPL)
{
	Size l_sizeof{};
	RESULT_CONDITION_ENSURE_NOLOG(ReadGeneric(&l_sizeof, sizeof l_sizeof, RESULT_ARG_PASS),
								  eResultErrorStreamFailedToRead, false);
	RESULT_CONDITION_ENSURE_NOLOG(l_sizeof == sizeof(T), eResultErrorStreamFailedToRead, false);
	return Impl<T>::Read(*this, data, RESULT_ARG_PASS);
}

template<typename T>
Dynamic& Dynamic::operator<<(const T& data)
{
	Write(data);
	return *this;
}

template<typename T>
Dynamic& Dynamic::operator>>(T& data)
{
	Read(data);
	return *this;
}

/**
 * @brief File stream class.
 *
 */
class File
{
public:
	File() = default;
	File(const char* file_path, Uint32 flags, RESULT_PARAM_DEFINE);
	File(File&&) NOEXCEPT			 = default;
	File& operator=(File&&) NOEXCEPT = default;
	File(const File&)				 = delete;
	File& operator=(const File&)	 = delete;

public:
	MAYBEUNUSED bool Open(const char* file_path, Uint32 flags, RESULT_PARAM_DEFINE);
	MAYBEUNUSED bool OpenWrite(const char* file_path, RESULT_PARAM_DEFINE);
	MAYBEUNUSED bool OpenRead(const char* file_path, RESULT_PARAM_DEFINE);
	MAYBEUNUSED bool OpenReadWrite(const char* file_path, RESULT_PARAM_DEFINE);
	MAYBEUNUSED bool Flush(RESULT_PARAM_DEFINE) const;

public:
	NODISCARD Uint64 WriteCursor(RESULT_PARAM_DEFINE) const;
	NODISCARD Uint64 ReadCursor(RESULT_PARAM_DEFINE) const;

public:
	MAYBEUNUSED bool WriteGeneric(const void* data, Size size, RESULT_PARAM_DEFINE) const;
	MAYBEUNUSED bool ReadGeneric(void* data, Size size, RESULT_PARAM_DEFINE) const;

	template<typename T>
	MAYBEUNUSED bool Write(const T* data, Size size, RESULT_PARAM_DEFINE);
	template<typename T>
	MAYBEUNUSED bool Read(T* data, Size size, RESULT_PARAM_DEFINE);

	template<typename T>
	MAYBEUNUSED bool Write(const T& data, RESULT_PARAM_DEFINE);
	template<typename T>
	MAYBEUNUSED bool Read(T& data, RESULT_PARAM_DEFINE);

public:
	template<typename T>
	File& operator<<(const T& data);

	template<typename T>
	File& operator>>(T& data);

private:
	Io::File file_;
};

template<typename T>
bool File::Write(const T* data, Size size, RESULT_PARAM_IMPL)
{
	const auto l_sizeof = sizeof(T);
	UNUSED(l_sizeof);
	RESULT_CONDITION_ENSURE_NOLOG(WriteGeneric(&l_sizeof, sizeof l_sizeof, RESULT_ARG_PASS),
								  eResultErrorStreamFailedToWrite, false);
	RESULT_CONDITION_ENSURE_NOLOG(WriteGeneric(&size, sizeof size, RESULT_ARG_PASS), eResultErrorStreamFailedToWrite,
								  false);
	return Impl<T>::Write(*this, data, size, RESULT_ARG_PASS);
}

template<typename T>
bool File::Read(T* data, Size size, RESULT_PARAM_IMPL)
{
	Size l_sizeof{}, l_size{};
	RESULT_CONDITION_ENSURE_NOLOG(ReadGeneric(&l_sizeof, sizeof l_sizeof, RESULT_ARG_PASS),
								  eResultErrorStreamFailedToRead, false);
	RESULT_CONDITION_ENSURE_NOLOG(l_sizeof == sizeof(T), eResultErrorStreamFailedToRead, false);
	RESULT_CONDITION_ENSURE_NOLOG(ReadGeneric(&l_size, sizeof l_size, RESULT_ARG_PASS), eResultErrorStreamFailedToRead,
								  false);
	RESULT_CONDITION_ENSURE_NOLOG(l_size == size, eResultErrorStreamFailedToRead, false);
	return Impl<T>::Read(*this, data, size, RESULT_ARG_PASS);
}

template<typename T>
bool File::Write(const T& data, RESULT_PARAM_IMPL)
{
	const auto l_sizeof = sizeof(T);
	RESULT_CONDITION_ENSURE_NOLOG(WriteGeneric(&l_sizeof, sizeof l_sizeof, RESULT_ARG_PASS),
								  eResultErrorStreamFailedToWrite, false);
	return Impl<T>::Write(*this, data, RESULT_ARG_PASS);
}

template<typename T>
bool File::Read(T& data, RESULT_PARAM_IMPL)
{
	Size l_sizeof{};
	RESULT_CONDITION_ENSURE_NOLOG(ReadGeneric(&l_sizeof, sizeof l_sizeof, RESULT_ARG_PASS),
								  eResultErrorStreamFailedToRead, false);
	RESULT_CONDITION_ENSURE_NOLOG(l_sizeof == sizeof(T), eResultErrorStreamFailedToRead, false);
	return Impl<T>::Read(*this, data, RESULT_ARG_PASS);
}

template<typename T>
File& File::operator<<(const T& data)
{
	Write(data);
	return *this;
}

template<typename T>
File& File::operator>>(T& data)
{
	Read(data);
	return *this;
}

}

#define STREAM_IMPL_FRIEND() friend struct Stream::Impl<this_t>

#define STREAM_IMPL_BEGIN(TYPE)                                                                                        \
	template<>                                                                                                         \
	struct Stream::Impl<TYPE>                                                                                          \
	{                                                                                                                  \
		using this_t = TYPE;

#define STREAM_SIZEOF(N) static constexpr Size SIZE = N;

#define STREAM_WRITE_IMPL_VALUE_FUNCTION()                                                                             \
	template<typename StreamType>                                                                                      \
	static bool Write(StreamType& stream, const this_t& value, RESULT_PARAM_DEFINE)

#define STREAM_WRITE_IMPL_VALUE_FUNCTION_BEGIN()                                                                       \
	STREAM_WRITE_IMPL_VALUE_FUNCTION()                                                                                 \
	{                                                                                                                  \
		RESULT_ENSURE_LAST_NOLOG(false);

#define STREAM_WRITE_IMPL_ARRAY_FUNCTION()                                                                             \
	template<typename StreamType>                                                                                      \
	static bool Write(StreamType& stream, const this_t* value, const Size size, RESULT_PARAM_DEFINE)

#define STREAM_IMPL_END()                                                                                              \
	}                                                                                                                  \
	;

#define STREAM_READ_IMPL_VALUE_FUNCTION()                                                                              \
	template<typename StreamType>                                                                                      \
	static bool Read(StreamType& stream, this_t& value, RESULT_PARAM_DEFINE)

#define STREAM_READ_IMPL_VALUE_FUNCTION_BEGIN()                                                                        \
	STREAM_READ_IMPL_VALUE_FUNCTION()                                                                                  \
	{                                                                                                                  \
		RESULT_ENSURE_LAST_NOLOG(false);

#define STREAM_IMPL_VALUE_FUNCTION_END()                                                                               \
	RESULT_OK();                                                                                                       \
	return true;                                                                                                       \
	}

#define STREAM_READ_IMPL_ARRAY_FUNCTION()                                                                              \
	template<typename StreamType>                                                                                      \
	static bool Read(StreamType& stream, this_t* value, const Size size, RESULT_PARAM_DEFINE)

#define STREAM_IMPL_POD(TYPE)                                                                                          \
	STREAM_IMPL_BEGIN(TYPE)                                                                                            \
	STREAM_WRITE_IMPL_VALUE_FUNCTION()                                                                                 \
	{                                                                                                                  \
		static_assert(eastl::is_pod_v<this_t>, "Invalid plain old data type.");                                        \
		RESULT_ENSURE_LAST_NOLOG(false);                                                                               \
		RESULT_CONDITION_ENSURE_NOLOG(stream.Write((const void*)&value, sizeof(this_t), RESULT_ARG_PASS),              \
									  eResultErrorStreamFailedToWrite, false);                                         \
		RESULT_OK();                                                                                                   \
		return true;                                                                                                   \
	}                                                                                                                  \
	STREAM_WRITE_IMPL_ARRAY_FUNCTION()                                                                                 \
	{                                                                                                                  \
		static_assert(eastl::is_pod_v<this_t>, "Invalid plain old data type.");                                        \
		RESULT_ENSURE_LAST_NOLOG(false);                                                                               \
		const auto l_sizeof = sizeof this_t;                                                                           \
		UNUSED(l_sizeof);                                                                                              \
		RESULT_CONDITION_ENSURE_NOLOG(stream.WriteGeneric((const void*)&l_sizeof, sizeof l_sizeof, RESULT_ARG_PASS),   \
									  eResultErrorStreamFailedToWrite, false);                                         \
		RESULT_CONDITION_ENSURE_NOLOG(stream.WriteGeneric((const void*)&size, sizeof size, RESULT_ARG_PASS),           \
									  eResultErrorStreamFailedToWrite, false);                                         \
		RESULT_CONDITION_ENSURE_NOLOG(stream.WriteGeneric((const void*)value, l_sizeof* size, RESULT_ARG_PASS),        \
									  eResultErrorStreamFailedToWrite, false);                                         \
		RESULT_OK();                                                                                                   \
		return true;                                                                                                   \
	}                                                                                                                  \
	STREAM_READ_IMPL_VALUE_FUNCTION()                                                                                  \
	{                                                                                                                  \
		static_assert(eastl::is_pod_v<this_t>, "Invalid plain old data type.");                                        \
		RESULT_ENSURE_LAST_NOLOG(false);                                                                               \
		Size l_sizeof{};                                                                                               \
		RESULT_CONDITION_ENSURE_NOLOG(stream.ReadGeneric(&l_sizeof, sizeof(l_sizeof), RESULT_ARG_PASS),                \
									  eResultErrorStreamFailedToRead, false);                                          \
		RESULT_CONDITION_ENSURE_NOLOG(l_sizeof == sizeof this_t, eResultErrorStreamFailedToRead, false);               \
		RESULT_CONDITION_ENSURE_NOLOG(stream.ReadGeneric((void*)&value, sizeof(this_t), RESULT_ARG_PASS),              \
									  eResultErrorStreamFailedToRead, false);                                          \
		RESULT_OK();                                                                                                   \
		return true;                                                                                                   \
	}                                                                                                                  \
	STREAM_READ_IMPL_ARRAY_FUNCTION()                                                                                  \
	{                                                                                                                  \
		static_assert(eastl::is_pod_v<this_t>, "Invalid plain old data type.");                                        \
		RESULT_ENSURE_LAST_NOLOG(false);                                                                               \
		Size l_sizeof{}, l_size{};                                                                                     \
		RESULT_CONDITION_ENSURE_NOLOG(stream.ReadGeneric((void*)&l_sizeof, sizeof l_sizeof, RESULT_ARG_PASS),          \
									  eResultErrorStreamFailedToRead, false);                                          \
		RESULT_CONDITION_ENSURE_NOLOG(stream.ReadGeneric((void*)&l_size, sizeof l_size, RESULT_ARG_PASS),              \
									  eResultErrorStreamFailedToRead, false);                                          \
		RESULT_CONDITION_ENSURE_NOLOG(stream.ReadGeneric((void*)value, l_sizeof* l_size, RESULT_ARG_PASS),             \
									  eResultErrorStreamFailedToRead, false);                                          \
		RESULT_OK();                                                                                                   \
		return true;                                                                                                   \
	}                                                                                                                  \
	STREAM_IMPL_END()

STREAM_IMPL_POD(bool)
STREAM_IMPL_POD(char)
STREAM_IMPL_POD(Int8)
STREAM_IMPL_POD(Int16)
STREAM_IMPL_POD(Int32)
STREAM_IMPL_POD(Int64)
STREAM_IMPL_POD(Uint8)
STREAM_IMPL_POD(Uint16)
STREAM_IMPL_POD(Uint32)
STREAM_IMPL_POD(Uint64)
STREAM_IMPL_POD(Float32)
STREAM_IMPL_POD(Float64)

namespace Stream
{

template<typename... Types>
struct Impl<eastl::vector<Types...>>
{
	using this_t = eastl::vector<Types...>;

	STREAM_WRITE_IMPL_VALUE_FUNCTION()
	{
		RESULT_ENSURE_LAST_NOLOG(false);
		const auto l_sizeof = this_t::value_type;
		if (!stream.Write(&l_sizeof, sizeof l_sizeof, RESULT_ARG_PASS))
		{
			RESULT_ERROR(eResultErrorStreamFailedToWrite, false);
		}
		const auto l_size = value.size();
		if (!stream.Write(&l_size, sizeof l_size, RESULT_ARG_PASS))
		{
			RESULT_ERROR(eResultErrorStreamFailedToWrite, false);
		}
		if (!stream.Write(value.data(), l_size * l_sizeof, RESULT_ARG_PASS))
		{
			RESULT_ERROR(eResultErrorStreamFailedToWrite, false);
		}
		RESULT_OK();
		return true;
	}
	STREAM_READ_IMPL_VALUE_FUNCTION()
	{
		RESULT_ENSURE_LAST_NOLOG(false);
		typename this_t::size_type l_sizeof{}, l_size{};
		if (!stream.Read(&l_sizeof, sizeof l_sizeof, RESULT_ARG_PASS))
		{
			RESULT_ERROR(eResultErrorStreamFailedToRead, false);
		}
		RESULT_CONDITION_ENSURE_NOLOG(l_sizeof > 0, eResultErrorZeroSize, false);
		RESULT_CONDITION_ENSURE_NOLOG(l_sizeof == sizeof this_t::value_type, eResultErrorStreamInvalidSizes, false);
		if (!stream.Read(&l_size, sizeof l_size, RESULT_ARG_PASS))
		{
			RESULT_ERROR(eResultErrorStreamFailedToRead, false);
		}
		RESULT_CONDITION_ENSURE_NOLOG(l_size > 0, eResultErrorZeroSize, false);
		value.resize(l_size);
		if (!stream.Read(&value.data(), l_size * l_sizeof, RESULT_ARG_PASS))
		{
			RESULT_ERROR(eResultErrorStreamFailedToRead, false);
		}
		RESULT_OK();
		return true;
	}
};

template<typename... Types>
struct Impl<eastl::basic_string<Types...>>
{
	using this_t = eastl::basic_string<Types...>;

	STREAM_WRITE_IMPL_VALUE_FUNCTION()
	{
		RESULT_ENSURE_LAST_NOLOG(false);
		const auto l_sizeof = sizeof this_t::value_type;
		if (!stream.Write(&l_sizeof, sizeof l_sizeof, RESULT_ARG_PASS))
		{
			RESULT_ERROR(eResultErrorStreamFailedToWrite, false);
		}
		const auto l_size = value.size();
		if (!stream.Write(&l_size, sizeof l_size, RESULT_ARG_PASS))
		{
			RESULT_ERROR(eResultErrorStreamFailedToWrite, false);
		}
		if (!stream.Write(value.data(), l_size * l_sizeof, RESULT_ARG_PASS))
		{
			RESULT_ERROR(eResultErrorStreamFailedToWrite, false);
		}
		RESULT_OK();
		return true;
	}
	STREAM_READ_IMPL_VALUE_FUNCTION()
	{
		RESULT_ENSURE_LAST_NOLOG(false);
		typename this_t::size_type l_sizeof{}, l_size{};
		if (!stream.Read(&l_sizeof, sizeof l_sizeof, RESULT_ARG_PASS))
		{
			RESULT_ERROR(eResultErrorStreamFailedToRead, false);
		}
		RESULT_CONDITION_ENSURE_NOLOG(l_sizeof > 0, eResultErrorZeroSize, false);
		RESULT_CONDITION_ENSURE_NOLOG(l_sizeof == sizeof this_t::value_type, eResultErrorStreamInvalidSizes, false);
		if (!stream.Read(&l_size, sizeof l_size, RESULT_ARG_PASS))
		{
			RESULT_ERROR(eResultErrorStreamFailedToRead, false);
		}
		RESULT_CONDITION_ENSURE_NOLOG(l_size > 0, eResultErrorZeroSize, false);
		value.resize(l_size);
		if (!stream.Read(value.data(), l_size * l_sizeof, RESULT_ARG_PASS))
		{
			RESULT_ERROR(eResultErrorStreamFailedToRead, false);
		}
		RESULT_OK();
		return true;
	}
};

template<typename... Types>
struct Impl<eastl::hash_map<Types...>>
{
	using this_t = eastl::hash_map<Types...>;

	STREAM_WRITE_IMPL_VALUE_FUNCTION()
	{
		RESULT_ENSURE_LAST_NOLOG(false);
		const auto l_size = value.size();
		if (!stream.Write(&l_size, sizeof l_size, RESULT_ARG_PASS))
		{
			RESULT_ERROR(eResultErrorStreamFailedToWrite, false);
		}

		for (const auto& l_pair: value)
		{
			if (!stream.Write(&l_pair, sizeof this_t::value_type, RESULT_ARG_PASS))
			{
				RESULT_ERROR(eResultErrorStreamFailedToWrite, false);
			}
		}
		RESULT_OK();
		return true;
	}
	STREAM_READ_IMPL_VALUE_FUNCTION()
	{
		RESULT_ENSURE_LAST_NOLOG(false);
		typename this_t::size_type l_size{};
		if (!stream.Read(&l_size, sizeof l_size, RESULT_ARG_PASS))
		{
			RESULT_ERROR(eResultErrorStreamFailedToRead, false);
		}
		RESULT_CONDITION_ENSURE_NOLOG(l_size > 0, eResultErrorZeroSize, false);
		value.reserve(l_size);
		for (typename this_t::size_type pair_index = 0ull; pair_index < l_size; ++pair_index)
		{
			typename this_t::value_type l_value;
			if (!stream.Read(&value, RESULT_ARG_PASS))
			{
				RESULT_ERROR(eResultErrorStreamFailedToRead, false);
			}
			value.emplace(l_value);
		}
		RESULT_OK();
		return true;
	}
};

template<typename T>
struct Impl<eastl::basic_string_view<T>>
{
	using this_t = eastl::basic_string_view<T>;

	STREAM_WRITE_IMPL_VALUE_FUNCTION()
	{
		RESULT_ENSURE_LAST_NOLOG(false);
		const auto l_sizeof = this_t::value_type;
		if (!stream.Write(&l_sizeof, sizeof l_sizeof, RESULT_ARG_PASS))
		{
			RESULT_ERROR(eResultErrorStreamFailedToWrite, false);
		}
		const auto l_size = value.size();
		if (!stream.Write(&l_size, sizeof l_size, RESULT_ARG_PASS))
		{
			RESULT_ERROR(eResultErrorStreamFailedToWrite, false);
		}
		if (!stream.Write(value.data(), l_size * l_sizeof, RESULT_ARG_PASS))
		{
			RESULT_ERROR(eResultErrorStreamFailedToWrite, false);
		}
		RESULT_OK();
		return true;
	}
};

template<typename First, typename Second>
struct Impl<eastl::pair<First, Second>>
{
	using this_t = eastl::pair<First, Second>;

	STREAM_WRITE_IMPL_VALUE_FUNCTION()
	{
		RESULT_ENSURE_LAST_NOLOG(false);
		if (!stream.Write(&value.first, RESULT_ARG_PASS))
		{
			RESULT_ERROR(eResultErrorStreamFailedToWrite, false);
		}
		if (!stream.Write(&value.second, RESULT_ARG_PASS))
		{
			RESULT_ERROR(eResultErrorStreamFailedToWrite, false);
		}
		RESULT_OK();
		return true;
	}
	STREAM_READ_IMPL_VALUE_FUNCTION()
	{
		RESULT_ENSURE_LAST_NOLOG(false);
		if (!stream.Read(&value.first, RESULT_ARG_PASS))
		{
			RESULT_ERROR(eResultErrorStreamFailedToWrite, false);
		}
		if (!stream.Read(&value.second, RESULT_ARG_PASS))
		{
			RESULT_ERROR(eResultErrorStreamFailedToWrite, false);
		}
		RESULT_OK();
		return true;
	}
};

}

#endif
