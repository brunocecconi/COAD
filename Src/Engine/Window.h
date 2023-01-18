
#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#include "Engine/Common.h"

#include <glm/vec2.hpp>
#include <EASTL/unique_ptr.h>

LOG_DEFINE(Window);

namespace Engine
{

#if PLATFORM_WINDOWS
using window_handle_t = HWND;
using window_info_t	  = WNDCLASSA;
#endif

/**
 * @brief Window singleton class.
 *
 */
class Window
{
	CLASS_BODY_NON_MOVEABLE_COPYABLE(Window);

public:
#if PLATFORM_WINDOWS
	enum EMessageBoxResult : uint32_t
	{
		eMbrNone	  = 0,
		eMbrOk		  = IDOK,
		eMbrCancel	  = IDCANCEL,
		eMbrAbort	  = IDABORT,
		eMbrRetry	  = IDRETRY,
		eMbrIgnore	  = IDIGNORE,
		eMbrYes		  = IDYES,
		eMbrNo		  = IDNO,
		eMbrClose	  = IDCLOSE,
		eMbrHelp	  = IDHELP,
		eMbrTryAgain  = IDTRYAGAIN,
		eMbrRtontinue = IDCONTINUE
	};
	enum EMessageBoxTypes : uint32_t
	{
		eMbtNone			   = 0,
		eMbtAbortRetryIgnore   = MB_ABORTRETRYIGNORE,
		eMbtCancelTryContinue  = MB_CANCELTRYCONTINUE,
		eMbtHelp			   = MB_HELP,
		eMbtOk				   = MB_OK,
		eMbtOkCancel		   = MB_OKCANCEL,
		eMbtRetryCancel		   = MB_RETRYCANCEL,
		eMbtYesNo			   = MB_YESNO,
		eMbtYesNoCancel		   = MB_YESNOCANCEL,
		eMbtApplModal		   = MB_APPLMODAL,
		eMbtSystemModal		   = MB_SYSTEMMODAL,
		eMbtTaskModal		   = MB_TASKMODAL,
		eMbtTextRightJustified = MB_RIGHT,
		eMbtTopMost			   = MB_TOPMOST
	};
#endif

public:
	struct CreateInfo
	{
		const char* Title;
		int32_t		Width, Height;
	};

	struct Rect
	{
		int32_t Left, Top, Right, Bottom;
	};

	struct State
	{
#if PLATFORM_WINDOWS
		UINT Style, PreviousStyle;
		RECT Rect, PreviousRect;
#endif
		bool Fullscreen;
	};

private:
	friend class Manager;

	Window();
	EXPLICIT Window(const CreateInfo& CreateInfo, RESULT_PARAM_DEFINE);
	~Window();

	void Create(const CreateInfo& CreateInfo, RESULT_PARAM_DEFINE);
public:
	void Update(RESULT_PARAM_DEFINE) const;
	void Destroy(RESULT_PARAM_DEFINE);

public:
	void Show(RESULT_PARAM_DEFINE) const;
	void Hide(RESULT_PARAM_DEFINE) const;
	void SetFullscreen(bool Value, RESULT_PARAM_DEFINE);
	void ToggleFullscreen(RESULT_PARAM_DEFINE);

	NODISCARD bool IsVisible() const;
	NODISCARD bool IsFullScreen() const;
	NODISCARD glm::uvec2 GetSize() const;
	NODISCARD glm::uvec2	  GetPreviousSize() const;
	NODISCARD Rect			  GetRect() const;
	NODISCARD Rect			  GetPreviousRect() const;
	NODISCARD window_handle_t GetHandle() const;
	NODISCARD bool IsMinimized() const;

private:
#if PLATFORM_WINDOWS
	static LRESULT WindowProc(HWND Hwnd, UINT Umsg, WPARAM Wparam, LPARAM Lparam);
#endif

public:
	static EMessageBoxResult MessageBoxInfo(const char* Text, const char* Caption, uint32_t Types = eMbtNone);
	static EMessageBoxResult MessageBoxError(const char* Text, const char* Caption, uint32_t Types = eMbtNone);
	static EMessageBoxResult MessageBoxWarning(const char* Text, const char* Caption, uint32_t Types = eMbtNone);
	static EMessageBoxResult MessageBoxQuestion(const char* Text, const char* Caption, uint32_t Types = eMbtNone);

private:
	window_handle_t mHandle{};
	State			mState{};
};

} // namespace Engine

CLASS_VALIDATION(Engine::Window);

#endif
