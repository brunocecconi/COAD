
#include "Engine/Window.h"
#include "Engine/InputHandler.h"
#include "Render/Manager.h"
#include "Engine/Manager.h"

#if EDITOR
#if PLATFORM_WINDOWS
#include "Editor/Windows/imgui_impl_win32.h"
#endif
#endif

namespace Engine
{

#if PLATFORM_WINDOWS

LRESULT Window::WindowProc(const HWND Hwnd, const UINT Umsg, const WPARAM Wparam, const LPARAM Lparam)
{
	if(Umsg == WM_SIZE)
	{
		RECT lClientRect = {};
        ::GetClientRect(Hwnd, &lClientRect);

        int lWidth = lClientRect.right - lClientRect.left;
        int lHeight = lClientRect.bottom - lClientRect.top;

		Render::Manager::Instance().ResizeFrame({lWidth, lHeight});
	}
#if EDITOR
	ImGui_ImplWin32_WndProcHandler(Hwnd, Umsg, Wparam, Lparam);
#endif
	return Input::Handler::Instance().WindowProc(Hwnd, Umsg, Wparam, Lparam);
}
#endif

Window::Window()
{
}

Window::Window(const CreateInfo& CreateInfo, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_ENSURE_CALL_NOLOG(Create(CreateInfo, RESULT_ARG_PASS));
	RESULT_OK();
}

Window::~Window()
{
}

void Window::Create(const CreateInfo& CreateInfo, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

#if PLATFORM_WINDOWS

	WNDCLASSA lWc{};
	lWc.style		  = CS_HREDRAW | CS_VREDRAW;
	lWc.hIcon		  = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
	lWc.hCursor		  = (HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
	lWc.lpfnWndProc	  = Window::WindowProc;
	lWc.hInstance	  = GetModuleHandle(nullptr);
	lWc.lpszClassName = "Window";
	lWc.hbrBackground = CreateSolidBrush(RGB(31, 31, 127));
	RegisterClassA(&lWc);

	const int32_t lScreenWidth	= ::GetSystemMetrics(SM_CXSCREEN);
	const int32_t lScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	constexpr UINT lStyle = WS_OVERLAPPEDWINDOW;

	mState.Rect = {0, 0, static_cast<LONG>(CreateInfo.Width), static_cast<LONG>(CreateInfo.Height)};
	::AdjustWindowRect(&mState.Rect, lStyle, FALSE);

	const int32_t lWindowWidth	= mState.Rect.right - mState.Rect.left;
	const int32_t lWindowHeight = mState.Rect.bottom - mState.Rect.top;
	const int32_t lWindowX		= eastl::max(0, (lScreenWidth - lWindowWidth) / 2);
	const int32_t lWindowY		= eastl::max(0, (lScreenHeight - lWindowHeight) / 2);

	mHandle = CreateWindowA(lWc.lpszClassName, CreateInfo.Title, lStyle, lWindowX, lWindowY, lWindowWidth,
							lWindowHeight, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

	mState.Style = GetWindowLong(mHandle, GWL_STYLE);

#endif

	RESULT_OK();
}

void Window::Show(RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE_NOLOG(mHandle, NullPtr);
#if PLATFORM_WINDOWS
	ShowWindow(mHandle, SW_NORMAL);
#endif
	RESULT_OK();
}

void Window::Hide(RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE_NOLOG(mHandle, NullPtr);
#if PLATFORM_WINDOWS
	ShowWindow(mHandle, SW_HIDE);
#endif
	RESULT_OK();
}

void Window::SetFullscreen(const bool Value, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE_NOLOG(mHandle, NullPtr);

	LOGC(Info, Window, "Set fullscreen: %s", BOOL_TO_CSTR(Value));

	if (mState.Fullscreen != Value)
	{
		mState.Fullscreen = Value;

#if PLATFORM_WINDOWS
		if (mState.Fullscreen)
		{
			// Save previous state
			::GetWindowRect(mHandle, &mState.PreviousRect);
			mState.PreviousStyle = GetWindowLongA(mHandle, GWL_STYLE);

			// Set style
			SetWindowLongA(mHandle, GWL_STYLE, 0);

			// Get monitor info
			const HMONITOR lMonitor = ::MonitorFromWindow(mHandle, MONITOR_DEFAULTTONEAREST);
			MONITORINFO	   lMi		= {sizeof lMi};
			GetMonitorInfoA(lMonitor, &lMi);

			// Save state
			mState.Rect = lMi.rcMonitor;
			mState.Style = 0;

			// Set window pos
			SetWindowPos(mHandle, HWND_TOP, mState.Rect.left, mState.Rect.top,
						 mState.Rect.right - mState.Rect.left, mState.Rect.bottom - mState.Rect.top,
						 SWP_FRAMECHANGED | SWP_NOACTIVATE);

			// Apply show window
			ShowWindow(mHandle, SW_MAXIMIZE);
		}
		else
		{
			// Restore state
			const RECT lPreviousRect  = mState.Rect;
			const UINT lPreviousStyle = mState.Style;
			mState.Style              = mState.PreviousStyle;
			mState.Rect               = mState.PreviousRect;
			SetWindowLongA(mHandle, GWL_STYLE, mState.Style);

			// Save state
			mState.PreviousRect = lPreviousRect;
			mState.PreviousStyle = lPreviousStyle;

			// Set window pos
			SetWindowPos(mHandle, HWND_NOTOPMOST, mState.Rect.left, mState.Rect.top,
						 mState.Rect.right - mState.Rect.left,
						 mState.Rect.bottom - mState.Rect.top, SWP_FRAMECHANGED | SWP_NOACTIVATE);

			// Apply show window
			ShowWindow(mHandle, SW_NORMAL);
		}
#endif
	}
	RESULT_OK();
}

void Window::ToggleFullscreen(RESULT_PARAM_IMPL)
{
	SetFullscreen(!mState.Fullscreen);
}

void Window::Update(RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE_NOLOG(mHandle, NullPtr);
#if PLATFORM_WINDOWS
	MSG lMsg = {};
	while (PeekMessageA(&lMsg, mHandle, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&lMsg);
		DispatchMessageA(&lMsg);
	}
#endif
	RESULT_OK();
}

void Window::Destroy(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE_NOLOG(mHandle, NullPtr);
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

INLINE bool Window::IsFullScreen() const
{
	return mState.Fullscreen;
}

glm::uvec2 Window::GetSize() const
{
#if PLATFORM_WINDOWS
	RECT lArea{};
	GetClientRect(mHandle, &lArea);
	return {lArea.right, lArea.bottom};
#else
	return {};
#endif
}

glm::uvec2 Window::GetPreviousSize() const
{
	return {mState.PreviousRect.right - mState.PreviousRect.left, mState.PreviousRect.bottom - mState.PreviousRect.top};
}

Window::Rect Window::GetRect() const
{
#if PLATFORM_WINDOWS
	RECT lArea{};
	GetClientRect(mHandle, &lArea);
	return {lArea.left, lArea.top, lArea.right, lArea.bottom};
#else
	return {};
#endif
}

Window::Rect Window::GetPreviousRect() const
{
	return {mState.PreviousRect.left, mState.PreviousRect.top, mState.PreviousRect.right, mState.PreviousRect.bottom};
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
