
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
	using assets_data_hash_map_t = eastl::hash_map<Meta::id_t, eastl::vector<Byte>>;

	/**
	 * @brief A hash map that holds an asset id as key and an index to the data.
	 *
	 */
	using assets_index_map_t = eastl::hash_map<id_t, Uint64>;

	/**
	 * @brief A hash map that holds an asset id as key and an cursor index of a file.
	 *
	 */
	using assets_data_cursor_t = eastl::hash_map<id_t, Uint64>;

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
	MAYBEUNUSED bool Export(const char* path, const char* dst_path, RESULT_PARAM_DEFINE);

	template<typename Asset>
	MAYBEUNUSED bool Load(const char* path, RESULT_PARAM_DEFINE);

	template<typename Asset>
	MAYBEUNUSED bool Unload(const char* path, RESULT_PARAM_DEFINE);

	template<typename Asset>
	NODISCARD Ptr<Asset> Get(const char* path, RESULT_PARAM_DEFINE);

	template<typename Asset>
	NODISCARD Ptr<Asset> Get(id_t id, RESULT_PARAM_DEFINE);

public:
	void Reserve(Size new_capacity, RESULT_PARAM_DEFINE);

private:
	template<typename AssetType>
	AssetType* AddOrGet(const char* path);

	template<typename AssetType>
	AssetType* AddOrGetUninitialized(const char* path);

private:
	assets_data_hash_map_t data_map_{DEBUG_NAME_VAL("Asset")};
	assets_index_map_t	   index_map_{DEBUG_NAME_VAL("Asset")};
	assets_data_cursor_t   cursor_map_{DEBUG_NAME_VAL("Asset")};
	Stream::File		   stream_file_{};
	static Registry*	   instance_;
};

template<typename Asset>
bool Registry::Export(const char* path, const char* dst_path, RESULT_PARAM_IMPL)
{
	ClassValidation<Asset>{};

	RESULT_ENSURE_LAST_NOLOG(false);

#ifdef TOOL
	auto l_ptr = AddOrGet<Asset>(path);
	l_ptr->Export(dst_path, RESULT_ARG_PASS);
	return PTR(l_ptr);
#else
	ENFORCE_MSG(false, "Not allowed in current build type.");
	return PTR((Asset*)nullptr);
#endif
}

template<typename Asset>
bool Registry::Load(const char* path, RESULT_PARAM_IMPL)
{
	ClassValidation<Asset>{};

	RESULT_ENSURE_LAST_NOLOG(false);

	auto l_ptr = AddOrGet<Asset>(path);

	RESULT_CONDITION_ENSURE_NOLOG(l_ptr, eResultErrorAssetFailedToAdd, false);
	RESULT_ENSURE_CALL_NOLOG(Stream::Dynamic l_ar(DEBUG_NAME_VAL("Asset"), RESULT_ARG_PASS), false);
	RESULT_ENSURE_CALL_NOLOG(Io::File::ReadAll(l_ar.Container(), path, RESULT_ARG_PASS), false);
	RESULT_CONDITION_ENSURE_NOLOG(!l_ar.BufferIsEmpty(), eResultErrorAssetLoadFailedInvalidFile, false);
	RESULT_CONDITION_ENSURE_NOLOG(l_ar.Read(*l_ptr, RESULT_ARG_PASS), eResultErrorStreamFailedToRead, false);
	RESULT_CONDITION_ENSURE_NOLOG(l_ptr->OnLoad(), eResultErrorAssetObjectLoadFailed, false);
	RESULT_OK();

	return true;
}

template<typename Asset>
bool Registry::Unload(const char* path, RESULT_PARAM_IMPL)
{
	ClassValidation<Asset>{};
	RESULT_ENSURE_LAST_NOLOG(false);
	RESULT_OK();
	return true;
}

template<typename Asset>
Ptr<Asset> Registry::Get(const char* path, RESULT_PARAM_IMPL)
{
	return Get<Asset>(MakeId(path, strlen(path)), RESULT_ARG_PASS);
}

template<typename Asset>
Ptr<Asset> Registry::Get(const id_t id, RESULT_PARAM_IMPL)
{
	ClassValidation<Asset>{};

	RESULT_ENSURE_LAST_NOLOG(PTR((Asset*)nullptr));

	const auto& l_type		 = Meta::Typeof<Asset>();
	auto&		l_data_array = data_map_[l_type.Id()];
	if (index_map_.find(id) != index_map_.cend())
	{
		return PTR(reinterpret_cast<Asset*>(l_data_array.data() + index_map_[id]));
	}
	return PTR((Asset*)nullptr);
}

template<typename AssetType>
AssetType* Registry::AddOrGet(const char* path)
{
	const id_t l_id			= Asset::MakeId(path, strlen(path));
	auto&	   l_data_array = data_map_[Meta::Typeof<AssetType>().Id()];
	if (index_map_.find(l_id) == index_map_.cend())
	{
		Stream::Dynamic l_stream{};
		AssetType l_asset{l_stream, path};
		index_map_[l_id] = l_data_array.size();
		l_data_array.insert(l_data_array.cend(), reinterpret_cast<Uint8*>(&l_asset),
							reinterpret_cast<Uint8*>(&l_asset) + sizeof(AssetType));
	}
	return reinterpret_cast<AssetType*>(l_data_array.data() + index_map_[l_id]);
}

template<typename AssetType>
AssetType* Registry::AddOrGetUninitialized(const char* path)
{
	const id_t l_id			= Asset::MakeId(path, strlen(path));
	auto&	   l_data_array = data_map_[Meta::Typeof<AssetType>().Id()];
	if (index_map_.find(l_id) == index_map_.cend())
	{
		index_map_[l_id] = l_data_array.size();
		l_data_array.insert(l_data_array.cend(), sizeof(AssetType), 0);
	}
	return reinterpret_cast<AssetType*>(l_data_array.data() + index_map_[l_id]);
}

} // namespace Asset

#endif
