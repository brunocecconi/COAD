
/** \file IO.cpp
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#include "Core/Io.h"
#include "Core/Assert.h"

#if _MSC_VER
#pragma comment(lib, "shlwapi")
#endif

namespace Io
{

File::File() : handle_{}
{
}

File::File(const char* file_path, const Uint32 flags, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_CALL_NOLOG(Open(file_path, flags, RESULT_ARG_PASS));
}

File::~File()
{
	fclose(handle_);
	handle_ = nullptr;
}

void File::Open(const char* file_path, const Uint32 flags, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();

	RESULT_CONDITION_ENSURE_NOLOG(!handle_, eResultErrorPtrIsNotNull);
	RESULT_CONDITION_ENSURE_NOLOG(file_path, eResultErrorNullPtr);
	RESULT_ENSURE_CALL_NOLOG(Path l_path{file_path});
	RESULT_ENSURE_CALL_NOLOG(Path l_full_path = l_path.FullPath());

	RESULT_CONDITION_ENSURE_NOLOG(flags != eFtNone, eResultErrorIoFileInvalidFlags);

	char l_mode[64] = {};
	if (flags & eFtWrite && !(flags & eFtRead))
	{
		strcat(l_mode, "w");
	}
	else if (flags & eFtWrite && flags & eFtRead)
	{
		strcat(l_mode, "w+");
	}
	else if (flags & eFtRead && !(flags & eFtWrite))
	{
		strcat(l_mode, "r");
	}
	else if (flags & eFtRead && flags & eFtWrite)
	{
		strcat(l_mode, "r+");
	}
	else if (flags & eFtAppend)
	{
		strcat(l_mode, "a");
	}

	handle_ = fopen(file_path, l_mode);
	RESULT_CONDITION_ENSURE_NOLOG(handle_, eResultErrorIoFileOpenFailed);
	RESULT_OK();
}

void File::OpenRead(const char* file_path, RESULT_PARAM_IMPL)
{
	Open(file_path, eFtRead, RESULT_ARG_PASS);
}

void File::OpenWrite(const char* file_path, RESULT_PARAM_IMPL)
{
	Open(file_path, eFtWrite, RESULT_ARG_PASS);
}

void File::OpenReadWrite(const char* file_path, RESULT_PARAM_IMPL)
{
	Open(file_path, eFtRead|eFtWrite, RESULT_ARG_PASS);
}

void File::Read(void* data, const Uint64 size, RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(handle_ && data, eResultErrorNullPtr);

	if (size == 0)
	{
		RESULT_ERROR(eResultErrorZeroSize);
	}
	if (fread(data, 1, size, handle_) > size)
	{
		RESULT_ERROR(eResultErrorIoFileReadFailed);
	}
	RESULT_OK();
}

void File::Write(const void* data, const Uint64 size, RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(handle_ && data, eResultErrorNullPtr);

	if (size == 0)
	{
		RESULT_ERROR(eResultErrorZeroSize);
	}
	if (fwrite(data, 1, size, handle_) != size)
	{
		RESULT_ERROR(eResultErrorIoFileWriteFailed);
	}
	RESULT_OK();
}

void File::Seek(const Uint64 value, const SeekType origin, RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(handle_, eResultErrorNullPtr);

	if (_fseeki64(handle_, value, origin) != 0)
	{
		RESULT_ERROR(eResultErrorIoFileSeekFailed);
	}

	RESULT_OK();
}

Uint64 File::Tell(RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG(0);
	RESULT_CONDITION_ENSURE_NOLOG(handle_, eResultErrorNullPtr, 0);
	RESULT_OK();
	return static_cast<Uint64>(_ftelli64(handle_));
}

Uint64 File::Size(RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG(0);
	RESULT_CONDITION_ENSURE_NOLOG(handle_, eResultErrorNullPtr, 0);
	RESULT_ENSURE_CALL_NOLOG(const auto l_current_cursor = Tell(RESULT_ARG_PASS), 0);
	RESULT_ENSURE_CALL_NOLOG(Seek(0, eStEnd, RESULT_ARG_PASS), 0);
	RESULT_ENSURE_CALL_NOLOG(const auto l_size = Tell(RESULT_ARG_PASS), 0);
	RESULT_ENSURE_CALL_NOLOG(Seek(l_current_cursor, eStBegin, RESULT_ARG_PASS), 0);
	RESULT_OK();
	return l_size;
}

void File::Flush(RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(handle_, eResultErrorNullPtr);

	if (fflush(handle_) != 0)
	{
		RESULT_ERROR(eResultErrorIoFileFlushFailed);
	}

	RESULT_OK();
}

void File::Close(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(handle_, eResultErrorNullPtr);

	if (fclose(handle_) != 0)
	{
		RESULT_ERROR(eResultErrorIoFileCloseFailed);
	}

	handle_ = nullptr;
	RESULT_OK();
}

void File::Read(const char* file_path, void* data, const Uint64 size, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_ENSURE_CALL_NOLOG(const File l_file(file_path, eFtRead, RESULT_ARG_PASS));
	RESULT_ENSURE_CALL_NOLOG(l_file.Read(data, size, RESULT_ARG_PASS));
	RESULT_OK();
}

void File::Write(const char* file_path, const void* data, const Uint64 size, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_ENSURE_CALL_NOLOG(const File l_file(file_path, eFtWrite, RESULT_ARG_PASS));
	RESULT_ENSURE_CALL_NOLOG(l_file.Write(data, size, RESULT_ARG_PASS));
	RESULT_OK();
}

Path::Path(const char* file_path, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();

	if (file_path == nullptr)
	{
		RESULT_ENSURE_NOLOG(eResultErrorNullPtr);
	}

	if (const auto l_size = std::strlen(file_path); l_size > MAX)
	{
		RESULT_ENSURE_NOLOG(eResultErrorIoExceededMaxPathLength);
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

Path Path::RelativePath(const char* to, RESULT_PARAM_IMPL) const
{
	Path l_path{};
	RESULT_ENSURE_LAST_NOLOG(l_path);

	if (!to)
	{
		RESULT_ENSURE_NOLOG(eResultErrorNullPtr, l_path);
	}
	if (strlen(to) > MAX)
	{
		RESULT_ENSURE_NOLOG(eResultErrorIoExceededMaxPathLength, l_path);
	}
	PathRelativePathToA(l_path.value_, value_, FILE_ATTRIBUTE_DIRECTORY, to, FILE_ATTRIBUTE_NORMAL);
	return l_path;
}

eastl::string Path::Extension()
{
	return eastl::string{DEBUG_NAME_VAL("Path")};
}

void Path::RemoveExtension(RESULT_PARAM_IMPL)
{
}

bool Path::IsFile(RESULT_PARAM_IMPL) const
{
	return false;
}

bool Path::IsDirectory(RESULT_PARAM_IMPL) const
{
	return false;
}

const char* Path::data(RESULT_PARAM_IMPL) const
{
	return value_;
}

} // namespace Io
