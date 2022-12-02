
/** \file IO.h
 *
 * Copyright 2023 CoffeeAddict. All rights reserved.
 * This file is part of COAD and it is private.
 * You cannot copy, modify or share this file.
 *
 */

#ifndef CORE_IO_H
#define CORE_IO_H

#include "Core/Common.h"

#include <cstdio>
#include <EASTL/string.h>
#include <EASTL/scoped_ptr.h>

namespace Io
{

#if PLATFORM_OS_DESKTOP
typedef FILE* FileHandle;

#define PLATFORM_FS_MAX_PATH MAX_PATH

#endif

/**
 * @brief Path class.
 *
 */
class Path
{
	CLASS_BODY(Path)

public:
	EXPLICIT Path(const char* file_path = "", RESULT_PARAM_DEFINE);
	~Path();

public:
	static constexpr Uint32 MAX = PLATFORM_FS_MAX_PATH;

public:
	NODISCARD bool Exists() const;
	NODISCARD Path FullPath() const;
	NODISCARD Path RelativePath(const char* to, RESULT_PARAM_DEFINE) const;
	NODISCARD eastl::string Extension();
	void					RemoveExtension(RESULT_PARAM_DEFINE);

public:
	bool IsFile(RESULT_PARAM_DEFINE) const;
	bool IsDirectory(RESULT_PARAM_DEFINE) const;

public:
	const char* data(RESULT_PARAM_DEFINE) const;

private:
	char value_[MAX] = {};
};

/**
 * @brief File class.
 *
 */
class File
{
	CLASS_BODY_NON_COPYABLE(File)

public:
	enum FlagType : Uint32
	{
		eFtNone	  = 0,
		eFtWrite  = 1,
		eFtRead	  = 2,
		eFtAppend = 4
	};

	enum SeekType : Uint32
	{
		eStBegin,
		eStCurrent,
		eStEnd
	};

public:
	File();
	File(const char* file_path, Uint32 flags, RESULT_PARAM_DEFINE);
	~File();

public:
	void Open(const char* file_path, Uint32 flags, RESULT_PARAM_DEFINE);
	void OpenRead(const char* file_path, RESULT_PARAM_DEFINE);
	void OpenWrite(const char* file_path, RESULT_PARAM_DEFINE);
	void OpenReadWrite(const char* file_path, RESULT_PARAM_DEFINE);
	void Read(void* data, Uint64 size, RESULT_PARAM_DEFINE) const;
	void Write(const void* data, Uint64 size, RESULT_PARAM_DEFINE) const;
	void Seek(Uint64 value, SeekType origin, RESULT_PARAM_DEFINE) const;
	Uint64 Tell(RESULT_PARAM_DEFINE) const;
	Uint64 Size(RESULT_PARAM_DEFINE) const;
	void Flush(RESULT_PARAM_DEFINE) const;
	void Close(RESULT_PARAM_DEFINE);

	static void Read(const char* file_path, void* data, Uint64 size, RESULT_PARAM_DEFINE);
	static void Write(const char* file_path, const void* data, Uint64 size, RESULT_PARAM_DEFINE);

	template < typename T >
	static void ReadAll(T& container, const char* file_path, RESULT_PARAM_DEFINE);

public:
	template<typename T>
	File& operator<<(const T& value)
	{
		Write(value);
		return *this;
	}
	template<typename T>
	File& operator>>(T& value)
	{
		Read(value);
		return *this;
	}

private:
	FileHandle handle_;
};

template<typename T>
void File::ReadAll(T& container, const char* file_path, RESULT_PARAM_IMPL)
{
	static_assert(sizeof(typename T::value_type) == 1, "Invalid value_type sizeof.");
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_ENSURE_CALL_NOLOG(const File l_file(file_path, eFtRead, RESULT_ARG_PASS));
	RESULT_ENSURE_CALL_NOLOG(const auto l_size = l_file.Size(RESULT_ARG_PASS));
	RESULT_CONDITION_ENSURE_NOLOG(l_size > 0, eResultErrorZeroSize);
	container.resize(l_size);
	RESULT_ENSURE_CALL_NOLOG(l_file.Read(container.data(), l_size, RESULT_ARG_PASS));
	RESULT_OK();
}

/**
 * @brief Directory class.
 *
 */
class Directory
{
	CLASS_BODY(Directory)

public:
	~Directory()
	{
	}
};

} // namespace Io

CLASS_VALIDATION(Io::Path);
CLASS_VALIDATION(Io::File);
CLASS_VALIDATION(Io::Directory);

#endif
