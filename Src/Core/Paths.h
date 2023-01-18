
#ifndef CORE_PATHS_H
#define CORE_PATHS_H

#include "Core/Common.h"

#include <EASTL/string.h>

class Paths
{
public:
	template<typename Allocator = EASTLAllocatorType>
	static eastl::string WorkingDir(const Allocator& allocator = {DEBUG_NAME_VAL("Paths")})
	{
		return {"."};
	}

	template<typename Allocator = EASTLAllocatorType>
	static eastl::string AssetDir(const Allocator& allocator = {DEBUG_NAME_VAL("Paths")})
	{
		return {"../../Assets"};
	}

	template<typename Allocator = EASTLAllocatorType>
	static eastl::string ConfigsDir(const Allocator& allocator = {DEBUG_NAME_VAL("Paths")})
	{
		return {"../../Configs"};
	}

	template<typename Allocator = EASTLAllocatorType>
	static eastl::string LogsDir(const Allocator& allocator = {DEBUG_NAME_VAL("Paths")})
	{
		return {"../../Logs"};
	}
};

#endif
