
#ifndef META_TYPE_INFO_H
#define META_TYPE_INFO_H

#include "Core/Common.h"
#include "Core/Allocator.h"
#include "Core/Hash.h"
#include "Core/Algorithm.h"
#include "Core/Assert.h"

#include <EASTL/span.h>
#include <EASTL/string_hash_map.h>
#include <EASTL/utility.h>

#if _MSC_VER
// Disable warning C4702: unreachable code 
#pragma warning (disable:4702)
#endif

namespace Meta
{

/**
 * @brief Type tag structure.
 *
 * @tparam T Target type.
 *
 */
template<typename T>
struct TypeTag
{
	using type_t = T;
};

using id_t = Hash::fnv1a_t;

/**
 * @brief Type info class.
 */
class TypeInfo
{
	friend class TypeRegistry;

public:
	/**
	 * @brief Operations for type info.
	 */
	enum OperationType
	{
		eDefaultCtor,
		eMoveCtor,
		eCopyCtor,
		eMoveAssign,
		eCopyAssign,
		eDtor,
		eToString,
		eMax
	};
	static constexpr char* OPERATION_TYPE_STRING[] = {(char*)"eDefaultCtor", (char*)"eMoveCtor",   (char*)"eCopyCtor",
													  (char*)"eMoveAssign",	 (char*)"eCopyAssign", (char*)"eDtor",
													  (char*)"eToString"};

	/**
	 * @brief Operator body structure.
	 *
	 * Used as argument when Rebinder<T>::Operation function is called.
	 */
	struct OperationBody
	{
		OperationType type;
		Uint64		  args_tuple_size;
		void*		  args_tuple;
	};
	using operation_function_t = void (*)(OperationBody&);

public:
	/**
	 * @brief Rebinder structure.
	 *
	 * @tparam T Target type.
	 *
	 */
	template<typename T>
	struct Rebinder;

	/**
	 * @brief Binder structure.
	 *
	 * @tparam T Target type.
	 *
	 */
	template<typename T>
	struct Binder
	{
		using type_t				 = T;
		static constexpr Uint64 SIZE = sizeof(T);
	};

private:
	TypeInfo();

	template<typename T>
	EXPLICIT TypeInfo(TypeTag<T>);

public:
	TypeInfo(const TypeInfo&)				 = delete;
	TypeInfo& operator=(const TypeInfo&)	 = delete;
	TypeInfo(TypeInfo&&) NOEXCEPT			 = default;
	TypeInfo& operator=(TypeInfo&&) NOEXCEPT = default;

public:
	~TypeInfo() = default;

public:
	NODISCARD Uint64	  Id() const;
	NODISCARD Uint64	  Size() const;
	NODISCARD const char* Name() const;

	NODISCARD eastl::string ToString(Uint64 capacity = 256) const;
	NODISCARD eastl::string ToValueString(void* value, Uint64 capacity = 256) const;

	NODISCARD static const TypeInfo& None();

public:
	NODISCARD bool IsValid() const;

public:
	EXPLICIT	   operator bool() const;
	NODISCARD bool operator==(const TypeInfo& value) const;
	NODISCARD bool operator!=(const TypeInfo& value) const;

public:
	NODISCARD bool IsBoolean() const;
	NODISCARD bool IsIntegral() const;
	NODISCARD bool IsFloatingPoint() const;
	NODISCARD bool IsConvertibleTo(const TypeInfo& value) const;

public:
	/**
	 * @brief Invoke operation function.
	 *
	 * Used to call Rebinder<T>::Operation function.
	 *
	 * @tparam ...Args
	 * @param type Operation type.
	 * @param ...args Arguments to be passed to operation function.
	 *
	 */
	template<typename... Args>
	void InvokeOperation(OperationType type, Args&&... args) const;

	/**
	 * @brief Invoke operation custom function.
	 *
	 * Used to call Rebinder<T>::Operation function.
	 *
	 * @tparam ...Args
	 * @param type External body.
	 * @param ...args Arguments to be passed to operation function.
	 *
	 */
	template<typename... Args>
	void InvokeOperation(OperationBody& body, Args&&... args) const;

private:
	id_t				 id_;
	Uint64				 size_;
	const char*			 name_;
	operation_function_t operation_function_;
};

template<typename T>
TypeInfo::TypeInfo(TypeTag<T>)
	: id_{Rebinder<T>::ID}, size_{Rebinder<T>::SIZE}, name_{Rebinder<T>::NAME.data()}, operation_function_{
																						   &Rebinder<T>::Operations}
{
	// printf("'%s' reflected: id=%llu, size=%llu\n", name_, id_, size_);
}

template<typename... Args>
void TypeInfo::InvokeOperation(const OperationType type, Args&&... args) const
{
	ENFORCE_MSG(operation_function_,
				"Operations function is marked as not implemented for '%s' type, and it is needed to properly "
				"apply the '%s' operation.",
				name_, OPERATION_TYPE_STRING[type]);

	auto		  l_args_tuple = eastl::make_tuple(eastl::forward<Args>(args)...);
	OperationBody l_body{type, sizeof...(Args), &l_args_tuple};
	operation_function_(l_body);
}

template<typename... Args>
void TypeInfo::InvokeOperation(OperationBody& body, Args&&... args) const
{
	ENFORCE_MSG(operation_function_,
				"Operations function is marked as not implemented for '%s' type and it is needed to properly "
				"apply the target operation.",
				name_);

	auto l_args_tuple	 = eastl::make_tuple(eastl::forward<Args>(args)...);
	body.args_tuple_size = sizeof...(Args);
	body.args_tuple		 = &l_args_tuple;
	operation_function_(body);
}

/**
 * @brief TypeRegister class.
 *
 */
class TypeRegistry
{
public:
	TypeRegistry();

	template<typename T>
	const TypeInfo& Emplace();
	const TypeInfo& Get(const char* name);
	const TypeInfo& Get(id_t id);
	bool			IsRegistered(const char* name) const;

	static TypeRegistry& Instance();

private:
	eastl::hash_map<id_t, TypeInfo> types_{DEBUG_NAME_VAL("Meta")};
	static TypeRegistry*			instance_;
};

template<typename T>
const TypeInfo& TypeRegistry::Emplace()
{
	auto l_it = types_.find(TypeInfo::Rebinder<T>::ID);
	if (l_it == types_.cend())
	{
		l_it = types_.emplace(TypeInfo::Rebinder<T>::ID, TypeInfo{TypeTag<T>{}}).first;
	}
	return l_it->second;
}

/**
 * @brief Typeof function.
 *
 * Gets registered type by cpp typename.
 *
 * @tparam T Target typename.
 * @return Pointer to TypeInfo.
 *
 */
template<typename T>
const TypeInfo& Typeof()
{
	return TypeRegistry::Instance().Emplace<T>();
}

/**
 * @brief Typeof from value function.
 *
 * Gets registered type by cpp typename.
 *
 * @tparam T Target typename.
 * @return TypeInfo.
 *
 */
template<typename T>
const TypeInfo& Typeof(const T&)
{
	return Typeof<T>();
}

/**
 * @brief Function typeof.
 *
 * Gets registered type by type.
 *
 * @param name Target type name.
 * @return TypeInfo.
 *
 */
NODISCARD const TypeInfo& Typeof(const char* name);

/**
 * @brief Function typeof.
 *
 * Gets registered type by type.
 *
 * @param id Hash id type.
 * @return TypeInfo.
 *
 */
NODISCARD const TypeInfo& Typeof(id_t id);

/**
 * @brief Registry base types.
 *
 * Call Typeof for base types.
 *
 */
void RegistryBaseTypes();

namespace Algorithm
{

/**
 * @brief Prints an array.
 *
 * @tparam T Target type.
 * @tparam Size Target size.
 * @param arr Target array.
 *
 */
template<typename T, Uint32 Size>
constexpr void PrintArray(const eastl::array<T, Size>& arr)
{
	printf("eastl::array<%s, %u>( ", TypeInfo::Rebinder<T>::NAME.data(), Size);
	::Algorithm::Detail::PrintArray<0>(arr);
	printf(")\n");
}

template<typename T>
eastl::string ToString(const T& value, const EASTLAllocatorType& allocator = {DEBUG_NAME_VAL("Algorithm")},
					   Uint64 str_size_hint = 512)
{
	if constexpr (eastl::is_fundamental_v<T>)
	{
		RawBuffer<char> l_buffer{str_size_hint, allocator};
		sprintf(l_buffer.Get(), "%s(%s)", TypeInfo::Rebinder<T>::NAME.data(), eastl::to_string(value).c_str());
		return eastl::string{l_buffer.Get(), allocator};
	}
	else if constexpr(is_coad_class<T>)
	{
		return value.ToString(str_size_hint);
	}
	ENFORCE_MSG(false, "Invalid to string implementation.");
	return eastl::string{allocator};
}

INLINE eastl::string ToString(const eastl::string_view& value,
							  const EASTLAllocatorType& allocator	  = {DEBUG_NAME_VAL("Algorithm")},
							  const Uint64				str_size_hint = 0)
{
	RawBuffer<char> l_buffer{value.size() + 32 + str_size_hint, allocator};
	sprintf(l_buffer.Get(), "eastl::string_view(%s)", value.data());
	return eastl::string{l_buffer.Get(), allocator};
}

template<typename T, typename Y>
eastl::string ToString(const eastl::pair<T, Y>&	 pair,
					   const EASTLAllocatorType& allocator	   = {DEBUG_NAME_VAL("Algorithm")},
					   const Uint64				 str_size_hint = 512)
{
	RawBuffer<char> l_buffer{str_size_hint + 32, allocator};
	sprintf(l_buffer.Get(), "eastl::pair<%s, %s>(%s, %s)", TypeInfo::Rebinder<T>::NAME.data(),
			TypeInfo::Rebinder<Y>::NAME.data(), ToString(pair.first, allocator).c_str(),
			ToString(pair.second, allocator).c_str());
	return eastl::string{l_buffer.Get(), allocator};
}

template<typename T>
eastl::string ToString(const eastl::vector<T>&	 vector,
					   const EASTLAllocatorType& allocator	   = {DEBUG_NAME_VAL("Algorithm")},
					   const Uint64				 str_size_hint = 0)
{
	if (vector.empty())
	{
		return eastl::string{allocator};
	}

	eastl::string l_str{allocator};
	l_str.reserve(vector.size() * 32);
	l_str += "eastl::vector<";
	l_str += TypeInfo::Rebinder<T>::NAME.data();
	l_str += ">(";
	for (const auto& l_v: vector)
	{
		l_str += ToString(l_v) + ',';
	}
	l_str.pop_back();
	l_str += ')';
	return l_str;
}

} // namespace Algorithm

namespace Detail
{

FORCEINLINE bool CompareTypeInfoArray(const TypeInfo* const* arr1, const TypeInfo* const* arr2, const Uint32 size)
{
	for (Uint32 i = 0; i < size; i++)
	{
		if (arr1[i] != arr2[i])
		{
			return false;
		}
	}
	return true;
}

} // namespace Detail

} // namespace Meta

#include "Meta/Macros.h"

// META_TYPE_BINDER_DEFAULT(void*)
META_TYPE_BINDER_DEFAULT(bool)
META_TYPE_BINDER_DEFAULT(Int8)
META_TYPE_BINDER_DEFAULT(Int16)
META_TYPE_BINDER_DEFAULT(Int32)
META_TYPE_BINDER_DEFAULT(Int64)
META_TYPE_BINDER_DEFAULT(Uint8)
META_TYPE_BINDER_DEFAULT(Uint16)
META_TYPE_BINDER_DEFAULT(Uint32)
META_TYPE_BINDER_DEFAULT(Uint64)
META_TYPE_BINDER_DEFAULT(Float32)
META_TYPE_BINDER_DEFAULT(Float64)
META_TYPE_BINDER_DEFAULT(eastl::string)
META_TYPE_BINDER_DEFAULT(eastl::string_view)
META_TYPE_BINDER_DEFAULT_TEMPLATE(eastl::pair, "eastl::pair<eastl::string_view, Int64>", eastl::string_view, Int64)
META_TYPE_BINDER_DEFAULT_TEMPLATE(eastl::vector, "eastl::vector<eastl::pair<eastl::string_view, Int64>>",
								  eastl::pair<eastl::string_view, Int64>)

// META_TYPE_BINDER_DEFAULT(glm::vec2)
// META_TYPE_BINDER_DEFAULT(glm::vec3)
// META_TYPE_BINDER_DEFAULT(glm::vec4)
// META_TYPE_BINDER_DEFAULT(glm::quat)
// META_TYPE_BINDER_DEFAULT(glm::mat4)

#endif
