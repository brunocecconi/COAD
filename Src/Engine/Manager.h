
#ifndef ENGINE_MANAGER_H
#define ENGINE_MANAGER_H

#include "Engine/Common.h"
#include "Engine/ProgramArgs.h"
#include "Engine/Window.h"
#include "Core/Manager.h"

#include <EASTL/unique_ptr.h>
#include <EASTL/chrono.h>

LOG_DEFINE(Engine);

namespace Engine
{

struct test432432
{
	int a;
	int b;
};

/**
 * @brief Engine manager class.
 *
 */
class Manager: public ManagerNoThread<Manager>
{
	CLASS_BODY_NON_MOVEABLE_COPYABLE(Manager);

public:
	using base_t = ManagerNoThread<Manager>;
	friend class ManagerNoThread<Manager>;

private:
	Manager();

public:
	~Manager();

public:
	void				Initialize(int32_t Argc, char** Argv, RESULT_PARAM_DEFINE);
	MAYBEUNUSED int32_t Run(RESULT_PARAM_DEFINE);
	void				Finalize(RESULT_PARAM_DEFINE);

public:
	NODISCARD Window& GetWindow();
	void			  DestroyWindow();

private:
	void RunInternal(RESULT_PARAM_DEFINE);

public:
	static Manager& Instance();

private:
	ProgramArgs mArgs;
	Window		mWindow{};

public:
	NODISCARD float32_t GetDeltaTime() const;
	bool LoopPass{};

private:
	uint64_t										 mFrameCounter{};
	float32_t										 mDeltaTime{}, mTotalSeconds{}, mElapsedSeconds{};
	eastl::chrono::high_resolution_clock			 mClock{};
	eastl::chrono::high_resolution_clock::time_point mLoopBeginTime{}, mLoopEndTime{};
};

INLINE Manager& Instance()
{
	return Manager::Instance();
}

} // namespace Engine

CLASS_VALIDATION(Engine::Manager);

#endif
