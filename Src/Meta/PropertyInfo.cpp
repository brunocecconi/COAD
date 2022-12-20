
#include "Meta/PropertyInfo.h"

namespace Meta
{

void PropertyInfo::Set(void* owner, Value value) const
{
	ENFORCE_MSG(owner, "Invalid owner.");
	const Body l_body{(void*)this, owner, &value};
	set_function_(l_body);
}

Value PropertyInfo::Get(const void* owner) const
{
	ENFORCE_MSG(owner, "Invalid owner.");
	const Body l_body{(void*)this, const_cast<void*>(owner), nullptr};
	return Value{get_function_(l_body), type_info_};
}

const TypeInfo& PropertyInfo::OwnerType() const
{
	return owner_type_info_;
}

const TypeInfo& PropertyInfo::Type() const
{
	return type_info_;
}

const char* PropertyInfo::Name() const
{
	return name_;
}

id_t PropertyInfo::Id() const
{
	return id_;
}

uint32_t PropertyInfo::Flags() const
{
	return flags_;
}

eastl::string PropertyInfo::ToString(const uint64_t capacity) const
{
	RawBuffer<char> l_str{capacity+512, DEBUG_NAME_VAL("Meta")};
	sprintf(l_str.Data(), "PropertyInfo(OwnerTypeInfo=%s,TypeInfo=%s,Name=%s,Id=%llu)", 
		owner_type_info_.ToString().c_str(), type_info_.ToString().c_str(), name_, id_);
	return eastl::string{l_str.Data(), EASTLAllocatorType{DEBUG_NAME_VAL("Meta")}};
}

} // namespace Meta
