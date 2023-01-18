
#include "Meta/CtorInfo.h"

#include "Core/Assert.h"

namespace Meta
{

const TypeInfo& CtorInfo::OwnerType() const
{
	return owner_type_;
}

const CtorInfo::method_params_signature_t& CtorInfo::ParamsSignature() const
{
	return params_signature_;
}

uint32_t CtorInfo::NeededParamCount() const
{
	return total_param_count_ - optional_param_count_;
}

id_t CtorInfo::Id() const
{
	return id_;
}

uint32_t CtorInfo::TotalParamCount() const
{
	return total_param_count_;
}

uint32_t CtorInfo::OptionalParamCount() const
{
	return optional_param_count_;
}

#define META_CTOR_INFO_INVOKE_GENERIC_BODY(NUM, ...)                                                                   \
	ENFORCE_MSG(NUM >= NeededParamCount() && NUM <= TotalParamCount(),                                                 \
				"Invalid provided arguments count. Needed parameter count is '%u'.", NeededParamCount());              \
	auto l_args_tuple = eastl::make_tuple(__VA_ARGS__);                                                                \
	ENFORCE_MSG(Detail::CompareTypeInfoArray(                                                                          \
					ParamsSignature().data(),                                                                          \
					Detail::GetParamsSignatureFromTuple<method_params_signature_t>(l_args_tuple).data(), NUM),         \
				"Invalid arguments signature.");                                                                       \
	Value l_value{owner_type_};                                                                                        \
	Body  l_body{this, (void*)l_value.AsGeneric(), NUM, &l_args_tuple};                                                \
	function_(l_body);                                                                                                 \
	return eastl::move(l_value)

Value CtorInfo::Invoke() const
{
	META_CTOR_INFO_INVOKE_GENERIC_BODY(0);
}

Value CtorInfo::Invoke(Value p1) const
{
	META_CTOR_INFO_INVOKE_GENERIC_BODY(1, p1);
}

Value CtorInfo::Invoke(Value p1, Value p2) const
{
	META_CTOR_INFO_INVOKE_GENERIC_BODY(2, p1, p2);
}

Value CtorInfo::Invoke(Value p1, Value p2, Value p3) const
{
	META_CTOR_INFO_INVOKE_GENERIC_BODY(3, p1, p2, p3);
}

Value CtorInfo::Invoke(Value p1, Value p2, Value p3, Value p4) const
{
	META_CTOR_INFO_INVOKE_GENERIC_BODY(4, p1, p2, p3, p4);
}

Value CtorInfo::Invoke(Value p1, Value p2, Value p3, Value p4, Value p5) const
{
	META_CTOR_INFO_INVOKE_GENERIC_BODY(5, p1, p2, p3, p4, p5);
}

Value CtorInfo::Invoke(Value p1, Value p2, Value p3, Value p4, Value p5, Value p6) const
{
	META_CTOR_INFO_INVOKE_GENERIC_BODY(6, p1, p2, p3, p4, p5, p6);
}

Value CtorInfo::Invoke(Value p1, Value p2, Value p3, Value p4, Value p5, Value p6, Value p7) const
{
	META_CTOR_INFO_INVOKE_GENERIC_BODY(7, p1, p2, p3, p4, p5, p6, p7);
}

Value CtorInfo::Invoke(Value p1, Value p2, Value p3, Value p4, Value p5, Value p6, Value p7, Value p8) const
{
	META_CTOR_INFO_INVOKE_GENERIC_BODY(8, p1, p2, p3, p4, p5, p6, p7, p8);
}

Value CtorInfo::operator()() const
{
	return Invoke();
}

Value CtorInfo::operator()(Value p1) const
{
	return Invoke(p1);
}

Value CtorInfo::operator()(Value p1, Value p2) const
{
	return Invoke(p1, p2);
}

Value CtorInfo::operator()(Value p1, Value p2, Value p3) const
{
	return Invoke(p1, p2, p3);
}

Value CtorInfo::operator()(Value p1, Value p2, Value p3, Value p4) const
{
	return Invoke(p1, p2, p3, p4);
}

Value CtorInfo::operator()(Value p1, Value p2, Value p3, Value p4, Value p5) const
{
	return Invoke(p1, p2, p3, p4, p5);
}

Value CtorInfo::operator()(Value p1, Value p2, Value p3, Value p4, Value p5, Value p6) const
{
	return Invoke(p1, p2, p3, p4, p5, p6);
}

Value CtorInfo::operator()(Value p1, Value p2, Value p3, Value p4, Value p5, Value p6, Value p7) const
{
	return Invoke(p1, p2, p3, p4, p5, p6, p7);
}

Value CtorInfo::operator()(Value p1, Value p2, Value p3, Value p4, Value p5, Value p6, Value p7, Value p8) const
{
	return Invoke(p1, p2, p3, p4, p5, p6, p7, p8);
}
} // namespace Meta
