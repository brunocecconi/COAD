
#include "Engine/Manager.h"
#include "Render/Manager.h"
#include "Core/RawBuffer.h"

#include <EASTL/unordered_set.h>
#include <EASTL/vector_multimap.h>
#include <EASTL/optional.h>
#include <EASTL/span.h>

eastl::unique_ptr<Render::Manager> ManagerThread<Render::Manager>::mInstance;

LOG_DEFINE(RenderPlatform);

namespace Render
{

#define RENDER_INCLUDE_PLATFORM_MANAGER
#if PLATFORM_WINDOWS
#include "Render/ManagerWindows.h"
#endif
#undef RENDER_INCLUDE_PLATFORM_MANAGER

Manager::Manager()
	: mPlatformManager{new(Allocators::Default{DEBUG_NAME_VAL("Render")}.allocate(sizeof(PlatformManager)))
						   PlatformManager{}}
{
}

Manager::~Manager()
{
}

void Manager::Initialize(RESULT_PARAM_IMPL)
{
	LOGC(Info, Render, "Initializing...");
	RESULT_ENSURE_CALL(mPlatformManager->Initialize(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(base_t::Initialize(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(mThread.SetAffinity(1, RESULT_ARG_PASS));

	printf("Platform render post intialize memory used: %f\n", Allocators::GetAllocatedSize("PlatformRender"));
}

void Manager::Run(RESULT_PARAM_IMPL)
{
	LOGC(Info, Render, "Running...");
	RESULT_ENSURE_CALL(base_t::Run(RESULT_ARG_PASS));
}

void Manager::RunInternal(RESULT_PARAM_IMPL)
{
	ManagerWait<Engine::Manager>{RESULT_ARG_PASS};

	RESULT_ENSURE_LAST();
	RESULT_ENSURE_CALL(Thread::SleepCurrent(1u, RESULT_ARG_PASS));
	RESULT_OK();
}

void Manager::Finalize(RESULT_PARAM_IMPL)
{
	LOGC(Info, Render, "Finalizing...");
	RESULT_ENSURE_CALL(base_t::Finalize(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(base_t::WaitThreadFinish(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(mPlatformManager->Finalize(RESULT_ARG_PASS));
}

} // namespace Render

CLASS_VALIDATION(Render::PlatformManager);
