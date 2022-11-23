
#ifndef META_PROPERTY_INFO_H
#define META_PROPERTY_INFO_H

#include "Meta/TypeInfo.h"
#include "Meta/Value.h"

namespace Meta
{

class PropertyInfo
{
public:
	enum Flags
	{
		eNone,
		eReadOnly,
		eWriteOnly,
		eReadWrite
	};

	struct Body
	{
		void* info;
		void* owner;
		void* value;
	};

	template<typename Owner, typename T, Uint32 Flags = 0>
	struct Binder
	{
		using owner_t	   = Owner;
		using type_t	   = T;
		using member_ptr_t = T(Owner::*);
		using body_t	   = Body;

		static constexpr auto FLAGS = Flags;

		template<Size N>
		EXPLICIT constexpr Binder(const char (&name)[N]);

		NODISCARD constexpr const char* Name() const;

	private:
		const char* name_;
	};
	using binder_set_function_t = void (*)(const Body&);
	using binder_get_function_t = void* (*)(const Body&);

public:
	template<typename BinderType>
	EXPLICIT PropertyInfo(BinderType);

public:
	void			Set(void* owner, Value&& value) const;
	NODISCARD Value Get(const void* owner) const;

private:
	const TypeInfo&		  owner_type_info_;
	const TypeInfo&		  type_info_;
	const char*			  name_;
	Hash::fnv1a_t		  id_;
	Uint32				  flags_;
	binder_set_function_t set_function_;
	binder_get_function_t get_function_;
};

template<typename Owner, typename T, Uint32 Flags>
template<Size N>
constexpr PropertyInfo::Binder<Owner, T, Flags>::Binder(const char (&name)[N]) : name_{name}
{
}

template<typename Owner, typename T, Uint32 Flags>
constexpr const char* PropertyInfo::Binder<Owner, T, Flags>::Name() const
{
	return name_;
}

template<typename BinderType>
PropertyInfo::PropertyInfo(BinderType value)
	: owner_type_info_{Typeof<typename BinderType::owner_t>()}, type_info_{Typeof<typename BinderType::type_t>()},
	  name_{value.Name()}, id_{BinderType::ID}, set_function_{&BinderType::Set}, get_function_{&BinderType::Get}
{
}

} // namespace Meta

#endif
