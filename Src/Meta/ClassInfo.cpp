
#include "Meta/ClassInfo.h"

namespace Meta
{

ClassInfo*	   g_class_info_none		= nullptr;
ClassRegistry* ClassRegistry::instance_ = nullptr;

ClassInfo::ClassInfo() : type_info_{&TypeInfo::None()}
{
}

const TypeInfo& ClassInfo::Type() const
{
	return *type_info_;
}

Uint32 ClassInfo::Flags() const
{
	return flags_;
}

const CtorInfo& ClassInfo::GetCtor(const Uint64 index) const
{
	return ctors_[index];
}

const PropertyInfo& ClassInfo::GetProperty(const Hash::fnv1a_t id) const
{
	return properties_.at(id);
}

const MethodInfo& ClassInfo::GetMethod(const Hash::fnv1a_t id) const
{
	return methods_.at(id);
}

bool ClassInfo::HasCtor(const Uint64 index) const
{
	return index < ctors_.size();
}

bool ClassInfo::HasProperty(const Hash::fnv1a_t id) const
{
	return properties_.find(id) != properties_.cend();
}

bool ClassInfo::HasMethod(const Hash::fnv1a_t id) const
{
	return methods_.find(id) != methods_.cend();
}

const ClassInfo& ClassInfo::None()
{
	if (!g_class_info_none)
	{
		g_class_info_none = new (Allocators::Default(DEBUG_NAME_VAL("Meta")).allocate(sizeof(ClassInfo))) ClassInfo{};
	}
	return *g_class_info_none;
}

ClassRegistry::ClassRegistry()
{
	classes_.reserve(256);
}

const ClassInfo& ClassRegistry::Get(const char* name)
{
	return Get(Hash::Fnv1AHash(strlen(name), name));
}

const ClassInfo& ClassRegistry::Get(const Hash::fnv1a_t id)
{
	if (classes_.find(id) != classes_.cend())
	{
		return classes_.at(id);
	}
	return ClassInfo::None();
}

bool ClassRegistry::IsRegistered(const char* name) const
{
	return classes_.find(Hash::Fnv1AHash(strlen(name), name)) != classes_.cend();
}

ClassRegistry& ClassRegistry::Instance()
{
	if (!instance_)
	{
		instance_ = new (Allocators::Default(DEBUG_NAME_VAL("Meta")).allocate(sizeof(ClassRegistry))) ClassRegistry{};
	}
	return *instance_;
}

const ClassInfo& Classof(const char* name)
{
	return ClassRegistry::Instance().Get(name);
}

const ClassInfo& Classof(const Hash::fnv1a_t id)
{
	return ClassRegistry::Instance().Get(id);
}

} // namespace Meta
