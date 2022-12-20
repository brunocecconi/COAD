
#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include "Render/Common.h"
#include "Core/Manager.h"

#include <EASTL/unique_ptr.h>

#if PLATFORM_WINDOWS
#include <vulkan/vulkan.h>
#endif

LOG_DEFINE(Render);

namespace Engine
{
class Manager;
}

namespace Render
{

class PlatformManager;

/**
 * @brief Render manager class.
 *
 */
class Manager : public ManagerThread<Manager>
{
	CLASS_BODY_NON_MOVEABLE_COPYABLE(Manager);

public:
	using base_t = ManagerThread<Manager>;
	friend class base_t;

private:
	Manager();

public:
	~Manager();

private:
	void Initialize(RESULT_PARAM_DEFINE);
	void Run(RESULT_PARAM_DEFINE);
	void RunInternal(RESULT_PARAM_DEFINE);
	void Finalize(RESULT_PARAM_DEFINE);

private:
	friend class Engine::Manager;
	eastl::unique_ptr<PlatformManager> mPlatformManager;
};

} // namespace Render

CLASS_VALIDATION(Render::Manager);

#endif
