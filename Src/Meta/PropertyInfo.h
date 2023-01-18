
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
	template<id_t Id>
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
	template<id_t Id, typename Owner, typename T, uint32_t Flags = 0>
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
	NODISCARD id_t			  Id() const;
	NODISCARD uint32_t		  Flags() const;
	NODISCARD eastl::string ToString(uint64_t capacity = 256) const;

private:
	const TypeInfo&		  owner_type_info_;
	const TypeInfo&		  type_info_;
	const char*			  name_;
	id_t				  id_;
	uint32_t			  flags_;
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

#endif
