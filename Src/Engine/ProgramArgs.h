
#ifndef ENGINE_PROGRAM_ARGS_H
#define ENGINE_PROGRAM_ARGS_H

#include "Engine/Common.h"

#include <EASTL/string_hash_map.h>

namespace Engine
{

/**
 * @brief Program arguments class.
 *
 */
class ProgramArgs
{
	CLASS_BODY(ProgramArgs);

public:
	ProgramArgs() = default;
	EXPLICIT ProgramArgs(int32_t argc, char** argv, RESULT_PARAM_DEFINE);
	~ProgramArgs() = default;

public:
	NODISCARD bool Has(const char* name, RESULT_PARAM_DEFINE) const;
	NODISCARD eastl::string_view GetString(const char* name, RESULT_PARAM_DEFINE) const;
	NODISCARD uint64_t			 GetInteger(const char* name, RESULT_PARAM_DEFINE) const;
	NODISCARD float32_t			 GetFloat(const char* name, RESULT_PARAM_DEFINE) const;

private:
	eastl::string_hash_map<eastl::string> args_{DEBUG_NAME_VAL("Engine")};
};

} // namespace Engine

CLASS_VALIDATION(Engine::ProgramArgs);

#endif
