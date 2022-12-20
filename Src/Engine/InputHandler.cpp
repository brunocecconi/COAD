
#include "Engine/InputHandler.h"

namespace Engine
{

namespace Input
{

eastl::unique_ptr<Handler> Handler::instance_;

#if PLATFORM_WINDOWS
static constexpr uint32_t WINDOW_KEY_MAP[] =
{
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
	VK_LBUTTON,
	VK_RBUTTON,
	VK_CANCEL,
	VK_MBUTTON,

	VK_BACK,
	VK_TAB,
	VK_CLEAR,
	VK_RETURN,
	VK_SHIFT,
	VK_CONTROL,
	VK_MENU,
	VK_PAUSE,
	VK_CAPITAL,

	VK_ESCAPE,
	VK_PRIOR,
	VK_NEXT,
	VK_END,
	VK_HOME,
	VK_LEFT,
	VK_UP,
	VK_RIGHT,
	VK_DOWN,
	VK_SELECT,
	VK_PRINT,
	VK_EXECUTE,
	VK_SNAPSHOT,
	VK_INSERT,
	VK_DELETE,
	VK_HELP,

	VK_SLEEP,

	VK_NUMPAD0,
	VK_NUMPAD0,
	VK_NUMPAD2,
	VK_NUMPAD3,
	VK_NUMPAD4,
	VK_NUMPAD5,
	VK_NUMPAD6,
	VK_NUMPAD7,
	VK_NUMPAD8,
	VK_NUMPAD9,
	VK_MULTIPLY,
	VK_ADD,
	VK_SEPARATOR,
	VK_SUBTRACT,
	VK_DECIMAL,
	VK_DIVIDE,
	VK_F1,
	VK_F2,
	VK_F3,
	VK_F4,
	VK_F5,
	VK_F6,
	VK_F7,
	VK_F8,
	VK_F9,
	VK_F10,
	VK_F11,
	VK_F12,
	VK_F13,
	VK_F14,
	VK_F15,
	VK_F16,
	VK_F17,
	VK_F18,
	VK_F19,
	VK_F20,
	VK_F21,
	VK_F22,
	VK_F23,
	VK_F24,

	VK_NUMLOCK,
	VK_SCROLL,
	VK_LSHIFT,
	VK_RSHIFT,
	VK_LCONTROL,
	VK_RCONTROL,
	VK_LMENU,
	VK_RMENU

};
#endif

Handler::Handler()
{
}

Handler::~Handler()
{
}

void Handler::Update(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST_NOLOG();

	RESULT_OK();
}

bool Handler::IsKeyDown(EKey key) const
{
	return keydown_states_.keys_bitset.test(WINDOW_KEY_MAP[static_cast<uint32_t>(key)]);
}

bool Handler::IsKeyUp(EKey key) const
{
	return keyup_states_.keys_bitset.test(WINDOW_KEY_MAP[static_cast<uint32_t>(key)]);
}

Handler& Handler::Instance()
{
	if (!instance_)
	{
		instance_.reset(new (Allocators::Default{DEBUG_NAME_VAL("Engine::Input")}.allocate(sizeof(Handler))) Handler{});
	}
	return *instance_;
}

#if PLATFORM_WINDOWS
LRESULT Handler::WindowProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	if(umsg == WM_KEYDOWN)
	{
		if(!Instance().keydown_states_.keys_bitset.test(wparam))
		{
			Instance().keyup_states_.keys_bitset.set(wparam, false);
			Instance().keydown_states_.keys_bitset.set(wparam, true);
			//printf("keydown: wparam: %llu, lparam: %lli\n", wparam, lparam);
		}
	}
	else if(umsg == WM_KEYUP)
	{
		if(!Instance().keyup_states_.keys_bitset.test(wparam))
		{
			Instance().keydown_states_.keys_bitset.set(wparam, false);
			Instance().keyup_states_.keys_bitset.set(wparam, true);
			//printf("keyup: wparam: %llu, lparam: %lli\n", wparam, lparam);
		}
	}
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}
#endif

}

}