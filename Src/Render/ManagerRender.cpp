
#include "Engine/Manager.h"
#include "Render/Manager.h"
#include "Core/RawBuffer.h"

#include "Editor/Manager.h"

#include <EASTL/unordered_set.h>
#include <EASTL/vector_multimap.h>
#include <EASTL/optional.h>
#include <EASTL/span.h>

MANAGER_IMPL(Render::Manager);
LOG_DEFINE(PlatformRender);

#define RENDER_INCLUDE_PLATFORM_MANAGER
#if PLATFORM_WINDOWS
#include "Render/Windows/Manager.h"
#endif
#undef RENDER_INCLUDE_PLATFORM_MANAGER

namespace Render
{

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

#ifdef DEBUG
	printf("Platform render post intialize memory used: %f\n", Allocators::GetAllocatedSize("PlatformRender"));
#endif
}

void Manager::Run(RESULT_PARAM_IMPL)
{
	LOGC(Info, Render, "Running...");
	RESULT_ENSURE_CALL(base_t::Run(RESULT_ARG_PASS));
}

void Manager::RunInternal(RESULT_PARAM_IMPL)
{
	ManagerWait<Manager>{RESULT_ARG_PASS};
	ManagerWait<Engine::Manager>{RESULT_ARG_PASS};

	RESULT_ENSURE_LAST();

	if (mRequestedRtvSize.x > 0 && mRequestedRtvSize.y > 0)
	{
		RESULT_ENSURE_CALL(mPlatformManager->Resize(mRequestedRtvSize, RESULT_ARG_PASS));
		mRequestedRtvSize = {};
	}

	RESULT_ENSURE_CALL(mPlatformManager->Update(RESULT_ARG_PASS));
	// RESULT_ENSURE_CALL(Thread::SleepCurrent(16u, RESULT_ARG_PASS));
	RESULT_OK();
}

void Manager::Finalize(RESULT_PARAM_IMPL)
{
	LOGC(Info, Render, "Finalizing...");
	RESULT_ENSURE_CALL(base_t::Finalize(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(base_t::WaitThreadFinish(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(mPlatformManager->Finalize(RESULT_ARG_PASS));
}

void Manager::ToggleFullscreen(Engine::Window& Window, RESULT_PARAM_IMPL)
{
	if(!IsInitialized())
	{
		return;
	}
	RESULT_CONDITION_ENSURE(Engine::Manager::Instance().IsInThread(), CurrentThreadIsNotTheRequiredOne);
	ManagerWait<Manager>{RESULT_ARG_PASS};
	RESULT_ENSURE_LAST();
	RESULT_ENSURE_CALL(Window.ToggleFullscreen(RESULT_ARG_PASS));
	mRequestedRtvSize = Window.GetSize();
	RESULT_OK();
}

void Manager::SetVsync(const bool Value) const
{
	mPlatformManager->mVsync = Value;
	LOGC(Info, Render, "Set vsync: %s", BOOL_TO_CSTR(Value));
}

void Manager::ToggleVsync() const
{
	SetVsync(!mPlatformManager->mVsync);
}

void Manager::ResizeFrame(glm::uvec2 NewSize, RESULT_PARAM_IMPL)
{
	if(!IsInitialized())
	{
		return;
	}
	RESULT_CONDITION_ENSURE(Engine::Manager::Instance().IsInThread(), CurrentThreadIsNotTheRequiredOne);
	ManagerWait<Manager>{RESULT_ARG_PASS};
	mRequestedRtvSize = NewSize;
	RESULT_OK();
}

#if PLATFORM_WINDOWS
void Manager::SetTextureData(ID3D12Resource* Handle, eastl::span<uint8_t> NewData, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(Handle, NullPtr);
	RESULT_OK();
}

ComPtr<ID3D12Resource> Manager::GetTextureCurrentBackBuffer(RESULT_PARAM_IMPL) const
{
	if(!IsInitialized())
	{
		return {};
	}
	RESULT_CONDITION_ENSURE(Engine::Manager::Instance().IsInThread(), CurrentThreadIsNotTheRequiredOne, {});
	RESULT_ENSURE_LAST({});
	RESULT_ENSURE_CALL(mPlatformManager->Flush(RESULT_ARG_PASS), {});
	RESULT_OK();
	return mPlatformManager->mBackBuffers[mPlatformManager->mSwapchain->GetCurrentBackBufferIndex()];
}
#endif

#if EDITOR
#if PLATFORM_WINDOWS
ID3D12Device* Manager::PlatformDevice() const
{
	return mPlatformManager->mDevice.Get();
}
uint32_t Manager::PlatformNumFrames()
{
	return PlatformManager::NUM_FRAMES;
}
ID3D12GraphicsCommandList* Manager::PlatformCmdList() const
{
	return mPlatformManager->mCommandList.Get();
}

#endif
#endif

void Manager::Flush() const
{
	mPlatformManager->Flush();
}

} // namespace Render
