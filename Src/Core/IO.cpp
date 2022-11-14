
#include "Core/IO.h"
#include "Core/Assert.h"

#if _MSC_VER
#pragma comment(lib, "shlwapi")
#endif

namespace Io
{

File::File(const char* file_path, FlagType flags)
{
	RESULT_ENSURE_CALL_NL(Open(file_path, flags));
}

File::~File()
{
	fclose(handle_);
	handle_ = nullptr;
}

void File::Open(const char* file_path, FlagType flags)
{
	if(!(handle_ && file_path))
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}

	Path l_path {file_path};
	ENSURE_LAST_RESULT_NL();

	Path l_full_path = l_path.FullPath();
	ENSURE_LAST_RESULT_NL();

	if(flags == eFtNone)
	{
		RESULT_ERROR(eResultErrorIoFileInvalidFlags);
	}

	char l_mode[64] = {};
	if(flags & eFtWrite && !(flags & eFtRead))
	{
		strcat(l_mode, "w");
	}
	else if(flags & eFtWrite && flags & eFtRead)
	{
		strcat(l_mode, "w+");
	}
	else if(flags & eFtRead && !(flags & eFtWrite))
	{
		strcat(l_mode, "r");
	}
	else if(flags & eFtRead && flags & eFtWrite)
	{
		strcat(l_mode, "r+");
	}
	else if(flags & eFtAppend)
	{
		strcat(l_mode, "a");
	}

	handle_ = fopen(file_path, l_mode);
	if(!handle_)
	{
		RESULT_ERROR(eResultErrorIoFileOpenFailed);
	}
	RESULT_OK();
}

void File::Read(void* data, const u64 size) const
{
	if(!(handle_ && data))
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}
	if(size == 0)
	{
		RESULT_ERROR(eResultErrorZeroSize);
	}
	if(fread(data, 1, size, handle_) > size)
	{
		RESULT_ERROR(eResultErrorIoFileReadFailed);
	}
	RESULT_OK();
}

void File::Write(const void* data, const u64 size) const
{
	if(!(handle_ && data))
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}
	if(size == 0)
	{
		RESULT_ERROR(eResultErrorZeroSize);
	}
	if(fwrite(data, 1, size, handle_) != size)
	{
		RESULT_ERROR(eResultErrorIoFileWriteFailed);
	}
	RESULT_OK();
}

void File::Seek(const u64 value, const SeekType origin) const
{
	if(!handle_)
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}

	if(_fseeki64(handle_, value, origin) != 0)
	{
		RESULT_ERROR(eResultErrorIoFileSeekFailed);
	}

	RESULT_OK();
}

void File::Tell(u64* value) const
{
	if(!(handle_ && value))
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}
	*value = _ftelli64(handle_);
	RESULT_OK();
}

void File::Flush() const
{
	if(!handle_)
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}

	if(fflush(handle_) != 0)
	{
		RESULT_ERROR(eResultErrorIoFileFlushFailed);
	}

	RESULT_OK();
}

void File::Close()
{
	if(!handle_)
	{
		RESULT_ERROR(eResultErrorNullPtr);
	}
	if(fclose(handle_) != 0)
	{
		RESULT_ERROR(eResultErrorIoFileCloseFailed);
	}

	handle_ = nullptr;
	RESULT_OK();
}

Path::Path(const char* file_path)
{
	if(!file_path)
	{
		ENSURE_RESULT_NL(eResultErrorNullPtr);
	}

	if(const auto l_size = std::strlen(file_path); 
		l_size > MAX)
	{
		ENSURE_RESULT_NL(eResultErrorIoExceededMaxPathLength);
	}

	strcpy(value_, file_path);
}

Path::~Path()
{
}

bool Path::Exists() const
{
	const Path& l_path = FullPath();
	return PathFileExistsA(l_path.value_) == TRUE;
}

Path Path::FullPath() const
{
	Path l_path;
	GetFullPathNameA(value_, MAX, l_path.value_, nullptr);
	return l_path;
}

Path Path::RelativePath(const char* to) const
{
	Path l_path{};
	if(!to)
	{
		ENSURE_RESULT_NL(eResultErrorNullPtr, l_path);
	}
	if(strlen(to) > MAX)
	{
		ENSURE_RESULT_NL(eResultErrorIoExceededMaxPathLength, l_path);
	}
	PathRelativePathToA(l_path.value_, value_, 
		FILE_ATTRIBUTE_DIRECTORY, to, FILE_ATTRIBUTE_NORMAL);
	return l_path;
}

eastl::string Path::Extension()
{
	return eastl::string{NAME_VAL("Path")};
}

void Path::RemoveExtension()
{
}

bool Path::IsFile() const
{
	return false;
}

bool Path::IsDirectory() const
{
	return false;
}

const char* Path::data() const
{
	return value_;
}

}
