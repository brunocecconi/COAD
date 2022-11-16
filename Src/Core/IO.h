
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
	EXPLICIT Path(const char* file_path = "" RESULT_ARG_OPT);
	~Path();

public:
	static constexpr u32 MAX = PLATFORM_FS_MAX_PATH;

public:
	NODISCARD bool Exists(RESULT_ARG_SINGLE_OPT) const;
	NODISCARD Path FullPath(RESULT_ARG_SINGLE_OPT) const;
	NODISCARD Path RelativePath(const char* to RESULT_ARG_OPT) const;
	NODISCARD eastl::string Extension(RESULT_ARG_SINGLE_OPT);
	void RemoveExtension(RESULT_ARG_SINGLE_OPT);

public:
	bool IsFile(RESULT_ARG_SINGLE_OPT) const;
	bool IsDirectory(RESULT_ARG_SINGLE_OPT) const;

public:
	const char* data(RESULT_ARG_SINGLE_OPT) const;

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
	enum FlagType
	{
		eFtNone		= 0,
		eFtWrite	= 1,
		eFtRead		= 2,
		eFtAppend	= 4
	};

	enum SeekType
	{
		eStBegin,
		eStCurrent,
		eStEnd
	};

public:
	File(const char* file_path, FlagType flags RESULT_ARG_OPT);
	~File();

public:
	void Open(const char* file_path, FlagType flags RESULT_ARG_OPT);
	void Read(void* data, u64 size RESULT_ARG_OPT) const;
	void Write(const void* data, u64 size RESULT_ARG_OPT) const;
	void Seek(u64 value, SeekType origin RESULT_ARG_OPT) const;
	void Tell(u64* value RESULT_ARG_OPT) const;
	void Flush(RESULT_ARG_SINGLE_OPT) const;
	void Close(RESULT_ARG_SINGLE_OPT);

public:
	template < typename T >
	void Read(T& value RESULT_ARG_OPT)
	{
		if constexpr (eastl::is_fundamental_v<T> || eastl::is_pod_v<T>)
		{
			RESULT_ENSURE_CALL_NL(Read((void*)&value, sizeof(T) RESULT_ARG_PASS));;
		}
		else
		{
			RESULT_ENSURE_CALL_NL(value.Read(*this));
		}
	}

	template < typename T >
	void Write(const T& value RESULT_ARG_OPT)
	{
		if constexpr (eastl::is_fundamental_v<T> || eastl::is_pod_v<T>)
		{
			RESULT_ENSURE_CALL_NL(Write((void*)&value, sizeof(T) RESULT_ARG_PASS));
		}
		else
		{
			RESULT_ENSURE_CALL_NL(value.Write(*this));
		}
	}

private:
	FileHandle handle_;
};

/**
 * @brief Directory class.
 *
*/
class Directory
{
	CLASS_BODY(Directory)

public:
	~Directory() {}
};

}

CLASS_VALIDATION(Io::Path);
CLASS_VALIDATION(Io::File);
CLASS_VALIDATION(Io::Directory);

#endif
