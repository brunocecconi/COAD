
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
#pragma warning(disable : 4702)
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
		uint64_t	  args_tuple_size;
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
		using type_t				   = T;
		static constexpr uint64_t SIZE = sizeof(T);
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
	NODISCARD uint64_t	  Id() const;
	NODISCARD uint64_t	  Size() const;
	NODISCARD const char* Name() const;

	NODISCARD eastl::string ToString(uint64_t capacity = 256) const;
	NODISCARD eastl::string ToValueString(void* value, uint64_t capacity = 256) const;

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
	uint64_t			 size_;
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
 * @param Arr Target array.
 *
 */
template<typename T, uint32_t Size>
constexpr void PrintArray(const eastl::array<T, Size>& Arr)
{
	printf("eastl::array<%s, %u>( ", TypeInfo::Rebinder<T>::NAME.data(), Size);
	::Algorithm::Detail::PrintArray<0>(Arr);
	printf(")\n");
}

template<typename T>
eastl::string ToString(const T& Value, const EASTLAllocatorType& Allocator = {DEBUG_NAME_VAL("Algorithm")},
					   uint64_t StrSizeHint = 512)
{
	if constexpr (eastl::is_fundamental_v<T>)
	{
		RawBuffer<char> lBuffer{StrSizeHint, Allocator};
		sprintf(lBuffer.Data(), "%s(%s)", TypeInfo::Rebinder<T>::NAME.data(), eastl::to_string(Value).c_str());
		return eastl::string{lBuffer.Data(), Allocator};
	}
	else if constexpr (is_coad_class<T>)
	{
		return Value.ToString(StrSizeHint);
	}
	ENFORCE_MSG(false, "Invalid to string implementation.");
	return eastl::string{Allocator};
}

INLINE eastl::string ToString(const eastl::string_view& Value,
							  const EASTLAllocatorType& Allocator	= {DEBUG_NAME_VAL("Algorithm")},
							  const uint64_t			StrSizeHint = 0)
{
	RawBuffer<char> lBuffer{Value.size() + 32 + StrSizeHint, Allocator};
	sprintf(lBuffer.Data(), "eastl::string_view(%s)", Value.data());
	return eastl::string{lBuffer.Data(), Allocator};
}

template<typename T, typename Y>
eastl::string ToString(const eastl::pair<T, Y>&	 Pair,
					   const EASTLAllocatorType& Allocator	 = {DEBUG_NAME_VAL("Algorithm")},
					   const uint64_t			 StrSizeHint = 512)
{
	RawBuffer<char> lBuffer{StrSizeHint + 32, Allocator};
	sprintf(lBuffer.Data(), "eastl::pair<%s, %s>(%s, %s)", TypeInfo::Rebinder<T>::NAME.data(),
			TypeInfo::Rebinder<Y>::NAME.data(), ToString(Pair.first, Allocator).c_str(),
			ToString(Pair.second, Allocator).c_str());
	return eastl::string{lBuffer.Data(), Allocator};
}

template<typename T>
eastl::string ToString(const eastl::vector<T>&	 Vector,
					   const EASTLAllocatorType& Allocator	   = {DEBUG_NAME_VAL("Algorithm")},
					   const uint64_t			 StrSizeHint = 0)
{
	if (Vector.empty())
	{
		return eastl::string{Allocator};
	}

	eastl::string lStr{Allocator};
	lStr.reserve(Vector.size() * 32);
	lStr += "eastl::vector<";
	lStr += TypeInfo::Rebinder<T>::NAME.data();
	lStr += ">(";
	for (const auto& l_v: Vector)
	{
		lStr += ToString(l_v) + ',';
	}
	lStr.pop_back();
	lStr += ')';
	return lStr;
}

} // namespace Algorithm

namespace Detail
{

FORCEINLINE bool CompareTypeInfoArray(const TypeInfo* const* arr1, const TypeInfo* const* arr2, const uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
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
META_TYPE_BINDER_DEFAULT(int8_t)
META_TYPE_BINDER_DEFAULT(int16_t)
META_TYPE_BINDER_DEFAULT(int32_t)
META_TYPE_BINDER_DEFAULT(int64_t)
META_TYPE_BINDER_DEFAULT(uint8_t)
META_TYPE_BINDER_DEFAULT(uint16_t)
META_TYPE_BINDER_DEFAULT(uint32_t)
META_TYPE_BINDER_DEFAULT(uint64_t)
META_TYPE_BINDER_DEFAULT(float32_t)
META_TYPE_BINDER_DEFAULT(float64_t)
META_TYPE_BINDER_DEFAULT(eastl::string)
META_TYPE_BINDER_DEFAULT(eastl::string_view)
META_TYPE_BINDER_DEFAULT_TEMPLATE(eastl::pair, "eastl::pair<eastl::string_view, Int64>", eastl::string_view, int64_t)
META_TYPE_BINDER_DEFAULT_TEMPLATE(eastl::vector, "eastl::vector<eastl::pair<eastl::string_view, Int64>>",
								  eastl::pair<eastl::string_view, int64_t>)

// META_TYPE_BINDER_DEFAULT(glm::vec2)
// META_TYPE_BINDER_DEFAULT(glm::vec3)
// META_TYPE_BINDER_DEFAULT(glm::vec4)
// META_TYPE_BINDER_DEFAULT(glm::quat)
// META_TYPE_BINDER_DEFAULT(glm::mat4)

#endif
