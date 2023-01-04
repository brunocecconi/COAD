
#ifndef ASSET_OBJECT_H
#define ASSET_OBJECT_H

#include "Core/Common.h"
#include "Core/Hash.h"
#include "Meta/Meta.h"
#include "Core/Stream.h"

#define ASSET_BODY(TYPE)                                                                                               \
	CLASS_BODY_NON_COPYABLE(TYPE);                                                                                     \
                                                                                                                       \
private:                                                                                                               \
	friend class Asset::Registry;                                                                                      \
	struct AssetClass                                                                                                  \
	{                                                                                                                  \
	}

#define ASSET_VALIDATION(TYPE)                                                                                         \
	CLASS_VALIDATION(                                                                                                  \
		TYPE, static_assert(Asset::is_asset_class<TYPE>,                                                               \
							"Invalid Asset class. Hint: You need to put ASSET_BODY() macro inside the class."););

namespace Asset
{

class Registry;

template<typename T>
concept is_asset_class = eastl::is_same_v<void, eastl::void_t<typename T::AssetClass>>;

using id_t = Hash::fnv1a_t;

template<size_t N>
constexpr id_t MakeId(const char (&Path)[N])
{
	return Hash::Fnv1A(Path);
}

constexpr id_t MakeId(const char* Path, const size_t Size)
{
	return Hash::Fnv1A(Size, Path);
}

/**
 * @brief Asset object.
 *
 */
class Object
{
	ASSET_BODY(Object);

public:
	using load_delegate_t	= void(RESULT_PARAM_IMPL);
	using unload_delegate_t = void(RESULT_PARAM_IMPL);

public:
	~Object();

private:
	EXPLICIT Object(Stream::Dynamic& SourceArchive, const char* Path, RESULT_PARAM_DEFINE);
	EXPLICIT Object(Stream::Dynamic& SourceArchive, const char* Path, id_t Id, RESULT_PARAM_DEFINE);

private:
	MAYBEUNUSED bool OnLoad(RESULT_PARAM_DEFINE);
	MAYBEUNUSED bool OnUnload(RESULT_PARAM_DEFINE);

public:
	NODISCARD eastl::string_view Path() const;
	NODISCARD id_t				 Id() const;
	NODISCARD bool				 IsLoaded() const;
	NODISCARD size_t				 SourceSize() const;

public:
	NODISCARD eastl::string ToString(uint64_t Capacity = 256) const;

public:
	MAYBEUNUSED bool AddPreLoadDelegate(eastl::function<load_delegate_t>&& Function, RESULT_PARAM_DEFINE);
	MAYBEUNUSED bool AddPostLoadDelegate(eastl::function<load_delegate_t>&& Function, RESULT_PARAM_DEFINE);
	MAYBEUNUSED bool AddPreUnloadDelegate(eastl::function<unload_delegate_t>&& Function, RESULT_PARAM_DEFINE);
	MAYBEUNUSED bool AddPostUnloadDelegate(eastl::function<unload_delegate_t>&& Function, RESULT_PARAM_DEFINE);

private:
	STREAM_IMPL_FRIEND();

	eastl::string									  mPath{DEBUG_NAME_VAL("Asset")};
	id_t											  mId{};
	bool											  mIsLoaded{};
	size_t											  mSourceSize{};
	eastl::vector<eastl::function<load_delegate_t>>	  mPreloadDelegate{DEBUG_NAME_VAL("Asset")};
	eastl::vector<eastl::function<load_delegate_t>>	  mPostloadDelegate{DEBUG_NAME_VAL("Asset")};
	eastl::vector<eastl::function<unload_delegate_t>> mPreunloadDelegate{DEBUG_NAME_VAL("Asset")};
	eastl::vector<eastl::function<unload_delegate_t>> mPostunloadDelegate{DEBUG_NAME_VAL("Asset")};

private:
	META_REBINDER_TYPE_INFO();
};
ASSET_VALIDATION(Object);

} // namespace Asset

META_TYPE_BINDER_BEGIN(Asset::Object)
META_TYPE_BINDER_OPERATIONS_CUSTOM(META_TYPE_BINDER_OPERATION_NOT_IMPL(eDefaultCtor),
								   META_TYPE_BINDER_DEFAULT_OPERATION_MOVE_CTOR(),
								   META_TYPE_BINDER_OPERATION_NOT_IMPL(eCopyCtor),
								   META_TYPE_BINDER_DEFAULT_OPERATION_MOVE_ASSIGN(),
								   META_TYPE_BINDER_OPERATION_NOT_IMPL(eMoveCtor),
								   META_TYPE_BINDER_DEFAULT_OPERATION_DTOR(),
								   META_TYPE_BINDER_DEFAULT_OPERATION_TO_STRING());
META_TYPE_BINDER_END()
META_TYPE_AUTO_REGISTER_NS(Asset::Object, AssetObject);

STREAM_IMPL_BEGIN(Asset::Object)
	STREAM_WRITE_IMPL_VALUE_FUNCTION_BEGIN()
		RESULT_CONDITION_ENSURE_NOLOG(stream.Write(value.mId, RESULT_ARG_PASS), StreamFailedToWrite, false);
		RESULT_CONDITION_ENSURE_NOLOG(stream.Write(value.mPath, RESULT_ARG_PASS), StreamFailedToWrite, false);
	STREAM_IMPL_VALUE_FUNCTION_END()
	STREAM_READ_IMPL_VALUE_FUNCTION_BEGIN()
		RESULT_CONDITION_ENSURE_NOLOG(stream.Read(value.mId, RESULT_ARG_PASS), StreamFailedToRead, false);
		RESULT_CONDITION_ENSURE_NOLOG(stream.Read(value.mPath, RESULT_ARG_PASS), StreamFailedToRead, false);
	STREAM_IMPL_VALUE_FUNCTION_END()
STREAM_IMPL_END()

#endif
