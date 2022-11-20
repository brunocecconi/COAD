
#include "Meta/EnumInfo.h"

namespace Meta
{

EnumInfo*	  g_enum_info_none		  = nullptr;
EnumRegister* EnumRegister::instance_ = nullptr;

EnumInfo::EnumInfo() : type_info_{TypeInfo::None()}, flags_{0llu}
{
}

const TypeInfo& EnumInfo::Type() const
{
	return type_info_;
}

const EnumInfo& EnumInfo::None()
{
	if(!g_enum_info_none)
	{
		g_enum_info_none = new (Allocators::Default(NAME_VAL("Meta")).allocate(sizeof(EnumInfo))) EnumInfo{};
	}
	return *g_enum_info_none;
}

const char* EnumInfo::ToNameGeneric(Int64 value)
{
	const auto l_it = 
		eastl::find_if(entries_.begin(), entries_.end(), [&value](const auto& v){ return v.second == value; });
	ENFORCE_MSG(l_it != entries_.cend(), "Failed to get enum value name from value.");
	return l_it->first.data();
}

Int64 EnumInfo::ToValueGeneric(const char* name)
{
	const auto l_it = 
		eastl::find_if(entries_.begin(), entries_.end(), [&name](const auto& v){ return v.first == name; });
	ENFORCE_MSG(l_it != entries_.cend(), "Failed to get enum value from name.");
	return l_it->second;
}

bool EnumInfo::IsBitmask() const
{
	return flags_ & eBitmask;
}

bool EnumInfo::HasFlags(const Int64 value) const
{
	return IsBitmask() ? flags_ & value : false;
}

EnumRegister::EnumRegister()
{
	enums_.reserve(64);
}

const EnumInfo& EnumRegister::Get(const char* name)
{
	if (enums_.find(name) != enums_.cend())
	{
		return enums_.at(name);
	}
	return EnumInfo::None();
}

bool EnumRegister::IsRegistered(const char* name) const
{
	return enums_.find(name) != enums_.cend();
}

EnumRegister& EnumRegister::Instance()
{
	if (!instance_)
	{
		instance_ = new (Allocators::Default(NAME_VAL("Meta")).allocate(sizeof(EnumRegister))) EnumRegister{};
	}
	return *instance_;
}

const EnumInfo& Enumof(const char* name)
{
	return EnumRegister::Instance().Get(name);
}

} // namespace Meta
