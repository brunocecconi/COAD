
#include "Meta/EnumInfo.h"
#include "Core/Algorithm.h"
#include "Core/Assert.h"

namespace Meta
{

EnumInfo*	  g_enum_info_none		  = nullptr;
EnumRegistry* EnumRegistry::instance_ = nullptr;

EnumInfo::EnumInfo() : mTypeInfo{TypeInfo::None()}, mFlags{0llu}
{
}

const TypeInfo& EnumInfo::Type() const
{
	return mTypeInfo;
}

eastl::string EnumInfo::ToString() const
{
	eastl::string lStr{EASTLAllocatorType{DEBUG_NAME_VAL("Meta")}};
	lStr.reserve(1024);
	lStr = "EnumInfo(TypeInfo=" + mTypeInfo.ToString() + ',';
	lStr += Algorithm::ToString(mEntries, lStr.get_allocator());
	lStr += ')';
	return lStr;
}

const EnumInfo& EnumInfo::None()
{
	if (!g_enum_info_none)
	{
		g_enum_info_none = new (Allocators::default_t(DEBUG_NAME_VAL("Meta")).allocate(sizeof(EnumInfo))) EnumInfo{};
	}
	return *g_enum_info_none;
}

const char* EnumInfo::ToNameGeneric(int64_t Value) const
{
	const auto l_it =
		eastl::find_if(mEntries.begin(), mEntries.end(), [&Value](const auto& v) { return v.second == Value; });
	ENFORCE_MSG(l_it != mEntries.cend(), "Failed to get enum value name from value.");
	return l_it->first.data();
}

int64_t EnumInfo::ToValueGeneric(const char* Name) const
{
	const auto l_it =
		eastl::find_if(mEntries.begin(), mEntries.end(), [&Name](const auto& v) { return v.first == Name; });
	ENFORCE_MSG(l_it != mEntries.cend(), "Failed to get enum value from name.");
	return l_it->second;
}

bool EnumInfo::IsBitmask() const
{
	return mFlags & eBitmask;
}

bool EnumInfo::HasFlags(const int64_t Value) const
{
	return IsBitmask() ? mFlags & Value : false;
}

EnumRegistry::EnumRegistry()
{
	enums_.reserve(64);
}

const EnumInfo& EnumRegistry::Get(const char* Name)
{
	return Get(Hash::Fnv1A(strlen(Name), Name));
}

const EnumInfo& EnumRegistry::Get(const id_t Id)
{
	if (enums_.find(Id) != enums_.cend())
	{
		return enums_.at(Id);
	}
	return EnumInfo::None();
}

bool EnumRegistry::IsRegistered(const char* Name) const
{
	return IsRegistered(Hash::Fnv1A(strlen(Name), Name));
}

bool EnumRegistry::IsRegistered(const id_t Id) const
{
	return enums_.find(Id) != enums_.cend();
}

EnumRegistry& EnumRegistry::Instance()
{
	if (!instance_)
	{
		instance_ = new (Allocators::default_t(DEBUG_NAME_VAL("Meta")).allocate(sizeof(EnumRegistry))) EnumRegistry{};
	}
	return *instance_;
}

const EnumInfo& Enumof(const char* Name)
{
	return EnumRegistry::Instance().Get(Name);
}

} // namespace Meta
