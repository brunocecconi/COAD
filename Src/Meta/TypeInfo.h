
#ifndef META_TYPE_INFO_H
#define META_TYPE_INFO_H

#include "Core/Common.h"
#include "Core/Allocator.h"
#include "Core/Hash.h"
#include "Core/Algorithm.h"

#include <EASTL/span.h>
#include <EASTL/string_hash_map.h>
#include <EASTL/utility.h>

#include "Core/RawBuffer.h"

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
	Hash::fnv1a_t				 id_;
	Uint64				 size_;
	const char*			 name_;
	operation_function_t operation_function_;
};

template<typename T>
TypeInfo::TypeInfo(TypeTag<T>)
	: id_{Rebinder<T>::ID}, size_{Rebinder<T>::SIZE}, name_{Rebinder<T>::NAME.data()}, operation_function_{
																					&Rebinder<T>::Operations}
{
	//printf("'%s' reflected: id=%llu, size=%llu\n", name_, id_, size_);
}

template<typename... Args>
void TypeInfo::InvokeOperation(const OperationType type, Args&&... args) const
{
	auto		  l_args_tuple = eastl::make_tuple(eastl::forward<Args>(args)...);
	OperationBody l_body{type, sizeof...(Args), &l_args_tuple};
	operation_function_(l_body);
}

template<typename... Args>
void TypeInfo::InvokeOperation(OperationBody& body, Args&&... args) const
{
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
	const TypeInfo& Get(Hash::fnv1a_t id);
	bool			IsRegistered(const char* name) const;

	static TypeRegistry& Instance();

private:
	eastl::hash_map<Hash::fnv1a_t, TypeInfo> types_{DEBUG_NAME_VAL("Meta")};
	static TypeRegistry*					 instance_;
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
NODISCARD const TypeInfo& Typeof(Hash::fnv1a_t id);

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

template<typename T, typename StringAllocator = EASTLAllocatorType,
		 typename = eastl::enable_if_t<eastl::is_fundamental_v<T>>>
eastl::basic_string<char, StringAllocator> ToString(const T&			   value,
													const StringAllocator& allocator = {DEBUG_NAME_VAL("Algorithm")},
													Uint64				   str_size_hint = 512)
{
	RawBuffer<char> l_buffer{str_size_hint, allocator};
	sprintf(l_buffer.Get(), "%s(%s)", TypeInfo::Rebinder<T>::NAME.data(), eastl::to_string(value).c_str());
	return eastl::basic_string<char, StringAllocator>{l_buffer.Get(), allocator};
}

template<typename StringAllocator = EASTLAllocatorType>
eastl::basic_string<char, StringAllocator> ToString(const eastl::string_view& value,
													const StringAllocator&	  allocator = {DEBUG_NAME_VAL("Algorithm")},
													const Uint64			  str_size_hint = 0)
{
	RawBuffer<char> l_buffer{value.size() + 32 + str_size_hint, allocator};
	sprintf(l_buffer.Get(), "eastl::string_view(%s)", value.data());
	return eastl::basic_string<char, StringAllocator>{l_buffer.Get(), allocator};
}

template<typename T, typename Y, typename StringAllocator = EASTLAllocatorType>
eastl::basic_string<char, StringAllocator> ToString(const eastl::pair<T, Y>& pair,
													const StringAllocator&	 allocator = {DEBUG_NAME_VAL("Algorithm")},
													const Uint64			 str_size_hint = 512)
{
	RawBuffer<char> l_buffer{str_size_hint + 32, allocator};
	sprintf(l_buffer.Get(), "eastl::pair<%s, %s>(%s, %s)", TypeInfo::Rebinder<T>::NAME.data(), TypeInfo::Rebinder<Y>::NAME.data(),
			ToString(pair.first, allocator).c_str(), ToString(pair.second, allocator).c_str());
	return eastl::basic_string<char, StringAllocator>{l_buffer.Get(), allocator};
}

template<typename T, typename StringAllocator = EASTLAllocatorType, typename VectorAllocator = EASTLAllocatorType>
eastl::basic_string<char, StringAllocator> ToString(const eastl::vector<T, VectorAllocator>& vector,
													const StringAllocator& allocator = {DEBUG_NAME_VAL("Algorithm")},
													const Uint64		   str_size_hint = 0)
{
	if (vector.empty())
	{
		return eastl::basic_string<char, StringAllocator>{allocator};
	}

	eastl::basic_string<char, StringAllocator> l_str{allocator};
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

}

} // namespace Meta

#define META_REBINDER_TYPE_INFO()	\
	friend struct Meta::TypeInfo::Rebinder<this_t>

#define META_TYPE_BINDER_SPECIALIZATION(TYPE)                                                                          \
	template<>                                                                                                         \
	struct TypeInfo::Rebinder<TYPE>: TypeInfo::Binder<TYPE>

#define META_TYPE_BINDER_SPECIALIZATION_TEMPLATE(TYPE, ...)                                                            \
	template<>                                                                                                         \
	struct TypeInfo::Rebinder<TYPE<__VA_ARGS__>>: TypeInfo::Binder<TYPE<__VA_ARGS__>>

#define META_TYPE_BINDER_BODY(TYPE)                                                                                    \
	using this_t							 = TYPE;                                                                   \
	static constexpr Hash::fnv1a_t		ID	 = Hash::Fnv1AHash(#TYPE);                                                 \
	static constexpr eastl::string_view NAME = (char*)#TYPE;

#define META_TYPE_BINDER_BODY_TEMPLATE(TYPE, TYPE_NAME, ...)                                                           \
	using this_t							 = TYPE<__VA_ARGS__>;                                                      \
	static constexpr Hash::fnv1a_t		ID	 = Hash::Fnv1AHash(TYPE_NAME);                                             \
	static constexpr eastl::string_view NAME = (char*)TYPE_NAME;

#define META_TYPE_BINDER_OPERATIONS_CUSTOM(DEFAULT_CTOR, MOVE_CTOR, COPY_CTOR, MOVE_ASSIGN, COPY_ASSIGN, DTOR,         \
										   TO_STRING)                                                                  \
	static void Operations(TypeInfo::OperationBody& body)                                                              \
	{                                                                                                                  \
		auto& l_default_args_tuple = *static_cast<eastl::tuple<void*, void*>*>(body.args_tuple);                       \
		switch (body.type)                                                                                             \
		{                                                                                                              \
		case TypeInfo::eDefaultCtor: {                                                                                 \
			DEFAULT_CTOR                                                                                               \
			return;                                                                                                    \
		}                                                                                                              \
		case TypeInfo::eMoveCtor: {                                                                                    \
			MOVE_CTOR                                                                                                  \
			return;                                                                                                    \
		}                                                                                                              \
		case TypeInfo::eCopyCtor: {                                                                                    \
			COPY_CTOR                                                                                                  \
			return;                                                                                                    \
		}                                                                                                              \
		case TypeInfo::eMoveAssign: {                                                                                  \
			MOVE_ASSIGN                                                                                                \
			return;                                                                                                    \
		}                                                                                                              \
		case TypeInfo::eCopyAssign: {                                                                                  \
			COPY_ASSIGN                                                                                                \
			return;                                                                                                    \
		}                                                                                                              \
		case TypeInfo::eDtor: {                                                                                        \
			DTOR return;                                                                                               \
		}                                                                                                              \
		case TypeInfo::eToString: {                                                                                    \
			TO_STRING                                                                                                  \
			return;                                                                                                    \
		}                                                                                                              \
		}                                                                                                              \
	}

#define META_TYPE_BINDER_DEFAULT_OPERATION_DEFAULT_CTOR() new (eastl::get<0>(l_default_args_tuple)) this_t{};

#define META_TYPE_BINDER_DEFAULT_OPERATION_MOVE_CTOR()                                                                 \
	new (eastl::get<0>(l_default_args_tuple))                                                                          \
		this_t{eastl::move(*static_cast<this_t*>(eastl::get<1>(l_default_args_tuple)))};

#define META_TYPE_BINDER_DEFAULT_OPERATION_COPY_CTOR()                                                                 \
	new (eastl::get<0>(l_default_args_tuple)) this_t{*static_cast<this_t*>(eastl::get<1>(l_default_args_tuple))};

#define META_TYPE_BINDER_DEFAULT_OPERATION_MOVE_ASSIGN()                                                               \
	*static_cast<this_t*>(eastl::get<0>(l_default_args_tuple)) =                                                       \
		eastl::move(*static_cast<this_t*>(eastl::get<1>(l_default_args_tuple)));

#define META_TYPE_BINDER_DEFAULT_OPERATION_COPY_ASSIGN()                                                               \
	*static_cast<this_t*>(eastl::get<0>(l_default_args_tuple)) =                                                       \
		*static_cast<this_t*>(eastl::get<1>(l_default_args_tuple));

#define META_TYPE_BINDER_DEFAULT_OPERATION_DTOR()                                                                      \
	eastl::destroy_at(static_cast<this_t*>(eastl::get<0>(l_default_args_tuple)));

#define META_TYPE_BINDER_DEFAULT_OPERATION_TO_STRING()                                                                 \
	auto& l_to_string_args_tuple = *static_cast<eastl::tuple<eastl::string*, void*, Uint64>*>(body.args_tuple);        \
	*eastl::get<0>(l_to_string_args_tuple) =                                                                           \
		Algorithm::ToString(*static_cast<this_t*>(eastl::get<1>(l_to_string_args_tuple)), {DEBUG_NAME("Meta")},        \
							eastl::get<2>(l_to_string_args_tuple));

#define META_TYPE_BINDER_DEFAULT_OPERATIONS()                                                                          \
	META_TYPE_BINDER_OPERATIONS_CUSTOM(                                                                                \
		META_TYPE_BINDER_DEFAULT_OPERATION_DEFAULT_CTOR(), META_TYPE_BINDER_DEFAULT_OPERATION_MOVE_CTOR(),             \
		META_TYPE_BINDER_DEFAULT_OPERATION_COPY_CTOR(), META_TYPE_BINDER_DEFAULT_OPERATION_MOVE_ASSIGN(),              \
		META_TYPE_BINDER_DEFAULT_OPERATION_COPY_ASSIGN(), META_TYPE_BINDER_DEFAULT_OPERATION_DTOR(),                   \
		META_TYPE_BINDER_DEFAULT_OPERATION_TO_STRING())

#define META_TYPE_BINDER_DEFAULT(TYPE)                                                                                 \
	namespace Meta                                                                                                     \
	{                                                                                                                  \
	META_TYPE_BINDER_SPECIALIZATION(TYPE){META_TYPE_BINDER_BODY(TYPE) META_TYPE_BINDER_DEFAULT_OPERATIONS()};          \
	}

#define META_TYPE_BINDER_DEFAULT_NO_TO_STRING(TYPE)                                                                    \
	namespace Meta                                                                                                     \
	{                                                                                                                  \
	META_TYPE_BINDER_SPECIALIZATION(TYPE){META_TYPE_BINDER_BODY(TYPE) META_TYPE_BINDER_OPERATIONS_CUSTOM(              \
		META_TYPE_BINDER_DEFAULT_OPERATION_DEFAULT_CTOR(), META_TYPE_BINDER_DEFAULT_OPERATION_MOVE_CTOR(),             \
		META_TYPE_BINDER_DEFAULT_OPERATION_COPY_CTOR(), META_TYPE_BINDER_DEFAULT_OPERATION_MOVE_ASSIGN(),              \
		META_TYPE_BINDER_DEFAULT_OPERATION_COPY_ASSIGN(), META_TYPE_BINDER_DEFAULT_OPERATION_DTOR(), EMPTY)};          \
	}

#define META_TYPE_BINDER_DEFAULT_TEMPLATE(TYPE, TYPE_NAME, ...)                                                        \
	namespace Meta                                                                                                     \
	{                                                                                                                  \
	META_TYPE_BINDER_SPECIALIZATION_TEMPLATE(TYPE, __VA_ARGS__){                                                       \
		META_TYPE_BINDER_BODY_TEMPLATE(TYPE, TYPE_NAME, __VA_ARGS__) META_TYPE_BINDER_DEFAULT_OPERATIONS()};           \
	}

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
