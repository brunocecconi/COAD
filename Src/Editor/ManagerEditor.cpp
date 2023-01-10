
#include "Editor/Manager.h"
#include "Engine/Window.h"
#include "Render/Manager.h"
#include "Engine/Manager.h"

#if EDITOR

#if PLATFORM_WINDOWS
#include "Editor/Windows/imgui_impl_win32.h"
#include "Editor/Windows/imgui_impl_dx12.h"
#include <d3d12.h>
#include <wrl.h>
#endif

MANAGER_IMPL(Editor::Manager);

namespace Editor
{

Manager::Manager()
{
}

Manager::~Manager()
{
}

void Manager::RunExternal(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	// Menu
	{
		if(ImGui::BeginMainMenuBar())
		{
			if(ImGui::BeginMenu("File"))
			{
				if(ImGui::MenuItem("Quit", "Esc", nullptr))
				{
					Engine::Manager::Instance().DestroyWindow();
				}
				ImGui::EndMenu();
			}
		}
		ImGui::EndMainMenuBar();
	}

	RESULT_OK();
}

void Manager::Initialize(const Engine::Window& Window, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_ENSURE_CALL(base_t::Initialize(RESULT_ARG_PASS));

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

#if PLATFORM_WINDOWS

	RESULT_CONDITION_ENSURE(ImGui_ImplWin32_Init(Window.GetHandle()), EditorFailedToInitImguiWindow);

	D3D12_DESCRIPTOR_HEAP_DESC lDesc = {};
	lDesc.Type						 = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	lDesc.NumDescriptors			 = 1;
	lDesc.Flags						 = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	Render::Manager::Instance().PlatformDevice()->CreateDescriptorHeap(&lDesc, IID_PPV_ARGS(&mSrvDescHeap));
	mCpuSrvDescHeap = mSrvDescHeap->GetCPUDescriptorHandleForHeapStart();
	mGpuSrvDescHeap = mSrvDescHeap->GetGPUDescriptorHandleForHeapStart();

	ImGui_ImplDX12_Init(Render::Manager::Instance().PlatformDevice(), Render::Manager::PlatformNumFrames(),
						DXGI_FORMAT_R8G8B8A8_UNORM, mSrvDescHeap, mCpuSrvDescHeap, mGpuSrvDescHeap);

#endif

	RESULT_OK();
}

void Manager::RunInternal(RESULT_PARAM_IMPL)
{
	ManagerWait<Manager>{RESULT_ARG_PASS};
	RESULT_ENSURE_LAST();
	RESULT_OK();
}

void Manager::Finalize(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	RESULT_ENSURE_CALL(base_t::Finalize(RESULT_ARG_PASS));
	RESULT_OK();
}

} // namespace Editor

#endif
