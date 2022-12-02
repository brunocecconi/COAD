
#include "Meta/TypeInfo.h"
#include "Core/Hash.h"

namespace Meta
{

TypeInfo*	  g_type_info_none		  = nullptr;
TypeRegistry* TypeRegistry::instance_ = nullptr;

const TypeInfo& Typeof(const char* name)
{
	return TypeRegistry::Instance().Get(name);
}

const TypeInfo& Typeof(const id_t id)
{
	return TypeRegistry::Instance().Get(id);
}

TypeInfo::TypeInfo() : id_{Hash::Fnv1A("None")}, size_{0}, name_{"None"}, operation_function_{nullptr}
{
}

TypeRegistry::TypeRegistry()
{
	types_.reserve(256);
}

const TypeInfo& TypeRegistry::Get(const char* name)
{
	return Get(Hash::Fnv1A(strlen(name), name));
}

const TypeInfo& TypeRegistry::Get(const id_t id)
{
	if (types_.find(id) != types_.cend())
	{
		return types_.at(id);
	}
	return TypeInfo::None();
}

bool TypeRegistry::IsRegistered(const char* name) const
{
	return types_.find(Hash::Fnv1A(strlen(name), name)) != types_.cend();
}

TypeRegistry& TypeRegistry::Instance()
{
	if (!instance_)
	{
		instance_ = new (Allocators::Default(DEBUG_NAME_VAL("Meta")).allocate(sizeof(TypeRegistry))) TypeRegistry{};
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

const TypeInfo& TypeInfo::None()
{
	if (!g_type_info_none)
	{
		g_type_info_none = new (Allocators::Default(DEBUG_NAME_VAL("Meta")).allocate(sizeof(TypeInfo))) TypeInfo{};
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

bool TypeInfo::IsBoolean() const
{
	return id_ == Typeof<bool>().id_;
}

bool TypeInfo::IsIntegral() const
{
	return id_ == Typeof<Int8>().id_ || id_ == Typeof<Int16>().id_ || id_ == Typeof<Int32>().id_ ||
		   id_ == Typeof<Int64>().id_ || id_ == Typeof<Uint8>().id_ || id_ == Typeof<Uint16>().id_ ||
		   id_ == Typeof<Uint32>().id_ || id_ == Typeof<Uint64>().id_;
}

bool TypeInfo::IsFloatingPoint() const
{
	return id_ == Typeof<Float32>().id_ || id_ == Typeof<Float64>().id_;
}

bool TypeInfo::IsConvertibleTo(const TypeInfo& value) const
{
	return (IsIntegral() && value.IsIntegral()) || (IsFloatingPoint() && value.IsFloatingPoint());
}

eastl::string TypeInfo::ToString(const Uint64 capacity) const
{
	RawBuffer<char> l_str{capacity, DEBUG_NAME_VAL("Meta")};
	sprintf(l_str.Get(), "TypeInfo(Name=%s,Id=%llu,Size=%llu)", name_, id_, size_);
	return eastl::string{l_str.Get(), EASTLAllocatorType{DEBUG_NAME_VAL("Meta")}};
}

eastl::string TypeInfo::ToValueString(void* value, const Uint64 capacity) const
{
	ENFORCE_MSG(operation_function_, "Operations function is marked as not implemented for '%s' type, and it is needed to properly "
									 "apply the value string operation.", name_);

	OperationBody l_body{};

	eastl::string l_string{EASTLAllocatorType{DEBUG_NAME_VAL("Meta")}};
	auto		  l_args_tuple = eastl::make_tuple(&l_string, value, capacity);

	l_body.type			   = eToString;
	l_body.args_tuple	   = &l_args_tuple;
	l_body.args_tuple_size = 3;

	operation_function_(l_body);

	return l_string;
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
