
#ifndef ENGINE_PROGRAM_ARGS_H
#define ENGINE_PROGRAM_ARGS_H

#include "Engine/Common.h"

#include <EASTL/string_hash_map.h>

namespace Engine
{

class ProgramArgs
{
public:
	EXPLICIT ProgramArgs(Int32 argc, char** argv);

public:


private:
	eastl::string_hash_map<eastl::string> args_{DEBUG_NAME_VAL("Engine::ProgramArgs")};
};

}

#endif
