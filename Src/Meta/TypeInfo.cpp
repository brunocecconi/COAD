
#include "Meta/TypeInfo.h"
#include "Core/Hash.h"

namespace Meta
{

TypeInfo*	  g_type_info_none		  = nullptr;
TypeRegister* TypeRegister::instance_ = nullptr;

const TypeInfo& Typeof(const char* name)
{
	return TypeRegister::Instance().Get(name);
}

TypeInfo::TypeInfo() : id_{Hash::Fnv1AHash("none")}, size_{0}, name_{"none"}
{
}

TypeRegister::TypeRegister()
{
	types_.reserve(256);
}

const TypeInfo& TypeRegister::Get(const char* name)
{
	if (types_.find(name) != types_.cend())
	{
		return types_.at(name);
	}
	return TypeInfo::None();
}

bool TypeRegister::IsRegistered(const char* name) const
{
	return types_.find(name) != types_.cend();
}

TypeRegister& TypeRegister::Instance()
{
	if (!instance_)
	{
		instance_ = new (Allocators::Default(NAME_VAL("Meta")).allocate(sizeof(TypeRegister))) TypeRegister{};
	}
	return *instance_;
}


Uint64 TypeInfo::Id() const
{
	return id_;
}

Uint64 TypeInfo::Size() const
{
	return size_;
}

const char* TypeInfo::Name() const
{
	return name_;
}

eastl::string TypeInfo::ToString() const
{
	char l_str[256];
	sprintf(l_str, "TypeInfo(Name=%s,Id=%llu,Size=%llu)", name_, id_, size_);
	return eastl::string{l_str, Allocators::Default{NAME_VAL("Meta")}};
}

const TypeInfo& TypeInfo::None()
{
	if (!g_type_info_none)
	{
		g_type_info_none = new (Allocators::Default(NAME_VAL("Meta")).allocate(sizeof(TypeInfo))) TypeInfo{};
	}
	return *g_type_info_none;
}

bool TypeInfo::IsValid() const
{
	return id_ && size_ && name_;
}

TypeInfo::operator bool() const
{
	return IsValid();
}

bool TypeInfo::operator==(const TypeInfo& value) const
{
	return id_ == value.id_;
}

bool TypeInfo::operator!=(const TypeInfo& value) const
{
	return id_ != value.id_;
}

void RegistryBaseTypes()
{
	Typeof<bool>();
	Typeof<Int8>();
	Typeof<Int16>();
	Typeof<Int32>();
	Typeof<Int64>();
	Typeof<Uint8>();
	Typeof<Uint16>();
	Typeof<Uint32>();
	Typeof<Uint64>();
	Typeof<Float32>();
	Typeof<Float64>();
	Typeof<eastl::string>();
	Typeof<eastl::string_view>();
}

} // namespace Meta
