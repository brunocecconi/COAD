
#include "Engine/ProgramArgs.h"

namespace Engine
{
ProgramArgs::ProgramArgs(const int32_t argc, char** argv, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(argv, NullPtr);
	RESULT_CONDITION_ENSURE_NOLOG(argc != 0, ZeroSize);
	for (auto i = 1; i < argc; i++)
	{
		eastl::string l_arg{argv[i], EASTLAllocatorType{DEBUG_NAME_VAL("Engine")}};
		if (const auto l_equal_index = l_arg.find('='); l_equal_index != eastl::string::npos)
		{
			args_.insert(l_arg.substr(0, l_equal_index).c_str(), l_arg.substr(l_equal_index + 1ull));
		}
		else
		{
			args_.insert(l_arg.data());
		}
	}
	RESULT_OK();
}

bool ProgramArgs::Has(const char* name, RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG(false);
	RESULT_CONDITION_ENSURE_NOLOG(name, NullPtr, false);
	RESULT_OK();
	return args_.find(name) != args_.cend();
}

eastl::string_view ProgramArgs::GetString(const char* name, RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG({});
	RESULT_CONDITION_ENSURE_NOLOG(name, NullPtr, {});
	RESULT_CONDITION_ENSURE_NOLOG(args_.find(name) != args_.cend(), ElementNotFound, {});
	RESULT_OK();
	return args_.at(name);
}

uint64_t ProgramArgs::GetInteger(const char* name, RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG({});
	RESULT_CONDITION_ENSURE_NOLOG(name, NullPtr, {});
	RESULT_CONDITION_ENSURE_NOLOG(args_.find(name) != args_.cend(), ElementNotFound, {});
	RESULT_OK();
	return strtoull(args_.at(name).c_str(), nullptr, 10);
}

float32_t ProgramArgs::GetFloat(const char* name, RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG({});
	RESULT_CONDITION_ENSURE_NOLOG(name, NullPtr, {});
	RESULT_CONDITION_ENSURE_NOLOG(args_.find(name) != args_.cend(), ElementNotFound, {});
	RESULT_OK();
	return strtof(args_.at(name).c_str(), nullptr);
}

} // namespace Engine
