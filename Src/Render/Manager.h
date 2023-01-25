
#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include <EASTL/chrono.h>

#include "Render/Common.h"
#include "Core/Manager.h"

#include <EASTL/unique_ptr.h>
#include <EASTL/span.h>

LOG_DEFINE(Render);

namespace Engine
{
class Manager;
class Window;
} // namespace Engine

#if EDITOR

#if OPENGL_ENABLED
#endif

namespace Editor
{
class Manager;
}
#endif

namespace Render
{

namespace Api
{
class Manager;
} // namespace Api

enum class EApi
{
	eNone,
	eOpengl,
	eVulkan,
	eD3D11,
	eD3D12
};

/**
 * @brief Render manager class.
 *
 */
class Manager: public ManagerThread<Manager>
{
	CLASS_BODY_NON_MOVEABLE_COPYABLE(Manager);

public:
	using base_t = ManagerThread<Manager>;
	friend class ManagerThread<Manager>;

private:
	Manager();

public:
	~Manager();

private:
	void Initialize(RESULT_PARAM_DEFINE);
	void Run(RESULT_PARAM_DEFINE);
	void PreRunLoop(RESULT_PARAM_DEFINE);
	void RunInternal(RESULT_PARAM_DEFINE);
	void Finalize(RESULT_PARAM_DEFINE);

public:
	void ToggleFullscreen(Engine::Window& Window, RESULT_PARAM_DEFINE);
	void SetVsync(bool Value) const;
	void ToggleVsync() const;
	void ResizeFrame(glm::uvec2 NewSize, RESULT_PARAM_DEFINE);
	void MarkDirtyFramebufferSize(RESULT_PARAM_DEFINE);

#if EDITOR
	void SetEditorActive(bool Value, RESULT_PARAM_DEFINE);
	void ToggleEditorActive(RESULT_PARAM_DEFINE);
#endif

	NODISCARD static EApi GetTargetApi();

public:
private:
#if EDITOR
	friend class Editor::Manager;
#endif

private:
	friend class Engine::Manager;
	friend class Api::Manager;

	eastl::unique_ptr<Api::Manager> mTargetApiManager;
	bool							mRequestedDirtyFramebufferSize{};
};

INLINE Manager& Instance()
{
	return Manager::Instance();
}

} // namespace Render

CLASS_VALIDATION(Render::Manager);

#endif
