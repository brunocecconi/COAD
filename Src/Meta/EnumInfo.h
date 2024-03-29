
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
	enum EFlags
	{
		eNone,
		eBitmask
	};

	template<typename EnumType>
	struct Rebinder;

	template<typename EnumType, uint32_t Flags = 0>
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
	NODISCARD const char* ToNameGeneric(int64_t Value) const;
	NODISCARD int64_t	  ToValueGeneric(const char* Name) const;

	template<typename T>
	NODISCARD const char* ToName(T Value) const;

	template<typename T>
	NODISCARD T ToValue(const char* Name) const;

public:
	NODISCARD bool IsBitmask() const;
	NODISCARD bool HasFlags(int64_t Value) const;

private:
	const TypeInfo&											mTypeInfo;
	eastl::vector<eastl::pair<eastl::string_view, int64_t>> mEntries{DEBUG_NAME_VAL("Meta")};
	uint64_t												mFlags;
};

template<typename T>
EnumInfo::EnumInfo(TypeTag<T>) : mTypeInfo{Typeof<typename Rebinder<T>::owner_t>()}, mFlags{Rebinder<T>::FLAGS}
{
	mEntries = Rebinder<T>::Pairs();
}

template<typename T>
const char* EnumInfo::ToName(const T Value) const
{
	static_assert(eastl::is_enum_v<T>, "Invalid enum type.");
	return ToNameGeneric(static_cast<int64_t>(Value));
}

template<typename T>
T EnumInfo::ToValue(const char* Name) const
{
	static_assert(eastl::is_enum_v<T>, "Invalid enum type.");
	return static_cast<T>(ToValueGeneric(Name));
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
	const EnumInfo& Get(const char* Name);
	const EnumInfo& Get(id_t Id);
	bool			IsRegistered(const char* Name) const;
	bool			IsRegistered(id_t Id) const;

	static EnumRegistry& Instance();

private:
	eastl::hash_map<id_t, EnumInfo> enums_{DEBUG_NAME_VAL("Meta")};
	static EnumRegistry*			instance_;
};

template<typename T>
const EnumInfo& EnumRegistry::Emplace()
{
	auto lIt = enums_.find(TypeInfo::Rebinder<T>::ID);
	if (lIt == enums_.cend())
	{
		lIt = enums_.emplace(TypeInfo::Rebinder<T>::ID, EnumInfo{TypeTag<T>{}}).first;
	}
	return lIt->second;
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

const EnumInfo& Enumof(const char* Name);

} // namespace Meta

#endif
