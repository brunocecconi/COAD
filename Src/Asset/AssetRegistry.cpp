
#include "Asset/Registry.h"
#include "Core/Paths.h"

namespace Asset
{

Registry* Registry::mInstance = nullptr;

Registry::Registry(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
#ifdef TOOL
	const auto& lPath = Paths::AssetDir(DEBUG_NAME_VAL("Asset")) + "Global.AssetRegistry";
	mStreamFile.OpenRead(lPath.c_str(), RESULT_ARG_PASS);
	if (RESULT_LAST_COMPARE(IoFileOpenFailed))
	{
		RESULT_OK();
		RESULT_CONDITION_ENSURE_NOLOG(mStreamFile.OpenWrite(lPath.c_str(), RESULT_ARG_PASS),
									  AssetFailedToOpenRegistryFile);
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
			new (Allocators::default_t{DEBUG_NAME_VAL("Asset")}.allocate(sizeof(Registry))) Registry{RESULT_ARG_PASS};
	}
	return *mInstance;
}

void Registry::Reserve(const size_t NewCapacity, RESULT_PARAM_IMPL)
{
	RESULT_CONDITION_ENSURE_NOLOG(NewCapacity > 0ull, ZeroSize);
	mDataMap.reserve(NewCapacity);
	mIndexMap.reserve(NewCapacity);
}

} // namespace Asset
