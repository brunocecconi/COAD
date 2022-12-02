
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

Object::Object(Stream::Dynamic& source_archive, const char* path, RESULT_PARAM_IMPL)
	: path_{path}, id_{MakeId(path, path_.size())}, source_size_{source_archive.BufferSize()}
{
}

Object::Object(Stream::Dynamic& source_archive, const char* path, const id_t id, RESULT_PARAM_IMPL)
	: path_{path}, id_{id}, source_size_{source_archive.BufferSize()}
{
}

bool Object::OnLoad(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	DELEGATE_CALL(preload_delegate_, false);
	RESULT_CONDITION_ENSURE_NOLOG(!is_loaded_, eResultErrorAssetAlreadyLoaded, false);
	DELEGATE_CALL(postload_delegate_, false);
	RESULT_OK();
	return true;
}

bool Object::OnUnload(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	DELEGATE_CALL(preunload_delegate_, false);
	RESULT_CONDITION_ENSURE_NOLOG(is_loaded_, eResultErrorAssetWasNotLoaded, false);
	DELEGATE_CALL(postunload_delegate_, false);
	RESULT_OK();
	return true;
}

eastl::string_view Object::Path() const
{
	return path_;
}

id_t Object::Id() const
{
	return id_;
}

bool Object::IsLoaded() const
{
	return is_loaded_;
}

Size Object::SourceSize() const
{
	return source_size_;
}

eastl::string Object::ToString(const Uint64 capacity) const
{
	RawBuffer<char> l_str{capacity + 256ull, DEBUG_NAME_VAL("Asset")};
	sprintf(l_str.Get(), "Asset::Object(TypeInfo=%s,Id=%llu,Path=%s)", Meta::Typeof<Object>().ToString().c_str(), id_,
			path_.data());
	return eastl::string{l_str.Get(), EASTLAllocatorType{DEBUG_NAME_VAL("Asset")}};
}

bool Object::AddPreLoadDelegate(eastl::function<load_delegate_t>&& function, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	preload_delegate_.emplace_back(function);
	RESULT_OK();
	return true;
}

bool Object::AddPostLoadDelegate(eastl::function<load_delegate_t>&& function, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	postload_delegate_.emplace_back(function);
	RESULT_OK();
	return true;
}

bool Object::AddPreUnloadDelegate(eastl::function<unload_delegate_t>&& function, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	preunload_delegate_.emplace_back(function);
	RESULT_OK();
	return true;
}

bool Object::AddPostUnloadDelegate(eastl::function<unload_delegate_t>&& function, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG(false);
	postunload_delegate_.emplace_back(function);
	RESULT_OK();
	return true;
}

} // namespace Asset
