
#include "Engine/Window.h"
#include "Engine/InputHandler.h"

namespace Engine
{

Window::Window()
{
}

Window::Window(const CreateInfo& CreateInfo, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_ENSURE_CALL_NOLOG(Create(CreateInfo, RESULT_ARG_PASS));
	RESULT_OK();
}

Window::~Window()
{
}

void Window::Create(const CreateInfo& CreateInfo, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();

#if PLATFORM_WINDOWS

	WNDCLASSA lWc{};
	lWc.lpfnWndProc	  = Input::Handler::WindowProc;
	lWc.hInstance	  = GetModuleHandle(nullptr);
	lWc.lpszClassName = "Window";
	RegisterClassA(&lWc);

	mHandle = CreateWindowA(lWc.lpszClassName, CreateInfo.Title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
							CreateInfo.Width, CreateInfo.Height, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

#endif

	RESULT_OK();
}

void Window::Show(RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(mHandle, eResultErrorNullPtr);
#if PLATFORM_WINDOWS
	ShowWindow(mHandle, 1);
#endif
	RESULT_OK();
}

void Window::Hide(RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(mHandle, eResultErrorNullPtr);
#if PLATFORM_WINDOWS
	ShowWindow(mHandle, 0);
#endif
	RESULT_OK();
}

void Window::Update(RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(mHandle, eResultErrorNullPtr);
#if PLATFORM_WINDOWS
	MSG lMsg = {};
	if (PeekMessageA(&lMsg, mHandle, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&lMsg);
		DispatchMessageA(&lMsg);
	}
#endif
	RESULT_OK();
}

void Window::Destroy(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();
	RESULT_CONDITION_ENSURE_NOLOG(mHandle, eResultErrorNullPtr);
#if PLATFORM_WINDOWS
	DestroyWindow(mHandle);
#endif
	mHandle = nullptr;
	RESULT_OK();
}

bool Window::IsVisible() const
{
#if PLATFORM_WINDOWS
	return IsWindowVisible(mHandle) == TRUE;
#else
	return false;
#endif
}

glm::ivec2 Window::GetSize() const
{
#if PLATFORM_WINDOWS
	RECT lArea{};
	GetClientRect(mHandle, &lArea);
	return {lArea.right, lArea.bottom};
#else
	return {};
#endif
}

window_handle_t Window::GetHandle() const
{
	return mHandle;
}

Window::EMessageBoxResult Window::MessageBoxInfo(const char* Text, const char* Caption, const uint32_t Types)
{
#if PLATFORM_WINDOWS
	return static_cast<EMessageBoxResult>(MessageBoxA(nullptr, Text, Caption, MB_OK | MB_ICONINFORMATION | Types));
#endif
}

Window::EMessageBoxResult Window::MessageBoxError(const char* Text, const char* Caption, const uint32_t Types)
{
#if PLATFORM_WINDOWS
	return static_cast<EMessageBoxResult>(MessageBoxA(nullptr, Text, Caption, MB_OK | MB_ICONERROR | Types));
#endif
}

Window::EMessageBoxResult Window::MessageBoxWarning(const char* Text, const char* Caption, const uint32_t Types)
{
#if PLATFORM_WINDOWS
	return static_cast<EMessageBoxResult>(MessageBoxA(nullptr, Text, Caption, MB_OK | MB_ICONWARNING | Types));
#endif
}

Window::EMessageBoxResult Window::MessageBoxQuestion(const char* Text, const char* Caption, const uint32_t Types)
{
#if PLATFORM_WINDOWS
	return static_cast<EMessageBoxResult>(MessageBoxA(nullptr, Text, Caption, MB_YESNO | MB_ICONQUESTION | Types));
#endif
}

} // namespace Engine
