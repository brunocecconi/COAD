
#ifndef ENGINE_INPUT_HANDLER_H
#define ENGINE_INPUT_HANDLER_H

#include "Engine/Common.h"

#include <EASTL/bitset.h>
#include <EASTL/unique_ptr.h>

#if PLATFORM_WINDOWS
#include <xinput.h>
#endif

namespace Engine
{

class Window;

namespace Input
{

enum class EKey : uint32_t
{
	eA,eB,eC,eD,eE,eF,eG,eH,eI,eJ,eK,eL,eM,eN,eO,eP,eQ,eR,eS,eT,eU,eV,eW,eX,eY,eZ,

	eLeftButton,
	eRightButton,
	eCancel,
	eMiddleButton,

	eBack,
	eTab,
	eClear,
	eReturn,
	eShift,
	eControl,
	eAlt,
	ePause,
	eCapsLock,

	eEscape,

	ePrior,
	eNext,
	eEnd,
	eHome,
	eLeft,
	eUp,
	eRight,
	eDown,
	eSelect,
	ePrint,
	eExecute,
	eSnapshot,
	eInsert,
	eDelete,
	eHelp,

	eSleep,

	eNumpad0,
	eNumpad1,
	eNumpad2,
	eNumpad3,
	eNumpad4,
	eNumpad5,
	eNumpad6,
	eNumpad7,
	eNumpad8,
	eNumpad9,
	eMultiply,
	eAdd,
	eSeparator,
	eSubtract,
	eDecimal,
	eDivide,
	eF1,
	eF2,
	eF3,
	eF4,
	eF5,
	eF6,
	eF7,
	eF8,
	eF9,
	eF10,
	eF11,
	eF12,
	eF13,
	eF14,
	eF15,
	eF16,
	eF17,
	eF18,
	eF19,
	eF20,
	eF21,
	eF22,
	eF23,
	eF24,

	eNumLock,
	eScrollLock,

	eLeftShift,
	eRightShift,
	eLeftControl,
	eRightControl,
	eLeftAlt,
	eRightAlt,

	eMax
};

enum class EGamepadButton
{
	eDpadUp,
	eDpadDown,
	eDpadLeft,
	eDpadRight,
	eStart,
	eCenter,
	eBack,
	eLeftThumb,
	eRightThumb,
	eLeftShoulder,
	eRightShoulder,
	eLeftTrigger,
	eRightTrigger,
	eFaceUp,
	eFaceDown,
	eFaceLeft,
	eFaceRight
};

struct KeyboardStates
{
	//eastl::bitset<static_cast<Uint32>(AsciiKeys::eMax)>	  ascii_keys;
	eastl::bitset<512> keys_bitset;
};

struct GamepadStates
{
	eastl::bitset<32> buttons_bitset;
	float32_t axis[16];
};

/**
 * @brief Input handler class.
 *
 */
class Handler
{
	CLASS_BODY_NON_MOVEABLE_COPYABLE(Handler);

private:
	Handler();

public:
	~Handler();

public:
	void Update(RESULT_PARAM_DEFINE);

	NODISCARD bool IsGamepadButtonDown(EGamepadButton button) const;
	NODISCARD bool IsGamepadButtonUp(EGamepadButton button) const;
	NODISCARD bool IsKeyDown(EKey key) const;
	NODISCARD bool IsKeyUp(EKey key) const;

public:
	static Handler& Instance();

private:
#if PLATFORM_WINDOWS
	static LRESULT WindowProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
#endif

private:
	friend class Engine::Window;
	KeyboardStates					  keydown_states_{};
	KeyboardStates					  keyup_states_{};
	static eastl::unique_ptr<Handler> instance_;
};

} // namespace Input

} // namespace Engine

CLASS_VALIDATION(Engine::Input::Handler);

#endif
