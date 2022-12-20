
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

uint64_t TypeInfo::Id() const
{
	return id_;
}

uint64_t TypeInfo::Size() const
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
	return id_ == Typeof<int8_t>().id_ || id_ == Typeof<int16_t>().id_ || id_ == Typeof<int32_t>().id_ ||
		   id_ == Typeof<int64_t>().id_ || id_ == Typeof<uint8_t>().id_ || id_ == Typeof<uint16_t>().id_ ||
		   id_ == Typeof<uint32_t>().id_ || id_ == Typeof<uint64_t>().id_;
}

bool TypeInfo::IsFloatingPoint() const
{
	return id_ == Typeof<float32_t>().id_ || id_ == Typeof<float64_t>().id_;
}

bool TypeInfo::IsConvertibleTo(const TypeInfo& value) const
{
	return (IsIntegral() && value.IsIntegral()) || (IsFloatingPoint() && value.IsFloatingPoint());
}

eastl::string TypeInfo::ToString(const uint64_t capacity) const
{
	RawBuffer<char> l_str{capacity, DEBUG_NAME_VAL("Meta")};
	sprintf(l_str.Data(), "TypeInfo(Name=%s,Id=%llu,Size=%llu)", name_, id_, size_);
	return eastl::string{l_str.Data(), EASTLAllocatorType{DEBUG_NAME_VAL("Meta")}};
}

eastl::string TypeInfo::ToValueString(void* value, const uint64_t capacity) const
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
	Typeof<int8_t>();
	Typeof<int16_t>();
	Typeof<int32_t>();
	Typeof<int64_t>();
	Typeof<uint8_t>();
	Typeof<uint16_t>();
	Typeof<uint32_t>();
	Typeof<uint64_t>();
	Typeof<float32_t>();
	Typeof<float64_t>();
	Typeof<eastl::string>();
	Typeof<eastl::string_view>();
}

} // namespace Meta
