
#include "Engine/Manager.h"
#include "Render/Manager.h"
#include "Editor/Manager.h"

MANAGER_NO_THREAD_IMPL(Engine::Manager);

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

	// Set main thread affinity
	RESULT_ENSURE_CALL(Thread::SetAffinity(Thread::HandleCurrent(), 0, RESULT_ARG_PASS));

	// Get program args
	RESULT_ENSURE_CALL(mArgs = ProgramArgs(Argc, Argv));

	// Create window
	RESULT_ENSURE_CALL(mWindow.Create({"Test window", 800, 600}, RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(mWindow.Show(RESULT_ARG_PASS));

	// Initialize managers
	RESULT_ENSURE_CALL(Render::Instance().Initialize(RESULT_ARG_PASS));
	RESULT_ENSURE_CALL(Render::Instance().ResizeFrame(mWindow.GetSize(), RESULT_ARG_PASS));

	// Initialize editor
#if EDITOR
	RESULT_ENSURE_CALL(Editor::Instance().Initialize(mWindow, RESULT_ARG_PASS));
#endif

	RESULT_OK();
}

int32_t Manager::Run(RESULT_PARAM_IMPL)
{
	LOGC(Info, Engine, "Running...");
	RESULT_ENSURE_LAST(EXIT_FAILURE);

	// Init clock
	mLoopBeginTime = mClock.now();

	// Run managers
	RESULT_ENSURE_CALL(Render::Manager::Instance().Run(RESULT_ARG_PASS), EXIT_FAILURE);

	// Run editor
#if EDITOR
	RESULT_ENSURE_CALL(Editor::Manager::Instance().Run(RESULT_ARG_PASS), EXIT_FAILURE);
#endif

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

	// Finalize editor
#if EDITOR
	RESULT_ENSURE_CALL(Editor::Manager::Instance().Finalize(RESULT_ARG_PASS));
#endif

	// Destroy window
	if (mWindow.IsVisible())
	{
		RESULT_ENSURE_CALL(mWindow.Destroy(RESULT_ARG_PASS));
	}

	// Call base finalize
	RESULT_ENSURE_CALL(base_t::Finalize(RESULT_ARG_PASS));

	RESULT_OK();
}

Window& Manager::GetWindow()
{
	return mWindow;
}

void Manager::DestroyWindow()
{
	mWindow.Destroy();
}

void Manager::RunInternal(RESULT_PARAM_IMPL)
{
	ManagerWait<Manager>{RESULT_ARG_PASS};

	RESULT_ENSURE_LAST();

	if (mWindow.IsVisible())
	{
		RESULT_ENSURE_CALL(mWindow.Update(RESULT_ARG_PASS));

		if (auto& lInput = Input::Manager::Instance(); lInput.IsVsyncKeyDown(true))
		{
			Render::Manager::Instance().ToggleVsync();
		}
		else if (lInput.IsFullscreenKeyDown(true))
		{
			RESULT_ENSURE_CALL(Render::Manager::Instance().ToggleFullscreen(mWindow, RESULT_ARG_PASS));
			if (RESULT_GET() != RESULT_OK_VALUE)
			{
				RESULT_OK();
				RESULT_ENSURE_CALL(mWindow.Destroy(RESULT_ARG_PASS));
			}
		}
		else if (lInput.IsKeyDown(Input::EKey::eEscape))
		{
			RESULT_ENSURE_CALL(mWindow.Destroy(RESULT_ARG_PASS));
		}
#if EDITOR
		else if (lInput.IsEditorKeyDown(true))
		{
			RESULT_ENSURE_CALL(Render::Instance().ToggleEditorActive());
		}
#endif
	}
	else
	{
		RESULT_ENSURE_CALL(Finalize(RESULT_ARG_PASS));
	}

	// Update frame info
	++mFrameCounter;
	mLoopEndTime = eastl::chrono::high_resolution_clock::now();
	mElapsedSeconds += static_cast<float32_t>((mLoopEndTime - mLoopBeginTime).count()) * 1e-9f;
	mDeltaTime	   = static_cast<float32_t>((mLoopEndTime - mLoopBeginTime).count()) * 1e-6f;
	mLoopBeginTime = mLoopEndTime;
	// mTotalSeconds += mDeltaSeconds;
	// mElapsedSeconds += mDeltaSeconds;

	// printf("Delta time: %f\n", mDeltaSeconds);

	/*if (mElapsedSeconds > 1.f)
	{
		char			lBuffer[500];
		const float64_t lFps = static_cast<float64_t>(mFrameCounter) / static_cast<float64_t>(mElapsedSeconds);
		snprintf(lBuffer, sizeof lBuffer, "FPS: %f\n", lFps);
		printf("%s\n", lBuffer);
		mElapsedSeconds = 0.f;
		mFrameCounter	= 0;
	}*/

	// RESULT_ENSURE_CALL(Thread::SleepCurrent(1u, RESULT_ARG_PASS));
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

NODISCARD float32_t Manager::GetDeltaTime() const
{
	return mDeltaTime;
}

} // namespace Engine
