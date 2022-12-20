
#include "Asset/Registry.h"
#include "Core/Paths.h"

namespace Asset
{

Registry* Registry::mInstance = nullptr;

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
	if (!mInstance)
	{
		mInstance =
			new (Allocators::Default{DEBUG_NAME_VAL("Asset")}.allocate(sizeof(Registry))) Registry{RESULT_ARG_PASS};
	}
	return *mInstance;
}

void Registry::Reserve(const size_t NewCapacity, RESULT_PARAM_IMPL)
{
	RESULT_CONDITION_ENSURE_NOLOG(NewCapacity > 0ull, eResultErrorZeroSize);
	mDataMap.reserve(NewCapacity);
	mIndexMap.reserve(NewCapacity);
}

} // namespace Asset
