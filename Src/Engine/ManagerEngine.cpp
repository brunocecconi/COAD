
#include "Engine/Manager.h"
#include "Render/Manager.h"

eastl::unique_ptr<Engine::Manager> ManagerNoThread<Engine::Manager>::mInstance;

namespace Engine
{

Manager::Manager()
{
}

Manager::~Manager()
{
}

void Manager::Initialize(const int32_t Argc, char** Argv, RESULT_PARAM_IMPL)
{
	LOGC(Info, Engine, "Intializing...");

	// Call this initialize base
	RESULT_ENSURE_CALL(base_t::Initialize(RESULT_ARG_PASS));

	// Get program args
	RESULT_ENSURE_CALL(mArgs = ProgramArgs(Argc, Argv));

	// Create window
	RESULT_ENSURE_CALL(mWindow.Create({"Test window", 800, 600}, RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(mWindow.Show(RESULT_ARG_PASS));

	// Initialize managers
	RESULT_ENSURE_CALL(Render::Manager::Instance().Initialize(RESULT_ARG_PASS));

	RESULT_OK();
}

int32_t Manager::Run(RESULT_PARAM_IMPL)
{
	LOGC(Info, Engine, "Running...");
	RESULT_ENSURE_LAST(EXIT_FAILURE);

	// Run managers
	RESULT_ENSURE_CALL(Render::Manager::Instance().Run(RESULT_ARG_PASS), EXIT_FAILURE);
	
	// Call run base
	RESULT_ENSURE_CALL(base_t::Run(RESULT_ARG_PASS), EXIT_FAILURE);

	RESULT_OK();
	return EXIT_SUCCESS;
}

void Manager::Finalize(RESULT_PARAM_IMPL)
{
	LOGC(Info, Engine, "Finalizing...");
	RESULT_ENSURE_LAST();

	// Finalize managers
	RESULT_ENSURE_CALL(Render::Manager::Instance().Finalize(RESULT_ARG_PASS));

	// Destroy window
	RESULT_ENSURE_CALL(mWindow.Destroy(RESULT_ARG_PASS));

	// Call base finalize
	RESULT_ENSURE_CALL(base_t::Finalize(RESULT_ARG_PASS));
	
	RESULT_OK();
}

void Manager::RunInternal(RESULT_PARAM_IMPL)
{
	ManagerWait<Manager>{RESULT_ARG_PASS};

	RESULT_ENSURE_LAST();
	if(mWindow.IsVisible())
	{
		RESULT_ENSURE_CALL(mWindow.Update(RESULT_ARG_PASS));
		RESULT_ENSURE_CALL(Thread::SleepCurrent(1, RESULT_ARG_PASS));
	}
	else
	{
		RESULT_ENSURE_CALL(Finalize(RESULT_ARG_PASS));
	}
	RESULT_OK();
}

Manager& Manager::Instance()
{
	if (!mInstance)
	{
		mInstance.reset(new (Allocators::Default{DEBUG_NAME_VAL("Engine")}.allocate(sizeof(Manager))) Manager{});
	}
	return *mInstance;
}

} // namespace Engine
