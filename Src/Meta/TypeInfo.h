
#ifndef META_TYPE_INFO_H
#define META_TYPE_INFO_H

#include "Core/Common.h"

#include <EASTL/span.h>
#include <EASTL/string_hash_map.h>
#include <EASTL/utility.h>

namespace Meta
{

namespace Detail
{

template<Size... Idxs>
constexpr auto SubstringAsArray(const char* str, eastl::index_sequence<Idxs...>)
{
	return eastl::array{str[Idxs]..., '\0'};
}

template<typename T>
constexpr auto TypeNameArray()
{
#if defined(__clang__)
	constexpr auto prefix	= std::string_view{"[T = "};
	constexpr auto suffix	= std::string_view{"]"};
	constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(__GNUC__)
	constexpr auto prefix	= std::string_view{"with T = "};
	constexpr auto suffix	= std::string_view{"]"};
	constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(_MSC_VER)
	constexpr auto prefix	= std::string_view{"TypeNameArray<"};
	constexpr auto suffix	= std::string_view{">(void)"};
	constexpr auto function = std::string_view{__FUNCSIG__};
#else
#error Unsupported compiler
#endif

	constexpr auto start = function.find(prefix) + prefix.size();
	constexpr auto end	 = function.rfind(suffix);

	static_assert(start < end);

	constexpr auto name = function.substr(start, (end - start));
	return SubstringAsArray(name.data(), eastl::make_index_sequence<name.size()>{});
}

template<typename T>
struct TypeNameHolder
{
	static inline constexpr auto VALUE = TypeNameArray<T>();
};

template<typename T>
constexpr eastl::string_view TypeName()
{
	constexpr auto& value = Detail::TypeNameHolder<T>::VALUE;
	return eastl::string_view{value.data(), value.size()};
}

} // namespace Detail

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
	friend class TypeRegister;
	friend class eastl::string_hash_map<TypeInfo>;
	friend struct eastl::pair<const char* const, TypeInfo>;

private:
	TypeInfo();

	template<typename T>
	EXPLICIT TypeInfo(TypeTag<T>);

private:
	TypeInfo(const TypeInfo&)				 = default;
	TypeInfo& operator=(const TypeInfo&)	 = default;
	TypeInfo(TypeInfo&&) NOEXCEPT			 = default;
	TypeInfo& operator=(TypeInfo&&) NOEXCEPT = default;

public:
	~TypeInfo() = default;

public:
	NODISCARD Uint64	  Id() const;
	NODISCARD Uint64	  Size() const;
	NODISCARD const char* Name() const;
	NODISCARD eastl::string			 ToString() const;
	NODISCARD static const TypeInfo& None();

public:
	NODISCARD bool IsValid() const;

public:
	EXPLICIT	   operator bool() const;
	NODISCARD bool operator==(const TypeInfo& value) const;
	NODISCARD bool operator!=(const TypeInfo& value) const;

private:
	Uint64		id_;
	Uint64		size_;
	const char* name_;
};

/**
 * @brief TypeRegister class.
 *
 */
class TypeRegister
{
public:
	TypeRegister();

	template<typename T>
	const TypeInfo& Emplace();
	const TypeInfo& Get(const char* name);
	bool			IsRegistered(const char* name) const;

	static TypeRegister& Instance();

private:
	eastl::string_hash_map<TypeInfo> types_{NAME_VAL("Meta")};
	static TypeRegister*			 instance_;
};

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
	return TypeRegister::Instance().Emplace<T>();
}

/**
 * @brief Typeof from value function.
 *
 * Gets registered type by cpp typename.
 *
 * @tparam T Target typename.
 * @return Pointer to TypeInfo.
 *
 */
template<typename T>
const TypeInfo& Typeof(const T&)
{
	return TypeRegister::Instance().Emplace<T>();
}

/**
 * @brief Function typeof.
 *
 * Gets registered type by type.
 *
 * @param name Target type name.
 * @return Pointer to TypeInfo.
 *
 */
NODISCARD const TypeInfo& Typeof(const char* name);

/**
 * @brief Registry base types.
 *
 * Call Typeof for base types.
 *
 */
void RegistryBaseTypes();

}

#endif
