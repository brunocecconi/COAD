
#ifndef META_VALUE_H
#define META_VALUE_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Core/Assert.h"
#include "Meta/TypeInfo.h"

namespace Meta
{

/**
 * @brief Meta value.
 *
 * Works similar to a any class but with some differences and it is more simple.
 *
 * Features:
 * 1. Copy semantics are not supported, all members are mutable to ensure move semantics all the time.
 * 2. It is ensured internal memory if type size is less or equal 32 bytes. Otherwise,
 * @ref Value will behave like a unique_ptr.
 *
 */
class Value
{
public:
	Value();
	Value(void* memory, const TypeInfo& type_info);
	Value(const TypeInfo& type_info);

	template<typename T, typename = eastl::enable_if_t<!eastl::is_same_v<eastl::remove_reference_t<T>, Value> &&
													   eastl::is_move_assignable_v<T>>>
	Value(T&& value);

	template<typename T, typename = eastl::enable_if_t<!eastl::is_same_v<eastl::remove_reference_t<T>, Value> &&
													   eastl::is_copy_assignable_v<T>>>
	Value(const T& value);

	Value(Value&& other) NOEXCEPT;
	Value& operator=(Value&& other) NOEXCEPT;
	Value(const Value& other);
	Value& operator=(const Value& other);

	~Value();

public:
	template<typename T, typename = eastl::enable_if_t<!eastl::is_same_v<T, Value> && eastl::is_move_assignable_v<T>>>
	Value& operator=(T&& value);

	template<typename T, typename = eastl::enable_if_t<!eastl::is_same_v<T, Value> && eastl::is_copy_assignable_v<T>>>
	Value& operator=(const T& value);

public:
	NODISCARD const TypeInfo& Type() const;

public:
	template<typename T, typename = eastl::enable_if_t<!eastl::is_same_v<T, Value> && eastl::is_move_assignable_v<T>>>
	void Set(T&& value);

	template<typename T, typename = eastl::enable_if_t<!eastl::is_same_v<T, Value> && eastl::is_copy_assignable_v<T>>>
	void Set(const T& value);

	NODISCARD bool IsValid() const;

public:
	NODISCARD operator bool() const;

public:
	NODISCARD const void*	 AsGeneric() const;
	NODISCARD const bool&	 AsBool() const;
	NODISCARD const Int8&	 AsInt8() const;
	NODISCARD const Int16&	 AsInt16() const;
	NODISCARD const Int32&	 AsInt32() const;
	NODISCARD const Int64&	 AsInt64() const;
	NODISCARD const Uint8&	 AsUint8() const;
	NODISCARD const Uint16&	 AsUint16() const;
	NODISCARD const Uint32&	 AsUint32() const;
	NODISCARD const Uint64&	 AsUint64() const;
	NODISCARD const Float32& AsFloat32() const;
	NODISCARD const Float64& AsFloat64() const;

	/*NODISCARD const glm::vec2& AsVec2() const;
	NODISCARD const glm::vec3& AsVec3() const;
	NODISCARD const glm::vec4& AsVec4() const;
	NODISCARD const glm::quat& AsQuat() const;
	NODISCARD const glm::mat4& AsMat4() const;*/

	NODISCARD const eastl::string_view& AsStringView() const;

	template<typename T = char, typename Allocator = EASTLAllocatorType>
	NODISCARD const eastl::basic_string<T, Allocator>& AsString() const;

	template<typename T, typename Allocator = EASTLAllocatorType>
	NODISCARD const eastl::vector<T, Allocator>& AsVector() const;

	template<typename T>
	NODISCARD const T& As() const;

	NODISCARD eastl::string ToString() const;

public:
	void Reset();

private:
	template<typename T>
	T* GetStoragePtr() const;

private:
	mutable eastl::aligned_storage_t<sizeof(IntPtr) * 4, PLATFORM_ALIGNMENT> iptr_{};
	mutable TypeInfo*														 type_info_{};
	mutable void*															 eptr_{};
};

template<typename T, typename>
Value::Value(T&& value)
{
	Set(eastl::move(value));
}

template<typename T, typename>
Value::Value(const T& value)
{
	Set(value);
}

template<typename T, typename>
Value& Value::operator=(T&& value)
{
	Set(eastl::move(value));
	return *this;
}

template<typename T, typename>
Value& Value::operator=(const T& value)
{
	Set(value);
	return *this;
}

template<typename T, typename>
void Value::Set(T&& value)
{
	if constexpr (sizeof T <= sizeof iptr_)
	{
		*reinterpret_cast<T*>(iptr_.mCharData) = eastl::move(value);
	}
	else
	{
		if (type_info_ ? Typeof<T>() != *type_info_ : true)
		{
			Allocators::Default l_allocator{DEBUG_NAME_VAL("Meta")};
			if (eptr_)
			{
				l_allocator.deallocate(eptr_, type_info_->Size());
			}
			eptr_ = l_allocator.allocate(sizeof(T));
		}
		*static_cast<T*>(eptr_) = eastl::move(value);
	}
	type_info_ = const_cast<TypeInfo*>(&Typeof<T>());
}

template<typename T, typename>
void Value::Set(const T& value)
{
	if constexpr (sizeof T <= sizeof iptr_)
	{
		*reinterpret_cast<T*>(iptr_.mCharData) = value;
	}
	else
	{
		if (type_info_ ? Typeof<T>() != *type_info_ : true)
		{
			Allocators::Default l_allocator{DEBUG_NAME_VAL("Meta")};
			if (eptr_)
			{
				l_allocator.deallocate(eptr_, type_info_->Size());
			}
			eptr_ = l_allocator.allocate(sizeof(T));
		}
		*static_cast<T*>(eptr_) = value;
	}
	type_info_ = const_cast<TypeInfo*>(&Typeof<T>());
}

template<typename T, typename Allocator>
const eastl::basic_string<T, Allocator>& Value::AsString() const
{
	constexpr auto size = sizeof(eastl::vector<T, Allocator>);
	return size > sizeof iptr_ ? (*static_cast<eastl::basic_string<T, Allocator>*>(eptr_))
							   : *reinterpret_cast<eastl::basic_string<T, Allocator>*>((char*)iptr_.mCharData);
}

template<typename T, typename Allocator>
const eastl::vector<T, Allocator>& Value::AsVector() const
{
	constexpr auto size = sizeof(eastl::vector<T, Allocator>);
	return size > sizeof iptr_ ? (*static_cast<eastl::vector<T, Allocator>*>(eptr_))
							   : *reinterpret_cast<eastl::vector<T, Allocator>*>((char*)iptr_.mCharData);
}

template<typename T>
const T& Value::As() const
{
	ENFORCE_MSG(type_info_ != nullptr, "Invalid null type.");
	ENFORCE_MSG(*type_info_ == Typeof<T>(), "Incompatible types. Expected '%s' passed '%s'.", type_info_->Name(),
				TypeInfo::Rebinder<T>::NAME.data());
	return *GetStoragePtr<T>();
}

template<typename T>
T* Value::GetStoragePtr() const
{
	return sizeof(T) > sizeof iptr_ ? static_cast<T*>(eptr_) : reinterpret_cast<T*>(iptr_.mCharData);
}

} // namespace Meta

#endif
