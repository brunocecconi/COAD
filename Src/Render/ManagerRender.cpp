
#include "Render/Manager.h"
#include "Render/Mesh.h"
#include "Engine/Manager.h"
#include "Core/RawBuffer.h"
#include "Core/Io.h"
#include "Core/Paths.h"

#include "Editor/Manager.h"

#include <EASTL/unordered_set.h>
#include <EASTL/vector_multimap.h>
#include <EASTL/optional.h>
#include <EASTL/span.h>

MANAGER_IMPL(Render::Manager);
LOG_DEFINE(PlatformRender);

#include "Render/Windows/OpenGLManager.h"
#include "Render/Windows/VulkanManager.h"

namespace Render
{

Manager::Manager()
	: mTargetApiManager{new(Allocators::default_t{DEBUG_NAME_VAL("Render")}.allocate(sizeof(Api::Manager)))
							Api::Manager{}}
{
}

Manager::~Manager()
{
}

void Manager::Initialize(RESULT_PARAM_IMPL)
{
	LOGC(Info, Render, "Initializing...");
	RESULT_ENSURE_CALL(mTargetApiManager->Initialize(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(base_t::Initialize(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(mThread.SetAffinity(1, RESULT_ARG_PASS));

#ifdef DEBUG
	printf("Platform render post intialize memory used: %f\n", Allocators::GetAllocatedSize("GraphicsApi"));
#endif
}

void Manager::Run(RESULT_PARAM_IMPL)
{
	LOGC(Info, Render, "Running...");
	RESULT_ENSURE_CALL(base_t::Run(RESULT_ARG_PASS));
}

void Manager::PreRunLoop(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_OK();
}

void Manager::RunInternal(RESULT_PARAM_IMPL)
{
	ManagerWait<Manager>{RESULT_ARG_PASS};
	ManagerWait<Engine::Manager>{RESULT_ARG_PASS};

	RESULT_ENSURE_LAST();
	if (mRequestedDirtyFramebufferSize)
	{
		mRequestedDirtyFramebufferSize = false;
		RESULT_ENSURE_CALL(mTargetApiManager->MarkDirtyFramebufferSize(RESULT_ARG_PASS));
	}
	RESULT_ENSURE_CALL(mTargetApiManager->Update(RESULT_ARG_PASS));
	RESULT_OK();
}

void Manager::Finalize(RESULT_PARAM_IMPL)
{
	LOGC(Info, Render, "Finalizing...");
	RESULT_ENSURE_LAST();
	RESULT_ENSURE_CALL(base_t::Finalize(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(base_t::WaitThreadFinish(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(mTargetApiManager->Finalize(RESULT_ARG_PASS));
	RESULT_OK();
}

void Manager::ToggleFullscreen(Engine::Window& Window, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(IsInitialized(), NotInitialized);
	RESULT_CONDITION_ENSURE(Engine::Manager::Instance().IsInThread(), CurrentThreadIsNotTheRequiredOne);
	ManagerWait<Manager>{RESULT_ARG_PASS};
	RESULT_ENSURE_LAST();
	RESULT_ENSURE_CALL(Window.ToggleFullscreen(RESULT_ARG_PASS));
	//mRequestedRtvSize = Window.GetSize();
	RESULT_OK();
}

void Manager::SetVsync(const bool Value) const
{
	mTargetApiManager->mVsync = Value;
	LOGC(Info, Render, "Set vsync: %s", BOOL_TO_CSTR(Value));
}

void Manager::ToggleVsync() const
{
	SetVsync(!mTargetApiManager->mVsync);
}

void Manager::ResizeFrame(const glm::uvec2 NewSize, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(IsInitialized(), NotInitialized);
	RESULT_CONDITION_ENSURE(Engine::Manager::Instance().IsInThread(), CurrentThreadIsNotTheRequiredOne);
	ManagerWait<Manager>{RESULT_ARG_PASS};
	//mRequestedRtvSize = NewSize;
	RESULT_OK();
}

void Manager::MarkDirtyFramebufferSize(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(IsInitialized(), NotInitialized);
	RESULT_CONDITION_ENSURE(Engine::Manager::Instance().IsInThread(), CurrentThreadIsNotTheRequiredOne);
	ManagerWait<Manager>{RESULT_ARG_PASS};
	mRequestedDirtyFramebufferSize = true;
	RESULT_OK();
}

#if EDITOR
void Manager::SetEditorActive(const bool Value, RESULT_PARAM_IMPL)
{
	ManagerWait<Manager>{RESULT_ARG_PASS};
	RESULT_ENSURE_LAST();
	mTargetApiManager->mEditor = Value;
	RESULT_OK();
}

void Manager::ToggleEditorActive(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_CALL(SetEditorActive(!mTargetApiManager->mEditor, RESULT_ARG_PASS));
}

EApi Manager::GetTargetApi()
{
#if OPENGL_ENABLED
	return EApi::eOpengl;
#elif VULKAN_ENABLED
	return EApi::eVulkan;
#else
	return EApi::eNone;
#endif
}

#if PLATFORM_WINDOWS

#endif

#endif

#if EDITOR
#if PLATFORM_WINDOWS
#endif
#endif

} // namespace Render
