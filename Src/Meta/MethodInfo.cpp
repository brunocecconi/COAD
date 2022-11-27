
#include "Meta/MethodInfo.h"

namespace Meta
{

NODISCARD const TypeInfo& MethodInfo::OwnerType() const
{
	return owner_type_;
}

NODISCARD const TypeInfo& MethodInfo::ReturnType() const
{
	return return_type_;
}

const char* MethodInfo::Name() const
{
	return name_;
}

Hash::fnv1a_t MethodInfo::Id() const
{
	return id_;
}

Uint32 MethodInfo::Flags() const
{
	return flags_;
}

NODISCARD const MethodInfo::method_params_signature_t& MethodInfo::ParamsSignature() const
{
	return params_signature_;
}

NODISCARD Uint32 MethodInfo::NeededParamCount() const
{
	return total_param_count_ - optional_param_count_;
}

NODISCARD Uint32 MethodInfo::TotalParamCount() const
{
	return total_param_count_;
}

NODISCARD Uint32 MethodInfo::OptionalParamCount() const
{
	return optional_param_count_;
}

#define META_METHOD_INFO_INVOKE_BODY(NUM, ...)                                                                         \
	ENFORCE_MSG(owner, "Invalid owner when invoke %s::%s.", owner_type_.Name(), name_);                                                                              \
	ENFORCE_MSG(NUM >= NeededParamCount() && NUM <= TotalParamCount(),                                                 \
				"Invalid provided arguments count. Needed parameter count is '%u'.", NeededParamCount());              \
	auto l_args_tuple = eastl::make_tuple(__VA_ARGS__);                                                                \
	ENFORCE_MSG(Detail::CompareTypeInfoArray(                                                                          \
					ParamsSignature().data(),                                                                          \
					Detail::GetParamsSignatureFromTuple<method_params_signature_t>(l_args_tuple).data(), NUM),         \
				"Invalid arguments signature.");                                                                       \
	Body l_body{this, const_cast<void*>(owner), NUM, &l_args_tuple};                                                                      \
	return function_(l_body)

Value MethodInfo::Invoke(const void* owner) const
{
	META_METHOD_INFO_INVOKE_BODY(0);
}

Value MethodInfo::Invoke(const void* owner, Value p1) const
{
	META_METHOD_INFO_INVOKE_BODY(1, p1);
}

Value MethodInfo::Invoke(const void* owner, Value p1, Value p2) const
{
	META_METHOD_INFO_INVOKE_BODY(2, p1, p2);
}

Value MethodInfo::Invoke(const void* owner, Value p1, Value p2, Value p3) const
{
	META_METHOD_INFO_INVOKE_BODY(3, p1, p2, p3);
}

Value MethodInfo::Invoke(const void* owner, Value p1, Value p2, Value p3, Value p4) const
{
	META_METHOD_INFO_INVOKE_BODY(4, p1, p2, p3, p4);
}

Value MethodInfo::Invoke(const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5) const
{
	META_METHOD_INFO_INVOKE_BODY(5, p1, p2, p3, p4, p5);
}

Value MethodInfo::Invoke(const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5, Value p6) const
{
	META_METHOD_INFO_INVOKE_BODY(6, p1, p2, p3, p4, p5, p6);
}

Value MethodInfo::Invoke(const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5, Value p6,
						 Value p7) const
{
	META_METHOD_INFO_INVOKE_BODY(7, p1, p2, p3, p4, p5, p6, p7);
}

Value MethodInfo::Invoke(const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5, Value p6,
						 Value p7, Value p8) const
{
	META_METHOD_INFO_INVOKE_BODY(8, p1, p2, p3, p4, p5, p6, p7, p8);
}

Value MethodInfo::operator()(const void* owner) const
{
	return Invoke(owner);
}

Value MethodInfo::operator()(const void* owner, Value p1) const
{
	return Invoke(owner,p1);
}

Value MethodInfo::operator()(const void* owner, Value p1, Value p2) const
{
	return Invoke(owner,p1,p2);
}

Value MethodInfo::operator()(const void* owner, Value p1, Value p2, Value p3) const
{
	return Invoke(owner,p1,p2,p3);
}

Value MethodInfo::operator()(const void* owner, Value p1, Value p2, Value p3, Value p4) const
{
	return Invoke(owner,p1,p2,p3,p4);
}

Value MethodInfo::operator()(const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5) const
{
	return Invoke(owner,p1,p2,p3,p4,p5);
}

Value MethodInfo::operator()(const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5, Value p6) const
{
	return Invoke(owner,p1,p2,p3,p4,p5,p6);
}

Value MethodInfo::operator()(const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5, Value p6,
	Value p7) const
{
	return Invoke(owner,p1,p2,p3,p4,p5,p6,p7);
}

Value MethodInfo::operator()(const void* owner, Value p1, Value p2, Value p3, Value p4, Value p5, Value p6, Value p7,
	Value p8) const
{
	return Invoke(owner,p1,p2,p3,p4,p5,p6,p7,p8);
}

} // namespace Meta