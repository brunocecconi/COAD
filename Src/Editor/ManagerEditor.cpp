
#include "Editor/Manager.h"

#if EDITOR

#include "Engine/Window.h"
#include "Render/Manager.h"
#include "Engine/Manager.h"

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
	/*{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Quit", "Esc", nullptr))
				{
					Engine::Manager::Instance().DestroyWindow();
				}
				ImGui::EndMenu();
			}
		}
		ImGui::EndMainMenuBar();
	}*/

	RESULT_OK();
}

void Manager::Initialize(const Engine::Window& Window, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_ENSURE_CALL(base_t::Initialize(RESULT_ARG_PASS));

	/*IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

#if PLATFORM_WINDOWS
	RESULT_CONDITION_ENSURE(ImGui_ImplWin32_Init(Window.GetHandle()), EditorFailedToInitImguiWindow);

#if OPENGL_ENABLED
	RESULT_CONDITION_ENSURE(ImGui_ImplOpenGL3_Init(), EditorFailedToInitImguiWindow);
#endif

#endif*/

	RESULT_OK();
}

void Manager::PreRunLoop(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
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

//#if PLATFORM_WINDOWS
//	ImGui_ImplWin32_Shutdown();
//#if OPENGL_ENABLED
//	ImGui_ImplOpenGL3_Shutdown();
//#endif
//#endif
//	ImGui::DestroyContext();

	RESULT_ENSURE_CALL(base_t::Finalize(RESULT_ARG_PASS));
	RESULT_OK();
}

} // namespace Editor

#endif
