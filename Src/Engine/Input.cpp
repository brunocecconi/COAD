
#include "Engine/Input.h"

#ifndef PRINT_INPUT
#define PRINT_INPUT 1
#endif

#ifdef RELEASE
#undef PRINT_INPUT
#define PRINT_INPUT 0
#endif

META_ENUM_AUTO_REGISTER_CUSTOM(Engine::Input::EKey, EngineInputEKey);

namespace Engine
{

namespace Input
{

eastl::unique_ptr<Manager> Manager::mInstance;

#if PLATFORM_WINDOWS
static constexpr uint32_t WINDOW_KEY_MAP[] = {
	'A',		'B',		 'C',		 'D',		   'E',			'F',		 'G',		 'H',		 'I',
	'J',		'K',		 'L',		 'M',		   'N',			'O',		 'P',		 'Q',		 'R',
	'S',		'T',		 'U',		 'V',		   'W',			'X',		 'Y',		 'Z',		 VK_LBUTTON,
	VK_RBUTTON, VK_CANCEL,	 VK_MBUTTON,

	VK_BACK,	VK_TAB,		 VK_CLEAR,	 VK_RETURN,	   VK_SHIFT,	VK_CONTROL,	 VK_MENU,	 VK_PAUSE,	 VK_CAPITAL,

	VK_ESCAPE,	VK_PRIOR,	 VK_NEXT,	 VK_END,	   VK_HOME,		VK_LEFT,	 VK_UP,		 VK_RIGHT,	 VK_DOWN,
	VK_SELECT,	VK_PRINT,	 VK_EXECUTE, VK_SNAPSHOT,  VK_INSERT,	VK_DELETE,	 VK_HELP,

	VK_SLEEP,

	VK_NUMPAD0, VK_NUMPAD0,	 VK_NUMPAD2, VK_NUMPAD3,   VK_NUMPAD4,	VK_NUMPAD5,	 VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8,
	VK_NUMPAD9, VK_MULTIPLY, VK_ADD,	 VK_SEPARATOR, VK_SUBTRACT, VK_DECIMAL,	 VK_DIVIDE,	 VK_F1,		 VK_F2,
	VK_F3,		VK_F4,		 VK_F5,		 VK_F6,		   VK_F7,		VK_F8,		 VK_F9,		 VK_F10,	 VK_F11,
	VK_F12,		VK_F13,		 VK_F14,	 VK_F15,	   VK_F16,		VK_F17,		 VK_F18,	 VK_F19,	 VK_F20,
	VK_F21,		VK_F22,		 VK_F23,	 VK_F24,

	VK_NUMLOCK, VK_SCROLL,	 VK_LSHIFT,	 VK_RSHIFT,	   VK_LCONTROL, VK_RCONTROL, VK_LMENU,	 VK_RMENU

};
#endif

Manager::Manager()
{
}

Manager::~Manager()
{
}

void Manager::Update(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();

	RESULT_OK();
}

bool Manager::IsVsyncKeyDown(const bool AutoReset)
{
	return IsKeyDown(EKey::eV, 0, AutoReset);
}

bool Manager::IsFullscreenKeyDown(const bool AutoReset)
{
	return IsKeyDown(EKey::eEnter, EKeyModifiers::eLeftAlt, AutoReset) || IsKeyDown(EKey::eF11, 0, AutoReset);
}

bool Manager::IsKeyDown(EKey Key, const EKeyModifiers::primitive_t Modifiers, const bool AutoReset)
{
	const auto lKeyMapValue = WINDOW_KEY_MAP[static_cast<uint32_t>(Key)];
	const bool lValue =
		((mKeydownStates.KeysModifiers & Modifiers) == Modifiers) && mKeydownStates.KeysBitset.test(lKeyMapValue);
	if (AutoReset)
	{
		mKeydownStates.KeysBitset.set(lKeyMapValue, false);
	}
	return lValue;
}

bool Manager::IsKeyUp(EKey Key, const EKeyModifiers::primitive_t Modifiers, const bool AutoReset)
{
	const auto lKeyMapValue = WINDOW_KEY_MAP[static_cast<uint32_t>(Key)];
	const bool lValue =
		((mKeyupStates.KeysModifiers & Modifiers) == Modifiers) && mKeyupStates.KeysBitset.test(lKeyMapValue);
	if (AutoReset)
	{
		mKeyupStates.KeysBitset.set(lKeyMapValue, false);
	}
	return lValue;
}

void Manager::ResetKeyUp(EKey Key)
{
	mKeyupStates.KeysBitset.set(WINDOW_KEY_MAP[static_cast<uint32_t>(Key)], false);
}

#if EDITOR
bool Manager::IsEditorKeyDown(const bool AutoReset)
{
	return IsKeyDown(EKey::eE, EKeyModifiers::eLeftControl, true);
}

void Manager::ResetEditorKeyDown()
{
	ResetKeyDown(EKey::eLeftControl);
	ResetKeyDown(EKey::eE);
}
#endif

void Manager::ResetKeyDown(EKey Key)
{
	mKeydownStates.KeysBitset.set(WINDOW_KEY_MAP[static_cast<uint32_t>(Key)], false);
}

void Manager::ResetFullscreenKeyDown()
{
	ResetKeyDown(EKey::eF11);
	ResetKeyDown(EKey::eLeftAlt);
	ResetKeyDown(EKey::eEnter);
}

void Manager::ResetVsyncKeyDown()
{
	ResetKeyDown(EKey::eV);
}

Manager& Manager::Instance()
{
	if (!mInstance)
	{
		mInstance.reset(new (Allocators::Default{DEBUG_NAME_VAL("Engine::Input")}.allocate(sizeof(Manager))) Manager{});
	}
	return *mInstance;
}

#if PLATFORM_WINDOWS
LRESULT Manager::WindowProc(const HWND Hwnd, const UINT Umsg, const WPARAM Wparam, const LPARAM Lparam)
{
	if (Umsg == WM_KEYDOWN || Umsg == WM_SYSKEYDOWN)
	{
		if (GetAsyncKeyState(VK_LSHIFT) < 0)
		{
			Instance().mKeyupStates.KeysModifiers &= ~EKeyModifiers::eLeftShift;
			Instance().mKeydownStates.KeysModifiers |= EKeyModifiers::eLeftShift;
		}
		if (GetAsyncKeyState(VK_RSHIFT) < 0)
		{
			Instance().mKeyupStates.KeysModifiers &= ~EKeyModifiers::eRightShift;
			Instance().mKeydownStates.KeysModifiers |= EKeyModifiers::eRightShift;
		}
		if (GetAsyncKeyState(VK_LCONTROL) < 0)
		{
			Instance().mKeyupStates.KeysModifiers &= ~EKeyModifiers::eLeftControl;
			Instance().mKeydownStates.KeysModifiers |= EKeyModifiers::eLeftControl;
		}
		if (GetAsyncKeyState(VK_RCONTROL) < 0)
		{
			Instance().mKeyupStates.KeysModifiers &= ~EKeyModifiers::eRightControl;
			Instance().mKeydownStates.KeysModifiers |= EKeyModifiers::eRightControl;
		}
		if (GetAsyncKeyState(VK_LMENU) < 0)
		{
			Instance().mKeyupStates.KeysModifiers &= ~EKeyModifiers::eLeftAlt;
			Instance().mKeydownStates.KeysModifiers |= EKeyModifiers::eLeftAlt;
		}
		if (GetAsyncKeyState(VK_RMENU) < 0)
		{
			Instance().mKeyupStates.KeysModifiers &= ~EKeyModifiers::eRightAlt;
			Instance().mKeydownStates.KeysModifiers |= EKeyModifiers::eRightAlt;
		}
		if (!Instance().mKeydownStates.KeysBitset.test(Wparam))
		{
			Instance().mKeyupStates.KeysBitset.set(Wparam, false);
			Instance().mKeydownStates.KeysBitset.set(Wparam, true);

#if PRINT_INPUT
			LOG(Info, "Input: keydown: %llu", Wparam);
#endif
		}
	}
	else if (Umsg == WM_KEYUP || Umsg == WM_SYSKEYUP)
	{
		if (GetAsyncKeyState(VK_LSHIFT) >= 0)
		{
			Instance().mKeydownStates.KeysModifiers &= ~EKeyModifiers::eLeftShift;
			Instance().mKeyupStates.KeysModifiers |= EKeyModifiers::eLeftShift;
		}
		if (GetAsyncKeyState(VK_RSHIFT) >= 0)
		{
			Instance().mKeydownStates.KeysModifiers &= ~EKeyModifiers::eRightShift;
			Instance().mKeyupStates.KeysModifiers |= EKeyModifiers::eRightShift;
		}
		if (GetAsyncKeyState(VK_LCONTROL) >= 0)
		{
			Instance().mKeydownStates.KeysModifiers &= ~EKeyModifiers::eLeftControl;
			Instance().mKeyupStates.KeysModifiers |= EKeyModifiers::eLeftControl;
		}
		if (GetAsyncKeyState(VK_RCONTROL) >= 0)
		{
			Instance().mKeydownStates.KeysModifiers &= ~EKeyModifiers::eRightControl;
			Instance().mKeyupStates.KeysModifiers |= EKeyModifiers::eRightControl;
		}
		if (GetAsyncKeyState(VK_LMENU) >= 0)
		{
			Instance().mKeydownStates.KeysModifiers &= ~EKeyModifiers::eLeftAlt;
			Instance().mKeyupStates.KeysModifiers |= EKeyModifiers::eLeftAlt;
		}
		if (GetAsyncKeyState(VK_RMENU) >= 0)
		{
			Instance().mKeydownStates.KeysModifiers &= ~EKeyModifiers::eRightAlt;
			Instance().mKeyupStates.KeysModifiers |= EKeyModifiers::eRightAlt;
		}
		if (!Instance().mKeyupStates.KeysBitset.test(Wparam))
		{
			Instance().mKeyupStates.KeysBitset.set(Wparam, true);
			Instance().mKeydownStates.KeysBitset.set(Wparam, false);
#if PRINT_INPUT
			LOG(Info, "Input: keyup: %llu", Wparam);
#endif
		}
	}
	return DefWindowProc(Hwnd, Umsg, Wparam, Lparam);
}
#endif

} // namespace Input

} // namespace Engine