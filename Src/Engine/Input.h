
#ifndef ENGINE_INPUT_HANDLER_H
#define ENGINE_INPUT_HANDLER_H

#include "Engine/Common.h"
#include "Meta/EnumInfo.h"

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
	eA,
	eB,
	eC,
	eD,
	eE,
	eF,
	eG,
	eH,
	eI,
	eJ,
	eK,
	eL,
	eM,
	eN,
	eO,
	eP,
	eQ,
	eR,
	eS,
	eT,
	eU,
	eV,
	eW,
	eX,
	eY,
	eZ,

	eLeftButton,
	eRightButton,
	eCancel,
	eMiddleButton,

	eBack,
	eTab,
	eClear,
	eEnter,
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

namespace EKeyModifiers
{
using primitive_t = uint32_t;
enum EType : primitive_t
{
	eNone		  = 0,
	eLeftShift	  = 1 << 0,
	eRightShift	  = 1 << 1,
	eLeftControl  = 1 << 2,
	eRightControl = 1 << 3,
	eLeftAlt	  = 1 << 4,
	eRightAlt	  = 1 << 5
};
} // namespace EKeyModifiers

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
	eastl::bitset<256>		   KeysBitset;
	EKeyModifiers::primitive_t KeysModifiers;
};

struct GamepadStates
{
	eastl::bitset<32> ButtonsBitset;
	float32_t		  Axis[16];
};

/**
 * @brief Input handler class.
 *
 */
class Manager
{
	CLASS_BODY_NON_MOVEABLE_COPYABLE(Manager);

private:
	Manager();

public:
	~Manager();

public:
	void Update(RESULT_PARAM_DEFINE);

	NODISCARD bool IsGamepadButtonDown(EGamepadButton Button) const;
	NODISCARD bool IsGamepadButtonUp(EGamepadButton Button) const;

	NODISCARD bool IsVsyncKeyDown(bool AutoReset = true);
	void		   ResetVsyncKeyDown();

	NODISCARD bool IsFullscreenKeyDown(bool AutoReset = true);
	void		   ResetFullscreenKeyDown();

	NODISCARD bool IsKeyDown(EKey Key, EKeyModifiers::primitive_t Modifiers = 0, bool AutoReset = false);
	void		   ResetKeyDown(EKey Key);

	NODISCARD bool IsKeyUp(EKey Key, EKeyModifiers::primitive_t Modifiers = 0, bool AutoReset = false);
	void		   ResetKeyUp(EKey Key);

#if EDITOR
	NODISCARD bool IsEditorKeyDown(bool AutoReset = true);
	void		   ResetEditorKeyDown();
#endif

public:
public:
	static Manager& Instance();

private:
#if PLATFORM_WINDOWS
	static LRESULT WindowProc(HWND Hwnd, UINT Umsg, WPARAM Wparam, LPARAM Lparam);
#endif

private:
	friend class Engine::Window;
	KeyboardStates					  mKeydownStates{};
	KeyboardStates					  mKeyupStates{};
	static eastl::unique_ptr<Manager> mInstance;
};

INLINE Manager& Instance()
{
	return Manager::Instance();
}

} // namespace Input

} // namespace Engine

CLASS_VALIDATION(Engine::Input::Manager);

META_TYPE_BINDER_BEGIN(Engine::Input::EKey)
META_TYPE_BINDER_DEFAULT_OPERATIONS()
META_TYPE_BINDER_END()

#undef META_ENUM_INFO_TARGET
#define META_ENUM_INFO_TARGET Engine::Input::EKey
META_ENUM_INFO_BINDER_BEGIN(0)
ENUM_VALUE(eA), ENUM_VALUE(eB), ENUM_VALUE(eC), ENUM_VALUE(eD), ENUM_VALUE(eE), ENUM_VALUE(eF), ENUM_VALUE(eG),
	ENUM_VALUE(eH), ENUM_VALUE(eI), ENUM_VALUE(eJ), ENUM_VALUE(eK), ENUM_VALUE(eL), ENUM_VALUE(eM), ENUM_VALUE(eN),
	ENUM_VALUE(eO), ENUM_VALUE(eP), ENUM_VALUE(eQ), ENUM_VALUE(eR), ENUM_VALUE(eS), ENUM_VALUE(eT), ENUM_VALUE(eU),
	ENUM_VALUE(eV), ENUM_VALUE(eW), ENUM_VALUE(eX), ENUM_VALUE(eY), ENUM_VALUE(eZ),

	ENUM_VALUE(eLeftButton), ENUM_VALUE(eRightButton), ENUM_VALUE(eCancel), ENUM_VALUE(eMiddleButton),

	ENUM_VALUE(eBack), ENUM_VALUE(eTab), ENUM_VALUE(eClear), ENUM_VALUE(eEnter), ENUM_VALUE(eShift),
	ENUM_VALUE(eControl), ENUM_VALUE(eAlt), ENUM_VALUE(ePause), ENUM_VALUE(eCapsLock),

	ENUM_VALUE(eEscape),

	ENUM_VALUE(ePrior), ENUM_VALUE(eNext), ENUM_VALUE(eEnd), ENUM_VALUE(eHome), ENUM_VALUE(eLeft), ENUM_VALUE(eUp),
	ENUM_VALUE(eRight), ENUM_VALUE(eDown), ENUM_VALUE(eSelect), ENUM_VALUE(ePrint), ENUM_VALUE(eExecute),
	ENUM_VALUE(eSnapshot), ENUM_VALUE(eInsert), ENUM_VALUE(eDelete), ENUM_VALUE(eHelp),

	ENUM_VALUE(eSleep),

	ENUM_VALUE(eNumpad0), ENUM_VALUE(eNumpad1), ENUM_VALUE(eNumpad2), ENUM_VALUE(eNumpad3), ENUM_VALUE(eNumpad4),
	ENUM_VALUE(eNumpad5), ENUM_VALUE(eNumpad6), ENUM_VALUE(eNumpad7), ENUM_VALUE(eNumpad8), ENUM_VALUE(eNumpad9),
	ENUM_VALUE(eMultiply), ENUM_VALUE(eAdd), ENUM_VALUE(eSeparator), ENUM_VALUE(eSubtract), ENUM_VALUE(eDecimal),
	ENUM_VALUE(eDivide), ENUM_VALUE(eF1), ENUM_VALUE(eF2), ENUM_VALUE(eF3), ENUM_VALUE(eF4), ENUM_VALUE(eF5),
	ENUM_VALUE(eF6), ENUM_VALUE(eF7), ENUM_VALUE(eF8), ENUM_VALUE(eF9), ENUM_VALUE(eF10), ENUM_VALUE(eF11),
	ENUM_VALUE(eF12), ENUM_VALUE(eF13), ENUM_VALUE(eF14), ENUM_VALUE(eF15), ENUM_VALUE(eF16), ENUM_VALUE(eF17),
	ENUM_VALUE(eF18), ENUM_VALUE(eF19), ENUM_VALUE(eF20), ENUM_VALUE(eF21), ENUM_VALUE(eF22), ENUM_VALUE(eF23),
	ENUM_VALUE(eF24),

	ENUM_VALUE(eNumLock), ENUM_VALUE(eScrollLock),

	ENUM_VALUE(eLeftShift), ENUM_VALUE(eRightShift), ENUM_VALUE(eLeftControl), ENUM_VALUE(eRightControl),
	ENUM_VALUE(eLeftAlt), ENUM_VALUE(eRightAlt) META_ENUM_INFO_BINDER_END()

#endif
