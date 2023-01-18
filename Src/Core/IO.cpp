
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

File::File() : mHandle{}
{
}

File::File(const char* FilePath, const uint32_t Flags, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_CALL_NOLOG(Open(FilePath, Flags, RESULT_ARG_PASS));
}

File::~File()
{
	fclose(mHandle);
	mHandle = nullptr;
}

void File::Open(const char* FilePath, const uint32_t Flags, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();

	RESULT_CONDITION_ENSURE_NOLOG(!mHandle, PtrIsNotNull);
	RESULT_CONDITION_ENSURE_NOLOG(FilePath, NullPtr);
	RESULT_ENSURE_CALL_NOLOG(const Path lPath{FilePath});
	RESULT_ENSURE_CALL_NOLOG(Path lFullPath = lPath.FullPath());

	RESULT_CONDITION_ENSURE_NOLOG(Flags != eFtNone, IoFileInvalidFlags);

	char lMode[64] = {};
	if (Flags & eFtWrite && !(Flags & eFtRead))
	{
		strcat(lMode, "w");
	}
	else if (Flags & eFtWrite && Flags & eFtRead)
	{
		strcat(lMode, "w+");
	}
	else if (Flags & eFtRead && !(Flags & eFtWrite))
	{
		strcat(lMode, "r");
	}
	else if (Flags & eFtRead && Flags & eFtWrite)
	{
		strcat(lMode, "r+");
	}
	else if (Flags & eFtAppend)
	{
		strcat(lMode, "a");
	}

	mHandle = fopen(FilePath, lMode);
	RESULT_CONDITION_ENSURE_NOLOG(mHandle, IoFileOpenFailed);
	RESULT_OK();
}

void File::OpenRead(const char* FilePath, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(!mHandle, PtrIsNotNull);
	RESULT_CONDITION_ENSURE_NOLOG(FilePath, NullPtr);
	RESULT_ENSURE_CALL_NOLOG(const Path lPath{FilePath});
	RESULT_ENSURE_CALL_NOLOG(Path lFullPath = lPath.FullPath());
	mHandle = fopen(FilePath, "r");
	RESULT_CONDITION_ENSURE_NOLOG(mHandle, IoFileOpenFailed);
	RESULT_OK();
}

void File::OpenWrite(const char* FilePath, RESULT_PARAM_IMPL)
{
	Open(FilePath, eFtWrite, RESULT_ARG_PASS);
}

void File::OpenReadWrite(const char* FilePath, RESULT_PARAM_IMPL)
{
	Open(FilePath, eFtRead | eFtWrite, RESULT_ARG_PASS);
}

void File::Read(void* Data, const uint64_t Size, RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(mHandle && Data, NullPtr);

	if (Size == 0)
	{
		RESULT_ERROR(ZeroSize);
	}
	if (fread(Data, 1, Size, mHandle) > Size)
	{
		RESULT_ERROR(IoFileReadFailed);
	}
	RESULT_OK();
}

uint64_t File::ReadText(void* Data, uint64_t Size, RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG({});
	RESULT_CONDITION_ENSURE_NOLOG(mHandle && Data, NullPtr, {});

	if (Size == 0)
	{
		RESULT_ERROR(ZeroSize, {});
	}

	const auto lData	   = static_cast<char*>(Data);
	char	   lLine[1024] = {};
	uint64_t   lIndex{};
	while (!feof(mHandle))
	{
		if (fgets(lLine, sizeof lLine, mHandle))
		{
			const auto lLength = strlen(lLine);
			memcpy(lData + lIndex, lLine, lLength);
			lIndex += lLength;
		}
	}

	RESULT_OK();
	return lIndex;
}

void File::Write(const void* Data, const uint64_t Size, RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(mHandle && Data, NullPtr);

	if (Size == 0)
	{
		RESULT_ERROR(ZeroSize);
	}
	if (fwrite(Data, 1, Size, mHandle) != Size)
	{
		RESULT_ERROR(IoFileWriteFailed);
	}
	RESULT_OK();
}

void File::Seek(const uint64_t Value, const ESeekType Origin, RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(mHandle, NullPtr);

	if (_fseeki64(mHandle, Value, Origin) != 0)
	{
		RESULT_ERROR(IoFileSeekFailed);
	}

	RESULT_OK();
}

uint64_t File::Tell(RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG(0);
	RESULT_CONDITION_ENSURE_NOLOG(mHandle, NullPtr, 0);
	RESULT_OK();
	return static_cast<uint64_t>(_ftelli64(mHandle));
}

uint64_t File::Size(RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG(0);
	RESULT_CONDITION_ENSURE_NOLOG(mHandle, NullPtr, 0);
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
	RESULT_CONDITION_ENSURE_NOLOG(mHandle, NullPtr);

	if (fflush(mHandle) != 0)
	{
		RESULT_ERROR(IoFileFlushFailed);
	}

	RESULT_OK();
}

void File::Close(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(mHandle, NullPtr);

	if (fclose(mHandle) != 0)
	{
		RESULT_ERROR(IoFileCloseFailed);
	}

	mHandle = nullptr;
	RESULT_OK();
}

void File::Read(const char* FilePath, void* Data, const uint64_t Size, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_ENSURE_CALL_NOLOG(const File l_file(FilePath, eFtRead, RESULT_ARG_PASS));
	RESULT_ENSURE_CALL_NOLOG(l_file.Read(Data, Size, RESULT_ARG_PASS));
	RESULT_OK();
}

void File::Write(const char* FilePath, const void* Data, const uint64_t Size, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_ENSURE_CALL_NOLOG(const File l_file(FilePath, eFtWrite, RESULT_ARG_PASS));
	RESULT_ENSURE_CALL_NOLOG(l_file.Write(Data, Size, RESULT_ARG_PASS));
	RESULT_OK();
}

Path::Path(const char* FilePath, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();

	if (FilePath == nullptr)
	{
		RESULT_ENSURE_NOLOG(NullPtr);
	}

	if (const auto lSize = std::strlen(FilePath); lSize > MAX)
	{
		RESULT_ENSURE_NOLOG(IoExceededMaxPathLength);
	}

	strcpy(mValue, FilePath);

	RESULT_OK();
}

Path::~Path()
{
}

bool Path::Exists() const
{
	const Path& lPath = FullPath();
	return PathFileExistsA(lPath.mValue) == TRUE;
}

Path Path::FullPath() const
{
	Path lPath;
	GetFullPathNameA(mValue, MAX, lPath.mValue, nullptr);
	return lPath;
}

Path Path::RelativePath(const char* To, RESULT_PARAM_IMPL) const
{
	Path lPath{};
	RESULT_ENSURE_LAST_NOLOG(lPath);

	if (!To)
	{
		RESULT_ENSURE_NOLOG(NullPtr, lPath);
	}
	if (strlen(To) > MAX)
	{
		RESULT_ENSURE_NOLOG(IoExceededMaxPathLength, lPath);
	}
	PathRelativePathToA(lPath.mValue, mValue, FILE_ATTRIBUTE_DIRECTORY, To, FILE_ATTRIBUTE_NORMAL);
	RESULT_OK();
	return lPath;
}

eastl::string Path::Extension()
{
	return eastl::string{DEBUG_NAME_VAL("Path")};
}

void Path::RemoveExtension(RESULT_PARAM_IMPL)
{
	RESULT_UNUSED();
}

bool Path::IsFile(RESULT_PARAM_IMPL) const
{
	RESULT_UNUSED();
	return false;
}

bool Path::IsDirectory(RESULT_PARAM_IMPL) const
{
	RESULT_UNUSED();
	return false;
}

const char* Path::Data(RESULT_PARAM_IMPL) const
{
	RESULT_UNUSED();
	return mValue;
}

} // namespace Io
