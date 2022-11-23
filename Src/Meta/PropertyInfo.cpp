
#include "Meta/PropertyInfo.h"

void Meta::PropertyInfo::Set(void* owner, Value&& value) const
{
	const Body l_body{(void*)this, owner, &value};
	set_function_(l_body);
}

Meta::Value Meta::PropertyInfo::Get(const void* owner) const
{
	const Body l_body{(void*)this, const_cast<void*>(owner), nullptr};
	return Value{get_function_(l_body), type_info_};
}
