
#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "Core/Common.h"
#include "Core/Algorithm.h"
#include "Asset/Object.h"
#include "Core/Ptr.h"
#include "Core/Io.h"

#include <EASTL/hash_map.h>

namespace Asset
{

/**
 * @brief Asset registry class.
 *
 */
class Registry
{
public:
	/**
	 * @brief A hash map that holds the type info to a vector of memory in bytes.
	 *
	 */
	using assets_data_hash_map_t = eastl::hash_map<Meta::id_t, eastl::vector<byte_t>>;

	/**
	 * @brief A hash map that holds an asset id as key and an index to the data.
	 *
	 */
	using assets_index_map_t = eastl::hash_map<id_t, uint64_t>;

	/**
	 * @brief A hash map that holds an asset id as key and an cursor index of a file.
	 *
	 */
	using assets_data_cursor_t = eastl::hash_map<id_t, uint64_t>;

public:
	EXPLICIT Registry(RESULT_PARAM_DEFINE);

	Registry(Registry&&) NOEXCEPT			 = delete;
	Registry& operator=(Registry&&) NOEXCEPT = delete;
	Registry(const Registry&)				 = delete;
	Registry& operator=(const Registry&)	 = delete;
	~Registry()								 = default;

public:
	static Registry& Instance(RESULT_PARAM_DEFINE);

public:
	template<typename Asset>
	MAYBEUNUSED bool Export(const char* Path, const char* DstPath, RESULT_PARAM_DEFINE);

	template<typename Asset>
	MAYBEUNUSED bool Load(const char* Path, RESULT_PARAM_DEFINE);

	template<typename Asset>
	MAYBEUNUSED bool Unload(const char* Path, RESULT_PARAM_DEFINE);

	template<typename Asset>
	NODISCARD Ptr<Asset> Get(const char* Path, RESULT_PARAM_DEFINE);

	template<typename Asset>
	NODISCARD Ptr<Asset> Get(id_t Id, RESULT_PARAM_DEFINE);

public:
	void Reserve(size_t NewCapacity, RESULT_PARAM_DEFINE);

private:
	template<typename AssetType>
	AssetType* AddOrGet(const char* Path);

	template<typename AssetType>
	AssetType* AddOrGetUninitialized(const char* Path);

private:
	assets_data_hash_map_t mDataMap{DEBUG_NAME_VAL("Asset")};
	assets_index_map_t	   mIndexMap{DEBUG_NAME_VAL("Asset")};
	assets_data_cursor_t   mCursorMap{DEBUG_NAME_VAL("Asset")};
	Stream::File		   mStreamFile{};
	static Registry*	   mInstance;
};

template<typename Asset>
bool Registry::Export(const char* Path, const char* DstPath, RESULT_PARAM_IMPL)
{
	ClassValidation<Asset>{};

	RESULT_ENSURE_LAST_NOLOG(false);

#ifdef TOOL
	auto lPtr = AddOrGet<Asset>(Path);
	lPtr->Export(DstPath, RESULT_ARG_PASS);
	return PTR(lPtr);
#else
	ENFORCE_MSG(false, "Not allowed in current build type.");
	return PTR((Asset*)nullptr);
#endif
}

template<typename Asset>
bool Registry::Load(const char* Path, RESULT_PARAM_IMPL)
{
	ClassValidation<Asset>{};

	RESULT_ENSURE_LAST_NOLOG(false);

	auto lPtr = AddOrGet<Asset>(Path);

	RESULT_CONDITION_ENSURE_NOLOG(lPtr, AssetFailedToAdd, false);
	RESULT_ENSURE_CALL_NOLOG(Stream::Dynamic lAr(DEBUG_NAME_VAL("Asset"), RESULT_ARG_PASS), false);
	RESULT_ENSURE_CALL_NOLOG(Io::File::ReadAll(lAr.Container(), Path, RESULT_ARG_PASS), false);
	RESULT_CONDITION_ENSURE_NOLOG(!lAr.BufferIsEmpty(), AssetLoadFailedInvalidFile, false);
	RESULT_CONDITION_ENSURE_NOLOG(lAr.Read(*lPtr, RESULT_ARG_PASS), StreamFailedToRead, false);
	RESULT_CONDITION_ENSURE_NOLOG(lPtr->OnLoad(), AssetObjectLoadFailed, false);
	RESULT_OK();

	return true;
}

template<typename Asset>
bool Registry::Unload(const char* Path, RESULT_PARAM_IMPL)
{
	ClassValidation<Asset>{};
	RESULT_ENSURE_LAST_NOLOG(false);
	RESULT_OK();
	return true;
}

template<typename Asset>
Ptr<Asset> Registry::Get(const char* Path, RESULT_PARAM_IMPL)
{
	return Get<Asset>(MakeId(Path, strlen(Path)), RESULT_ARG_PASS);
}

template<typename Asset>
Ptr<Asset> Registry::Get(const id_t Id, RESULT_PARAM_IMPL)
{
	ClassValidation<Asset>{};

	RESULT_ENSURE_LAST_NOLOG(PTR((Asset*)nullptr));

	const auto& lType		 = Meta::Typeof<Asset>();
	auto&		lDataArray = mDataMap[lType.Id()];
	if (mIndexMap.find(Id) != mIndexMap.cend())
	{
		return PTR(reinterpret_cast<Asset*>(lDataArray.data() + mIndexMap[Id]));
	}
	return PTR((Asset*)nullptr);
}

template<typename AssetType>
AssetType* Registry::AddOrGet(const char* Path)
{
	const id_t lId			= Asset::MakeId(Path, strlen(Path));
	auto&	   lDataArray = mDataMap[Meta::Typeof<AssetType>().Id()];
	if (mIndexMap.find(lId) == mIndexMap.cend())
	{
		Stream::Dynamic lStream{};
		AssetType lAsset{lStream, Path};
		mIndexMap[lId] = lDataArray.size();
		lDataArray.insert(lDataArray.cend(), reinterpret_cast<uint8_t*>(&lAsset),
							reinterpret_cast<uint8_t*>(&lAsset) + sizeof(AssetType));
	}
	return reinterpret_cast<AssetType*>(lDataArray.data() + mIndexMap[lId]);
}

template<typename AssetType>
AssetType* Registry::AddOrGetUninitialized(const char* Path)
{
	const id_t lId			= Asset::MakeId(Path, strlen(Path));
	auto&	   lDataArray = mDataMap[Meta::Typeof<AssetType>().Id()];
	if (mIndexMap.find(lId) == mIndexMap.cend())
	{
		mIndexMap[lId] = lDataArray.size();
		lDataArray.insert(lDataArray.cend(), sizeof(AssetType), 0);
	}
	return reinterpret_cast<AssetType*>(lDataArray.data() + mIndexMap[lId]);
}

} // namespace Asset

#endif
