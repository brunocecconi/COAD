
#ifndef CORE_REFLECTION_H
#define CORE_REFLECTION_H

#include "Core/types.h"
#include "Core/lang.h"

#include <EASTL/string.h>
#include <EASTL/type_traits.h>
#include <EASTL/span.h>

namespace refl
{

class type_info;
class argument_info;
class ctor_info;
class property_info;
class method_info;
class class_info;
class enum_info;

namespace type_info_type
{
enum type
{
	none,
	primitive_,
	class_,
	enum_
};

template < typename T, bool is_class, bool is_enum >
struct extract_type_detail
{
	static constexpr i32 value = type::primitive_;
};

template<typename T>
struct extract_type_detail<T, true, false>
{
	static constexpr i32 value = type::class_;
};

template<typename T>
struct extract_type_detail<T, false, true>
{
	static constexpr i32 value = type::enum_;
};

template < typename T >
using extract_type = extract_type_detail<T, eastl::is_class_v<T>, eastl::is_enum_v<T>>;

}

/**
 * @brief Type info class.
 * 
*/
class type_info
{
	CLASS_BODY_NO_HEADER(type_info)

public:
	constexpr type_info(in_t<u64> id, in_t<eastl::string_view> name, in_t<u64> size_of) 
		: id_{id}, name_{name}, size_of_{size_of}
	{
	}

public:
	constexpr auto id() const { return id_; }
	constexpr auto name() const { return name_.data(); }
	constexpr auto size_of() const { return size_of_; }

private:
	const u64 id_;
	const eastl::string_view name_;
	const u64 size_of_;
};

template < typename T >
constexpr type_info typeinfo();

/**
 * @brief Argument info class.
 * 
*/
class argument_info
{
	CLASS_BODY_NO_HEADER(argument_info)
};

struct attribute_info
{
	CLASS_BODY_NO_HEADER(attribute_info)
};

class ctor_info
{
	CLASS_BODY_NO_HEADER(ctor_info)
};

class property_info
{
	CLASS_BODY_NO_HEADER(property_info)

public:
	constexpr property_info(in_t<type_info> owner_type, in_t<type_info> type, in_t<eastl::string_view> name)
		: owner_type_{owner_type}, type_{type}, name_{name}
	{
	}

public:
	constexpr auto& owner_type() const { return owner_type_; }
	constexpr auto& type() const { return type_; }
	constexpr auto name() const { return name_.data(); }

private:
	const type_info& owner_type_;
	const type_info& type_;
	const eastl::string_view name_;
};

class method_info
{
	CLASS_BODY_NO_HEADER(method_info)
};

class class_info : type_info
{
	CLASS_BODY_NO_HEADER(class_info)
};

class enum_info
{
	CLASS_BODY_NO_HEADER(enum_info)

public:
	constexpr enum_info(in_t<type_info> type, move_t<eastl::span<property_info>> properties_view)
		: type_{type}, properties_view_{eastl::move(properties_view)}
	{
	}

public:
	constexpr auto& type() const { return type_; }
	constexpr auto& properties() const { return properties_view_; }

protected:
	const type_info& type_;
	const eastl::span<property_info> properties_view_;
};

template < u64 PropertiesCount >
class enum_info_desc : public enum_info
{
public:
	constexpr enum_info_desc(in_t<type_info> type, eastl::array<property_info, PropertiesCount>&& properties)
		: properties_{eastl::move(properties)}, enum_info{type, eastl::span{(property_info*)properties_.data(), PropertiesCount} }
	{
	}

private:
	const eastl::array<property_info, PropertiesCount> properties_;
};

}

#define REFLECT()

#endif