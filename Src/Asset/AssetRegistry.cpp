
#include "Asset/Registry.h"
#include "Core/Paths.h"

namespace Asset
{

Registry* Registry::instance_ = nullptr;

Registry::Registry(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
#ifdef TOOL
	const auto& l_path = Paths::AssetDir(DEBUG_NAME_VAL("Asset")) + "Global.AssetRegistry";
	stream_file_.OpenRead(l_path.c_str(), RESULT_ARG_PASS);
	if(RESULT_LAST_COMPARE(eResultErrorIoFileOpenFailed))
	{
		RESULT_OK();
		RESULT_CONDITION_ENSURE_NOLOG(stream_file_.OpenWrite(l_path.c_str(), RESULT_ARG_PASS),
								  eResultErrorAssetFailedToOpenRegistryFile);
	}
#endif
	Reserve(32ull);
	RESULT_OK();
}

Registry& Registry::Instance(RESULT_PARAM_IMPL)
{
	if (!instance_)
	{
		instance_ =
			new (Allocators::Default{DEBUG_NAME_VAL("Asset")}.allocate(sizeof(Registry))) Registry{RESULT_ARG_PASS};
	}
	return *instance_;
}

void Registry::Reserve(const Size new_capacity, RESULT_PARAM_IMPL)
{
	RESULT_CONDITION_ENSURE_NOLOG(new_capacity > 0ull, eResultErrorZeroSize);
	data_map_.reserve(new_capacity);
	index_map_.reserve(new_capacity);
}

} // namespace Asset
