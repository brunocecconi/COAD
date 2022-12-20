
#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#include "Engine/Common.h"

#include <glm/vec2.hpp>
#include <EASTL/unique_ptr.h>

namespace Engine
{

#if PLATFORM_WINDOWS
using window_handle_t = HWND;
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

private:
	friend class Manager;

	Window();
	EXPLICIT Window(const CreateInfo& CreateInfo, RESULT_PARAM_DEFINE);
	~Window();

	void Create(const CreateInfo& CreateInfo, RESULT_PARAM_DEFINE);
	void Update(RESULT_PARAM_DEFINE) const;
	void Destroy(RESULT_PARAM_DEFINE);

public:
	void Show(RESULT_PARAM_DEFINE) const;
	void Hide(RESULT_PARAM_DEFINE) const;

	NODISCARD bool IsVisible() const;
	NODISCARD glm::ivec2	  GetSize() const;
	NODISCARD window_handle_t GetHandle() const;

public:
	static EMessageBoxResult MessageBoxInfo(const char* Text, const char* Caption, uint32_t Types = eMbtNone);
	static EMessageBoxResult MessageBoxError(const char* Text, const char* Caption, uint32_t Types = eMbtNone);
	static EMessageBoxResult MessageBoxWarning(const char* Text, const char* Caption, uint32_t Types = eMbtNone);
	static EMessageBoxResult MessageBoxQuestion(const char* Text, const char* Caption, uint32_t Types = eMbtNone);

private:
	window_handle_t mHandle{};
};

} // namespace Engine

CLASS_VALIDATION(Engine::Window);

#endif
