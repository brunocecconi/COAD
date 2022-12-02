
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

template<Size N>
constexpr id_t MakeId(const char (&path)[N])
{
	return Hash::Fnv1A(path);
}

constexpr id_t MakeId(const char* path, const Size size)
{
	return Hash::Fnv1A(size, path);
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
	EXPLICIT Object(Stream::Dynamic& source_archive, const char* path, RESULT_PARAM_DEFINE);
	EXPLICIT Object(Stream::Dynamic& source_archive, const char* path, id_t id, RESULT_PARAM_DEFINE);

private:
	MAYBEUNUSED bool OnLoad(RESULT_PARAM_DEFINE);
	MAYBEUNUSED bool OnUnload(RESULT_PARAM_DEFINE);

public:
	NODISCARD eastl::string_view Path() const;
	NODISCARD id_t				 Id() const;
	NODISCARD bool				 IsLoaded() const;
	NODISCARD Size				 SourceSize() const;

public:
	NODISCARD eastl::string ToString(Uint64 capacity = 256) const;

public:
	MAYBEUNUSED bool AddPreLoadDelegate(eastl::function<load_delegate_t>&& function, RESULT_PARAM_DEFINE);
	MAYBEUNUSED bool AddPostLoadDelegate(eastl::function<load_delegate_t>&& function, RESULT_PARAM_DEFINE);
	MAYBEUNUSED bool AddPreUnloadDelegate(eastl::function<unload_delegate_t>&& function, RESULT_PARAM_DEFINE);
	MAYBEUNUSED bool AddPostUnloadDelegate(eastl::function<unload_delegate_t>&& function, RESULT_PARAM_DEFINE);

private:
	STREAM_IMPL_FRIEND();

	eastl::string									  path_{DEBUG_NAME_VAL("Asset")};
	id_t											  id_{};
	bool											  is_loaded_{};
	Size											  source_size_{};
	eastl::vector<eastl::function<load_delegate_t>>	  preload_delegate_{DEBUG_NAME_VAL("Asset")};
	eastl::vector<eastl::function<load_delegate_t>>	  postload_delegate_{DEBUG_NAME_VAL("Asset")};
	eastl::vector<eastl::function<unload_delegate_t>> preunload_delegate_{DEBUG_NAME_VAL("Asset")};
	eastl::vector<eastl::function<unload_delegate_t>> postunload_delegate_{DEBUG_NAME_VAL("Asset")};

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
		RESULT_CONDITION_ENSURE_NOLOG(stream.Write(value.id_, RESULT_ARG_PASS), eResultErrorStreamFailedToWrite, false);
		RESULT_CONDITION_ENSURE_NOLOG(stream.Write(value.path_, RESULT_ARG_PASS), eResultErrorStreamFailedToWrite, false);
	STREAM_IMPL_VALUE_FUNCTION_END()
	STREAM_READ_IMPL_VALUE_FUNCTION_BEGIN()
		RESULT_CONDITION_ENSURE_NOLOG(stream.Read(value.id_, RESULT_ARG_PASS), eResultErrorStreamFailedToRead, false);
		RESULT_CONDITION_ENSURE_NOLOG(stream.Read(value.path_, RESULT_ARG_PASS), eResultErrorStreamFailedToRead, false);
	STREAM_IMPL_VALUE_FUNCTION_END()
STREAM_IMPL_END()

#endif
