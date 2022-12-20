
#include "Asset/Object.h"

#define DELEGATE_CALL(X, ...)	\
	eastl::for_each(X.begin(), X.end(), [&](auto& f) {	\
		f(RESULT_ARG_PASS);	\
		RESULT_ENSURE_LAST_NOLOG();	\
	});	\
	RESULT_ENSURE_LAST_NOLOG(__VA_ARGS__);

namespace Asset
{

Object::~Object()
{
}

Object::Object(Stream::Dynamic& SourceArchive, const char* Path, RESULT_PARAM_IMPL)
	: mPath{Path}, mId{MakeId(Path, mPath.size())}, mSourceSize{SourceArchive.BufferSize()}
{
}

Object::Object(Stream::Dynamic& SourceArchive, const char* Path, const id_t Id, RESULT_PARAM_IMPL)
	: mPath{Path}, mId{Id}, mSourceSize{SourceArchive.BufferSize()}
{
}

bool Object::OnLoad(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	DELEGATE_CALL(mPreloadDelegate, false);
	RESULT_CONDITION_ENSURE_NOLOG(!mIsLoaded, eResultErrorAssetAlreadyLoaded, false);
	DELEGATE_CALL(mPostloadDelegate, false);
	RESULT_OK();
	return true;
}

bool Object::OnUnload(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	DELEGATE_CALL(mPreunloadDelegate, false);
	RESULT_CONDITION_ENSURE_NOLOG(mIsLoaded, eResultErrorAssetWasNotLoaded, false);
	DELEGATE_CALL(mPostunloadDelegate, false);
	RESULT_OK();
	return true;
}

eastl::string_view Object::Path() const
{
	return mPath;
}

id_t Object::Id() const
{
	return mId;
}

bool Object::IsLoaded() const
{
	return mIsLoaded;
}

size_t Object::SourceSize() const
{
	return mSourceSize;
}

eastl::string Object::ToString(const uint64_t Capacity) const
{
	RawBuffer<char> lStr{Capacity + 256ull, DEBUG_NAME_VAL("Asset")};
	sprintf(lStr.Data(), "Asset::Object(TypeInfo=%s,Id=%llu,Path=%s)", Meta::Typeof<Object>().ToString().c_str(), mId,
			mPath.data());
	return eastl::string{lStr.Data(), EASTLAllocatorType{DEBUG_NAME_VAL("Asset")}};
}

bool Object::AddPreLoadDelegate(eastl::function<load_delegate_t>&& Function, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	mPreloadDelegate.emplace_back(Function);
	RESULT_OK();
	return true;
}

bool Object::AddPostLoadDelegate(eastl::function<load_delegate_t>&& Function, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	mPostloadDelegate.emplace_back(Function);
	RESULT_OK();
	return true;
}

bool Object::AddPreUnloadDelegate(eastl::function<unload_delegate_t>&& Function, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	mPreunloadDelegate.emplace_back(Function);
	RESULT_OK();
	return true;
}

bool Object::AddPostUnloadDelegate(eastl::function<unload_delegate_t>&& Function, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	mPostunloadDelegate.emplace_back(Function);
	RESULT_OK();
	return true;
}

} // namespace Asset
