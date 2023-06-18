
#define EASTL_ASSERT_ENABLED	 0
#define COR_ENABLE_PROFILING	 0
#define CORECLR_ENABLE_PROFILING 0
#define PRINT_ALLOCATIONS		 1
#define EASTL_EASTDC_VSNPRINTF	 0

#include "Engine/Manager.h"
#include "Engine/ProgramArgs.h"

int Vsnprintf8(char* pDestination, size_t n, const char* pFormat, va_list arguments)
{
	return vsnprintf(pDestination, n, pFormat, arguments);
}

LOG_DEFINE(Main);

int32_t ProcessGame(const int32_t Argc, char** Argv)
{
	RESULT_VALUE_VAR(result);
	LOGC(Info, Main, "Preparing to run the game...");
	auto& lEngine = Engine::Manager::Instance();
	RESULT_VALUE_ENSURE_CALL(lEngine.Initialize(Argc, Argv, RESULT_ARG_VALUE_PASS), EXIT_FAILURE);
	RESULT_VALUE_ENSURE_CALL(lEngine.Run(RESULT_ARG_VALUE_PASS), EXIT_FAILURE);
	return EXIT_SUCCESS;
}

int32_t main(const int32_t Argc, char** Argv)
{
	LOG_INIT();
	LOG_SET_VERBOSITY(Info);
	const Engine::ProgramArgs lArgs{ Argc, Argv };
	if (lArgs.Has("--Game"))
	{
		return ProcessGame(Argc, Argv);
	}
	return 0;
}
