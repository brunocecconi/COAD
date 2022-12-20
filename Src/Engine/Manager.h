
#ifndef ENGINE_MANAGER_H
#define ENGINE_MANAGER_H

#include "Engine/Common.h"
#include "Engine/ProgramArgs.h"
#include "Engine/Window.h"
#include "Core/Manager.h"

#include <EASTL/unique_ptr.h>

LOG_DEFINE(Engine);

namespace Render
{
class PlatformManager;
}

namespace Engine
{

/**
 * @brief Engine manager class.
 *
 */
class Manager: public ManagerNoThread<Manager>
{
	CLASS_BODY_NON_MOVEABLE_COPYABLE(Manager);

public:
	using base_t = ManagerNoThread<Manager>;
	friend class base_t;

private:
	Manager();

public:
	~Manager();

public:
	void			  Initialize(int32_t Argc, char** Argv, RESULT_PARAM_DEFINE);
	MAYBEUNUSED int32_t Run(RESULT_PARAM_IMPL);
	void			  Finalize(RESULT_PARAM_IMPL);

private:
	void RunInternal(RESULT_PARAM_IMPL);

public:
	static Manager& Instance();

private:
	friend class Render::PlatformManager;
	ProgramArgs mArgs;
	Window		mWindow{};
};

} // namespace Engine

CLASS_VALIDATION(Engine::Manager);

#endif
