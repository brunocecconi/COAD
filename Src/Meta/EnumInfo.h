
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
	friend class EnumRegistry;

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

	template<typename EnumType>
	struct Rebinder;

	template<typename EnumType, Uint32 Flags = 0>
	struct Binder
	{
		using owner_t				= EnumType;
		static constexpr auto FLAGS = Flags;
	};

private:
	EnumInfo();

	template<typename T>
	EXPLICIT EnumInfo(TypeTag<T>);

public:
	NODISCARD const TypeInfo& Type() const;
	NODISCARD eastl::string			 ToString() const;
	NODISCARD static const EnumInfo& None();

public:
	NODISCARD const char* ToNameGeneric(Int64 value) const;
	NODISCARD Int64		  ToValueGeneric(const char* name) const;

	template<typename T>
	NODISCARD const char* ToName(T value) const;

	template<typename T>
	NODISCARD T ToValue(const char* name) const;

public:
	NODISCARD bool IsBitmask() const;
	NODISCARD bool HasFlags(Int64 value) const;

private:
	const TypeInfo&										  type_info_;
	eastl::vector<eastl::pair<eastl::string_view, Int64>> entries_{DEBUG_NAME_VAL("Meta")};
	Uint64												  flags_;
};

template<typename T>
EnumInfo::EnumInfo(TypeTag<T>) : type_info_{Typeof<typename Rebinder<T>::owner_t>()}, flags_{Rebinder<T>::FLAGS}
{
	entries_ = Rebinder<T>::Pairs();
}

template<typename T>
const char* EnumInfo::ToName(const T value) const
{
	static_assert(eastl::is_enum_v<T>, "Invalid enum type.");
	return ToNameGeneric(static_cast<Int64>(value));
}

template<typename T>
T EnumInfo::ToValue(const char* name) const
{
	static_assert(eastl::is_enum_v<T>, "Invalid enum type.");
	return static_cast<T>(ToValueGeneric(name));
}

/**
 * @brief Enum register class.
 *
 */
class EnumRegistry
{
	EnumRegistry();

public:
	template<typename T>
	const EnumInfo& Emplace();
	const EnumInfo& Get(const char* name);
	const EnumInfo& Get(id_t id);
	bool			IsRegistered(const char* name) const;
	bool			IsRegistered(id_t id) const;

	static EnumRegistry& Instance();

private:
	eastl::hash_map<id_t, EnumInfo> enums_{DEBUG_NAME_VAL("Meta")};
	static EnumRegistry*					 instance_;
};

template<typename T>
const EnumInfo& EnumRegistry::Emplace()
{
	auto l_it = enums_.find(TypeInfo::Rebinder<T>::ID);
	if (l_it == enums_.cend())
	{
		l_it = enums_.emplace(TypeInfo::Rebinder<T>::ID, EnumInfo{TypeTag<T>{}}).first;
	}
	return l_it->second;
}

template<typename T>
const EnumInfo& Enumof()
{
	return EnumRegistry::Instance().Emplace<T>();
}

template<typename T>
const EnumInfo& Enumof(T)
{
	return EnumRegistry::Instance().Emplace<T>();
}

const EnumInfo& Enumof(const char* name);

} // namespace Meta

#endif
