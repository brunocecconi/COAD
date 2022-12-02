
#include "Meta/EnumInfo.h"
#include "Core/Algorithm.h"
#include "Core/Assert.h"

namespace Meta
{

EnumInfo*	  g_enum_info_none		  = nullptr;
EnumRegistry* EnumRegistry::instance_ = nullptr;

EnumInfo::EnumInfo() : type_info_{TypeInfo::None()}, flags_{0llu}
{
}

const TypeInfo& EnumInfo::Type() const
{
	return type_info_;
}

eastl::string EnumInfo::ToString() const
{
	eastl::string l_str{Allocators::Default{DEBUG_NAME_VAL("Meta")}};
	l_str.reserve(1024);
	l_str = "EnumInfo(TypeInfo=" + type_info_.ToString() + ',';
	l_str += Algorithm::ToString(entries_, l_str.get_allocator());
	l_str += ')';
	return l_str;
}

const EnumInfo& EnumInfo::None()
{
	if (!g_enum_info_none)
	{
		g_enum_info_none = new (Allocators::Default(DEBUG_NAME_VAL("Meta")).allocate(sizeof(EnumInfo))) EnumInfo{};
	}
	return *g_enum_info_none;
}

const char* EnumInfo::ToNameGeneric(Int64 value) const
{
	const auto l_it =
		eastl::find_if(entries_.begin(), entries_.end(), [&value](const auto& v) { return v.second == value; });
	ENFORCE_MSG(l_it != entries_.cend(), "Failed to get enum value name from value.");
	return l_it->first.data();
}

Int64 EnumInfo::ToValueGeneric(const char* name) const
{
	const auto l_it =
		eastl::find_if(entries_.begin(), entries_.end(), [&name](const auto& v) { return v.first == name; });
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

EnumRegistry::EnumRegistry()
{
	enums_.reserve(64);
}

const EnumInfo& EnumRegistry::Get(const char* name)
{
	return Get(Hash::Fnv1A(strlen(name), name));
}

const EnumInfo& EnumRegistry::Get(const id_t id)
{
	if (enums_.find(id) != enums_.cend())
	{
		return enums_.at(id);
	}
	return EnumInfo::None();
}

bool EnumRegistry::IsRegistered(const char* name) const
{
	return IsRegistered(Hash::Fnv1A(strlen(name), name));
}

bool EnumRegistry::IsRegistered(const id_t id) const
{
	return enums_.find(id) != enums_.cend();
}

EnumRegistry& EnumRegistry::Instance()
{
	if (!instance_)
	{
		instance_ = new (Allocators::Default(DEBUG_NAME_VAL("Meta")).allocate(sizeof(EnumRegistry))) EnumRegistry{};
	}
	return *instance_;
}

const EnumInfo& Enumof(const char* name)
{
	return EnumRegistry::Instance().Get(name);
}

} // namespace Meta
