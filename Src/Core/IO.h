
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
	EXPLICIT Path(const char* file_path = "");
	~Path();

public:
	static constexpr u32 MAX = PLATFORM_FS_MAX_PATH;

public:
	NODISCARD bool Exists() const;
	NODISCARD Path FullPath() const;
	NODISCARD Path RelativePath(const char* to) const;
	NODISCARD eastl::string Extension();
	void RemoveExtension();

public:
	bool IsFile() const;
	bool IsDirectory() const;

public:
	const char* data() const;

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
	File(const char* file_path, FlagType flags);
	~File();

public:
	void Open(const char* file_path, FlagType flags);
	void Read(void* data, u64 size) const;
	void Write(const void* data, u64 size) const;
	void Seek(u64 value, SeekType origin) const;
	void Tell(u64* value) const;
	void Flush() const;
	void Close();

public:
	template < typename T >
	void Read(T& value)
	{
		if constexpr (eastl::is_fundamental_v<T> || eastl::is_pod_v<T>)
		{
			RESULT_ENSURE_CALL_NL(read((void*)&value, sizeof(T)));;
		}
		else
		{
			RESULT_ENSURE_CALL_NL(value.read(*this));
		}
	}

	template < typename T >
	void write(const T& value)
	{
		if constexpr (eastl::is_fundamental_v<T> || eastl::is_pod_v<T>)
		{
			RESULT_ENSURE_CALL_NL(write((void*)&value, sizeof(T)));
		}
		else
		{
			RESULT_ENSURE_CALL_NL(value.write(*this));
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
