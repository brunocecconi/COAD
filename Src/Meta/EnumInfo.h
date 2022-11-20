
#ifndef META_ENUM_INFO_H
#define META_ENUM_INFO_H

#include "Meta/TypeInfo.h"

namespace Meta
{

/**
 * @brief Enum info class.
 *
*/
class EnumInfo
{
	friend class TypeRegister;
	friend class eastl::string_hash_map<TypeInfo>;
	friend struct eastl::pair<const char* const, TypeInfo>;

public:
	/**
	 * @brief Enum info flags.
	 *
	*/
	enum Flags
	{
		eNone,
		eBitmask
	};

	template<typename EnumType, Uint32 Flags = 0>
	struct Binder
	{
		using owner_t				= EnumType;
		static constexpr auto FLAGS = Flags;
	};

private:
	EnumInfo();

	template<typename BinderType>
	EXPLICIT EnumInfo(BinderType) : type_info_{Typeof<typename BinderType::owner_t>()}, flags_{BinderType::FLAGS}
	{
		entries_ = BinderType::GetEntries();
	}

public:
	NODISCARD const TypeInfo&		 Type() const;
	NODISCARD static const EnumInfo& None();

public:
	const char* ToNameGeneric(Int64 value);
	Int64		ToValueGeneric(const char* name);

	template<typename T>
	const char* ToName(T value);

	template<typename T>
	T ToValue(const char* name);

public:
	NODISCARD bool IsBitmask() const;
	NODISCARD bool HasFlags(Int64 value) const;

private:
	const TypeInfo&										  type_info_;
	eastl::vector<eastl::pair<eastl::string_view, Int64>> entries_{NAME_VAL("Meta")};
	Uint64												  flags_;
};

template<typename T>
const char* EnumInfo::ToName(const T value)
{
	static_assert(eastl::is_enum_v<T>, "Invalid enum type.");
	return ToNameGeneric(static_cast<Int64>(value));
}

template<typename T>
T EnumInfo::ToValue(const char* name)
{
	static_assert(eastl::is_enum_v<T>, "Invalid enum type.");
	return static_cast<T>(ToValueGeneric(name));
}

/**
 * @brief Enum register class.
 *
*/
class EnumRegister
{
	EnumRegister();

public:
	template<typename T>
	const EnumInfo& Emplace()
	{
		constexpr auto l_type_name = Detail::TypeName<T>();
		auto		   l_it		   = enums_.find(l_type_name.data());
		if (l_it == enums_.cend())
		{
			l_it = enums_.emplace(l_type_name.data(), EnumInfo{TypeTag<T>{}}).first;
		}
		return l_it->second;
	}
	const EnumInfo& Get(const char* name);
	bool			IsRegistered(const char* name) const;

	static EnumRegister& Instance();

private:
	eastl::string_hash_map<EnumInfo> enums_{NAME_VAL("Meta")};
	static EnumRegister*			 instance_;
};

template<typename T>
const EnumInfo& Enumof()
{
	return EnumRegister::Instance().Emplace<T>();
}

template<typename T>
const EnumInfo& Enumof(T)
{
	return EnumRegister::Instance().Emplace<T>();
}

const EnumInfo& Enumof(const char* name);

}

#endif
