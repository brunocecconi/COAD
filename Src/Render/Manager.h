
#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

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
namespace Editor
{
class Manager;
}
#if PLATFORM_WINDOWS
namespace Microsoft::WRL
{
template<typename T>
class ComPtr;
}
struct ID3D12Device;
struct ID3D12GraphicsCommandList;
struct ID3D12Resource;
#endif
#endif

namespace Render
{

class PlatformManager;

/**
 * @brief Render manager class.
 *
 */
class Manager: public ManagerThread<Manager>
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

public:
	void ToggleFullscreen(Engine::Window& Window, RESULT_PARAM_DEFINE);
	void SetVsync(bool Value) const;
	void ToggleVsync() const;
	void ResizeFrame(glm::uvec2 NewSize, RESULT_PARAM_DEFINE);

public:
#if PLATFORM_WINDOWS
	void SetTextureData(ID3D12Resource* Handle, eastl::span<uint8_t> NewData, RESULT_PARAM_DEFINE);
	NODISCARD ComPtr<ID3D12Resource> GetTextureCurrentBackBuffer(RESULT_PARAM_DEFINE) const;
#endif

private:
#if EDITOR
	friend class Editor::Manager;

#if PLATFORM_WINDOWS
	NODISCARD ID3D12Device*				   PlatformDevice() const;
	NODISCARD static uint32_t			   PlatformNumFrames();
	NODISCARD ID3D12GraphicsCommandList*   PlatformCmdList() const;
#endif

#endif

	void Flush() const;

private:
	friend class Engine::Manager;

	eastl::unique_ptr<PlatformManager> mPlatformManager;
	glm::uvec2						   mRequestedRtvSize{};
};

INLINE Manager& Instance()
{
	return Manager::Instance();
}

} // namespace Render

CLASS_VALIDATION(Render::Manager);

#endif
