
#ifndef META_PROPERTY_INFO_H
#define META_PROPERTY_INFO_H

#include "Meta/TypeInfo.h"
#include "Meta/Value.h"

namespace Meta
{

/**
 * @brief Property info class.
 *
 */
class PropertyInfo
{
public:
	/**
	 * @brief Flags
	 *
	 */
	enum Flags
	{
		eNone,
		eReadOnly,
		eWriteOnly,
		eReadWrite
	};

	/**
	 * @brief Body structure.
	 *
	 * Used to call rebinder getters and setters.
	 *
	 */
	struct Body
	{
		void* info;
		void* owner;
		void* value;
	};

	/**
	 * @brief Rebinder structure.
	 *
	 * Used as child of @ref Binder and extract property info.
	 *
	 * @tparam Id Target id.
	 *
	 */
	template<Hash::fnv1a_t Id>
	struct Rebinder;

	/**
	 * @brief Binder structure.
	 *
	 * Used as parent to extract property info.
	 *
	 * @tparam Owner
	 * @tparam T
	 * @tparam Flags
	 * @tparam Id
	 */
	template<Hash::fnv1a_t Id, typename Owner, typename T, Uint32 Flags = 0>
	struct Binder
	{
		using owner_t	   = Owner;
		using type_t	   = T;
		using member_ptr_t = T(Owner::*);
		using body_t	   = Body;

		static constexpr auto ID	= Id;
		static constexpr auto FLAGS = Flags;
	};
	using binder_set_function_t = void (*)(const Body&);
	using binder_get_function_t = void* (*)(const Body&);

public:
	template<typename RebinderType>
	EXPLICIT PropertyInfo(RebinderType);

public:
	/**
	 * @brief Set property value.
	 *
	 * @param owner Target owner class generic object.
	 * @param value Target meta value.
	 *
	 */
	void Set(void* owner, Value value) const;

	/**
	 * @brief Get property value.
	 *
	 * @param owner Target owner class generic object.
	 *
	 * @return Target meta value.
	 *
	 */
	NODISCARD Value Get(const void* owner) const;

public:
	NODISCARD const TypeInfo& OwnerType() const;
	NODISCARD const TypeInfo& Type() const;
	NODISCARD const char*	  Name() const;
	NODISCARD Hash::fnv1a_t Id() const;
	NODISCARD Uint32		Flags() const;
	NODISCARD eastl::string ToString(Uint64 capacity = 256) const;

private:
	const TypeInfo&		  owner_type_info_;
	const TypeInfo&		  type_info_;
	const char*			  name_;
	Hash::fnv1a_t		  id_;
	Uint32				  flags_;
	binder_set_function_t set_function_;
	binder_get_function_t get_function_;
};

// template<typename Owner, typename T, Uint32 Flags>
// template<Size N>
// constexpr PropertyInfo::Binder<Owner, T, Flags>::Binder(const char (&name)[N]) : name_{name}
//{
// }
//
// template<typename Owner, typename T, Uint32 Flags>
// constexpr const char* PropertyInfo::Binder<Owner, T, Flags>::Name() const
//{
//	return name_;
// }

template<typename RebinderType>
PropertyInfo::PropertyInfo(RebinderType)
	: owner_type_info_{Typeof<typename RebinderType::owner_t>()}, type_info_{Typeof<typename RebinderType::type_t>()},
	  name_{RebinderType::NAME}, id_{RebinderType::ID}, flags_{RebinderType::FLAGS}, set_function_{&RebinderType::Set},
	  get_function_{&RebinderType::Get}
{
}

} // namespace Meta

#define META_REBINDER_PROPERTY(OWNER, PROPERTY)                                                                        \
public:                                                                                                                \
	static constexpr auto Property_##PROPERTY##_ID = #OWNER "::" #PROPERTY##_fnv1a;                                    \
                                                                                                                       \
private:                                                                                                               \
	friend struct Meta::PropertyInfo::Rebinder<Property_##PROPERTY##_ID>

#define META_PROPERTY_INFO_BINDER(OWNER, TYPE, NAME_SYMBOL, FLAGS)                                                     \
	template<>                                                                                                         \
	struct Meta::PropertyInfo::Rebinder<OWNER::Property_##NAME_SYMBOL##_ID>                                            \
		: Binder<OWNER::Property_##NAME_SYMBOL##_ID, OWNER, TYPE, FLAGS>

#define META_PROPERTY_INFO_BINDER_BODY(OWNER, NAME_SYMBOL)                                                             \
	static constexpr char NAME[64]	 = {#OWNER "::" #NAME_SYMBOL};                                                     \
	static constexpr auto MEMBER_PTR = &OWNER::NAME_SYMBOL;

#define META_PROPERTY_INFO_BINDER_SET_CUSTOM(BODY)                                                                     \
	static void Set(const body_t& body)                                                                                \
	{                                                                                                                  \
		if constexpr (FLAGS & (Meta::PropertyInfo::eWriteOnly | Meta::PropertyInfo::eReadWrite))                       \
		{                                                                                                              \
			BODY                                                                                                       \
		}                                                                                                              \
	}

#define META_PROPERTY_INFO_BINDER_GET_CUSTOM(BODY)                                                                     \
	static void* Get(const body_t& body)                                                                               \
	{                                                                                                                  \
		if constexpr (FLAGS & (Meta::PropertyInfo::eReadOnly | Meta::PropertyInfo::eReadWrite))                        \
		{                                                                                                              \
			BODY                                                                                                       \
		}                                                                                                              \
		else                                                                                                           \
		{                                                                                                              \
			return nullptr;                                                                                            \
		}                                                                                                              \
	}

#define META_PROPERTY_INFO_BINDER_SET_DEFAULT()                                                                        \
	META_PROPERTY_INFO_BINDER_SET_CUSTOM(                                                                              \
		if constexpr (FLAGS & (Meta::PropertyInfo::eWriteOnly | Meta::PropertyInfo::eReadWrite)) {                     \
			static_cast<owner_t*>(body.owner)->*MEMBER_PTR = *static_cast<type_t*>(body.value);                        \
		})

#define META_PROPERTY_INFO_BINDER_GET_DEFAULT()                                                                        \
	META_PROPERTY_INFO_BINDER_GET_CUSTOM(return &(static_cast<owner_t*>(body.owner)->*MEMBER_PTR);)

#define META_PROPERTY_INFO_BINDER_DEFAULT(OWNER, TYPE, NAME_SYMBOL, FLAGS)                                             \
	namespace Meta                                                                                                     \
	{                                                                                                                  \
	META_PROPERTY_INFO_BINDER(OWNER, TYPE, NAME_SYMBOL, FLAGS)                                                         \
	{                                                                                                                  \
		META_PROPERTY_INFO_BINDER_BODY(OWNER, NAME_SYMBOL);                                                            \
		META_PROPERTY_INFO_BINDER_SET_DEFAULT();                                                                       \
		META_PROPERTY_INFO_BINDER_GET_DEFAULT();                                                                       \
	};                                                                                                                 \
	}

#endif
