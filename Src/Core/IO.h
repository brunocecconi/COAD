
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
typedef FILE* file_handle_t;

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
	EXPLICIT Path(const char* FilePath = "", RESULT_PARAM_DEFINE);
	~Path();

public:
	static constexpr uint32_t MAX = PLATFORM_FS_MAX_PATH;

public:
	NODISCARD bool Exists() const;
	NODISCARD Path FullPath() const;
	NODISCARD Path RelativePath(const char* To, RESULT_PARAM_DEFINE) const;
	NODISCARD eastl::string Extension();
	void					RemoveExtension(RESULT_PARAM_DEFINE);

public:
	bool IsFile(RESULT_PARAM_DEFINE) const;
	bool IsDirectory(RESULT_PARAM_DEFINE) const;

public:
	const char* Data(RESULT_PARAM_DEFINE) const;

private:
	char mValue[MAX] = {};
};

/**
 * @brief File class.
 *
 */
class File
{
	CLASS_BODY_NON_COPYABLE(File)

public:
	enum EFlagType : uint32_t
	{
		eFtNone	  = 0,
		eFtWrite  = 1,
		eFtRead	  = 2,
		eFtAppend = 4
	};

	enum ESeekType : uint32_t
	{
		eStBegin,
		eStCurrent,
		eStEnd
	};

public:
	File();
	File(const char* FilePath, uint32_t Flags, RESULT_PARAM_DEFINE);
	~File();

public:
	void	 Open(const char* FilePath, uint32_t Flags, RESULT_PARAM_DEFINE);
	void	 OpenRead(const char* FilePath, RESULT_PARAM_DEFINE);
	void	 OpenWrite(const char* FilePath, RESULT_PARAM_DEFINE);
	void	 OpenReadWrite(const char* FilePath, RESULT_PARAM_DEFINE);
	void	 Read(void* Data, uint64_t Size, RESULT_PARAM_DEFINE) const;
	void	 Write(const void* Data, uint64_t Size, RESULT_PARAM_DEFINE) const;
	void	 Seek(uint64_t Value, ESeekType Origin, RESULT_PARAM_DEFINE) const;
	uint64_t Tell(RESULT_PARAM_DEFINE) const;
	uint64_t Size(RESULT_PARAM_DEFINE) const;
	void	 Flush(RESULT_PARAM_DEFINE) const;
	void	 Close(RESULT_PARAM_DEFINE);

	static void Read(const char* FilePath, void* Data, uint64_t Size, RESULT_PARAM_DEFINE);
	static void Write(const char* FilePath, const void* Data, uint64_t Size, RESULT_PARAM_DEFINE);

	template<typename T>
	static void ReadAll(T& Container, const char* FilePath, RESULT_PARAM_DEFINE);

public:
	template<typename T>
	File& operator<<(const T& Value)
	{
		Write(Value);
		return *this;
	}
	template<typename T>
	File& operator>>(T& Value)
	{
		Read(Value);
		return *this;
	}

private:
	file_handle_t mHandle;
};

template<typename T>
void File::ReadAll(T& Container, const char* FilePath, RESULT_PARAM_IMPL)
{
	static_assert(sizeof(typename T::value_type) == 1, "Invalid value_type sizeof.");
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_ENSURE_CALL_NOLOG(const File lFile(FilePath, eFtRead, RESULT_ARG_PASS));
	RESULT_ENSURE_CALL_NOLOG(const auto lSize = lFile.Size(RESULT_ARG_PASS));
	RESULT_CONDITION_ENSURE_NOLOG(lSize > 0, ZeroSize);
	Container.resize(lSize);
	RESULT_ENSURE_CALL_NOLOG(lFile.Read(Container.data(), lSize, RESULT_ARG_PASS));
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
