
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

const CtorInfo& ClassInfo::GetCtorInfo(const Uint64 index) const
{
	return ctors_[index];
}

const PropertyInfo& ClassInfo::GetPropertyInfo(const Hash::fnv1a_t id) const
{
	return properties_.at(id);
}

const MethodInfo& ClassInfo::GetMethodInfo(const Hash::fnv1a_t id) const
{
	return methods_.at(id);
}

bool ClassInfo::HasCtorInfo(const Uint64 index) const
{
	return index < ctors_.size();
}

bool ClassInfo::HasPropertyInfo(const Hash::fnv1a_t id) const
{
	return properties_.find(id) != properties_.cend();
}

bool ClassInfo::HasMethodInfo(const Hash::fnv1a_t id) const
{
	return methods_.find(id) != methods_.cend();
}

#define META_CLASS_INFO_INVOKE_CTOR(...)	\
	const auto l_ctor = Detail::FindCompatibleCtor(eastl::span{const_cast<CtorInfo*>(ctors_.data()), ctors_.size()}, Detail::CreateTypeInfoArray(__VA_ARGS__));	\
	ENFORCE_MSG(l_ctor, "Failed to find compatible ctor.");	\
	return l_ctor->Invoke(__VA_ARGS__);

Value ClassInfo::InvokeCtor() const
{
	META_CLASS_INFO_INVOKE_CTOR()
}

Value ClassInfo::InvokeCtor(Value p1) const
{
	META_CLASS_INFO_INVOKE_CTOR(p1)
}

Value ClassInfo::InvokeCtor(Value p1, Value p2) const
{
	META_CLASS_INFO_INVOKE_CTOR(p1, p2)
}

Value ClassInfo::InvokeCtor(Value p1, Value p2, Value p3) const
{
	META_CLASS_INFO_INVOKE_CTOR(p1, p2, p3)
}

Value ClassInfo::InvokeCtor(Value p1, Value p2, Value p3, Value p4) const
{
	META_CLASS_INFO_INVOKE_CTOR(p1, p2, p3, p4)
}

Value ClassInfo::InvokeCtor(Value p1, Value p2, Value p3, Value p4, Value p5) const
{
	META_CLASS_INFO_INVOKE_CTOR(p1, p2, p3, p4, p5)
}

Value ClassInfo::InvokeCtor(Value p1, Value p2, Value p3, Value p4, Value p5, Value p6) const
{
	META_CLASS_INFO_INVOKE_CTOR(p1, p2, p3, p4, p5, p6)
}

Value ClassInfo::InvokeCtor(Value p1, Value p2, Value p3, Value p4, Value p5, Value p6, Value p7) const
{
	META_CLASS_INFO_INVOKE_CTOR(p1, p2, p3, p4, p5, p6, p7)
}

Value ClassInfo::InvokeCtor(Value p1, Value p2, Value p3, Value p4, Value p5, Value p6, Value p7, Value p8) const
{
	META_CLASS_INFO_INVOKE_CTOR(p1, p2, p3, p4, p5, p6, p7, p8)
}

#define META_CLASS_INFO_INVOKE_METHOD(...)	\
	if(methods_.find(id) != methods_.cend())	\
	{	\
		return methods_.at(id).Invoke(owner, __VA_ARGS__);	\
	}	\
	return Value{};

Value ClassInfo::InvokeMethod(const Hash::fnv1a_t id, const void* owner) const
{
	META_CLASS_INFO_INVOKE_METHOD()
}

Value ClassInfo::InvokeMethod(Hash::fnv1a_t id, const void* owner, Value p1) const
{
	META_CLASS_INFO_INVOKE_METHOD(p1)
}

Value ClassInfo::InvokeMethod(Hash::fnv1a_t id, const void* owner, Value p1, Value p2) const
{
	META_CLASS_INFO_INVOKE_METHOD(p1,p2)
}

Value ClassInfo::InvokeMethod(Hash::fnv1a_t id, const void* owner, Value p1, Value p2, Value p3) const
{
	META_CLASS_INFO_INVOKE_METHOD(p1,p2,p3)
}

Value ClassInfo::InvokeMethod(Hash::fnv1a_t id, const void* owner, Value p1, Value p2, Value p3, Value p4) const
{
	META_CLASS_INFO_INVOKE_METHOD(p1,p2,p3,p4)
}

Value ClassInfo::InvokeMethod(Hash::fnv1a_t id, const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5) const
{
	META_CLASS_INFO_INVOKE_METHOD(p1,p2,p3,p4,p5)
}

Value ClassInfo::InvokeMethod(Hash::fnv1a_t id, const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5, Value p6) const
{
	META_CLASS_INFO_INVOKE_METHOD(p1,p2,p3,p4,p5,p6)
}

Value ClassInfo::InvokeMethod(Hash::fnv1a_t id, const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5, Value p6,
	Value p7) const
{
	META_CLASS_INFO_INVOKE_METHOD(p1,p2,p3,p4,p5,p6,p7)
}

Value ClassInfo::InvokeMethod(Hash::fnv1a_t id, const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5, Value p6,
	Value p7, Value p8) const
{
	META_CLASS_INFO_INVOKE_METHOD(p1,p2,p3,p4,p5,p6,p7,p8)
}

void ClassInfo::SetProperty(const Hash::fnv1a_t id, void* owner, const Value value) const
{
	if(properties_.find(id) != properties_.cend())
	{
		properties_.at(id).Set(owner, value);
	}
}

Value ClassInfo::GetProperty(const Hash::fnv1a_t id, const void* owner) const
{
	if(properties_.find(id) != properties_.cend())
	{
		return properties_.at(id).Get(owner);
	}
	return Value{};
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
