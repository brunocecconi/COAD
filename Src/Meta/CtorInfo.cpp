
#include "Meta/CtorInfo.h"

namespace Meta
{

const TypeInfo& CtorInfo::OwnerType() const
{
	return owner_type_;
}

const method_params_signature_t& CtorInfo::ParamsSignature() const
{
	return params_signature_;
}

Uint32 CtorInfo::NeededParamCount() const
{
	return total_param_count_ - optional_param_count_;
}

Uint32 CtorInfo::TotalParamCount() const
{
	return total_param_count_;
}

Uint32 CtorInfo::OptionalParamCount() const
{
	return optional_param_count_;
}

} // namespace Meta
