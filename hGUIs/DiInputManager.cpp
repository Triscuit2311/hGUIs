#include "pch.h"
#include "DiInputManager.h"
#include "InputDefines.h"
#include "logging.h"


bool DiInputManager::IsStateByteSet(const BYTE byte)
{
	// Checks if the high bit of the byte is set (0x80 is 10000000 in binary).
	return 0x80 & byte;
}

BYTE DiInputManager::CompMouseEnum(const DiInput button)
{
	return button - 0xEE;
}

DiInputManager::DiInputManager()
{
	m_DirectInputInstance = nullptr;
	m_DiKeyboardDevice = nullptr;
	m_DiMouseDevice = nullptr;
	m_MouseAbsolutePosition = new tagPOINT();

	DiKeyNames = {
		{vKb_ESCAPE, "Esc"},
		{vKb_1, "1"},
		{vKb_2, "2"},
		{vKb_3, "3"},
		{vKb_4, "4"},
		{vKb_5, "5"},
		{vKb_6, "6"},
		{vKb_7, "7"},
		{vKb_8, "8"},
		{vKb_9, "9"},
		{vKb_0, "0"},
		{vKb_MINUS, "-"},
		{vKb_EQUALS, "="},
		{vKb_BACKSAPCE, "Backspace"},
		{vKb_TAB, "Tab"},
		{vKb_Q, "Q"},
		{vKb_W, "W"},
		{vKb_E, "E"},
		{vKb_R, "R"},
		{vKb_T, "T"},
		{vKb_Y, "Y"},
		{vKb_U, "U"},
		{vKb_I, "I"},
		{vKb_O, "O"},
		{vKb_P, "P"},
		{vKb_LBRACKET, "["},
		{vKb_RBRACKET, "]"},
		{vKb_RETURN, "Enter"},
		{vKb_LCONTROL, "Ctrl"},
		{vKb_A, "A"},
		{vKb_S, "S"},
		{vKb_D, "D"},
		{vKb_F, "F"},
		{vKb_G, "G"},
		{vKb_H, "H"},
		{vKb_J, "J"},
		{vKb_K, "K"},
		{vKb_L, "L"},
		{vKb_SEMICOLON, ";"},
		{vKb_APOSTROPHE, "'"},
		{vKb_GRAVE, "`"},
		{vKb_LSHIFT, "Left Shift"},
		{vKb_BACKSLASH, "\\"},
		{vKb_Z, "Z"},
		{vKb_X, "X"},
		{vKb_C, "C"},
		{vKb_V, "V"},
		{vKb_B, "B"},
		{vKb_N, "N"},
		{vKb_M, "M"},
		{vKb_COMMA, ","},
		{vKb_PERIOD, "."},
		{vKb_SLASH, "/"},
		{vKb_RSHIFT, "Right Shift"},
		{vKb_NUMPADMULTIPLY, "Numpad *"},
		{vKb_LALT, "Left Alt"},
		{vKb_SPACE, "Spacebar"},
		{vKb_CAPSLOCK, "Caps Lock"},
		{vKb_F1, "F1"},
		{vKb_F2, "F2"},
		{vKb_F3, "F3"},
		{vKb_F4, "F4"},
		{vKb_F5, "F5"},
		{vKb_F6, "F6"},
		{vKb_F7, "F7"},
		{vKb_F8, "F8"},
		{vKb_F9, "F9"},
		{vKb_F10, "F10"},
		{vKb_NUMLOCK, "Num Lock"},
		{vKb_SCROLL, "Scroll Lock"},
		{vKb_NUMPAD7, "Numpad 7"},
		{vKb_NUMPAD8, "Numpad 8"},
		{vKb_NUMPAD9, "Numpad 9"},
		{vKb_NUMPADSUBTRACT, "Numpad -"},
		{vKb_NUMPAD4, "Numpad 4"},
		{vKb_NUMPAD5, "Numpad 5"},
		{vKb_NUMPAD6, "Numpad 6"},
		{vKb_NUMPADADD, "Numpad +"},
		{vKb_NUMPAD1, "Numpad 1"},
		{vKb_NUMPAD2, "Numpad 2"},
		{vKb_NUMPAD3, "Numpad 3"},
		{vKb_NUMPAD0, "Numpad 0"},
		{vKb_NUMPADDECIMAL, "Numpad ."},
		{vKb_OEM_102, "OEM 102"},
		{vKb_F11, "F11"},
		{vKb_F12, "F12"},
		{vKb_F13, "F13"},
		{vKb_F14, "F14"},
		{vKb_F15, "F15"},
		{vKb_KANA, "Kana"},
		{vKb_ABNT_C1, "/?"},
		{vKb_CONVERT, "Convert"},
		{vKb_NOCONVERT, "No Convert"},
		{vKb_YEN, "Yen"},
		{vKb_ABNT_C2, "Numpad ."},
		{vKb_NUMPADEQUALS, "Numpad ="},
		{vKb_PREVTRACK, "Previous Track"},
		{vKb_AT, "@"},
		{vKb_COLON, ":"},
		{vKb_UNDERLINE, "_"},
		{vKb_KANJI, "Kanji"},
		{vKb_STOP, "Stop"},
		{vKb_AX, "AX"},
		{vKb_UNLABELED, "Unlabeled"},
		{vKb_NEXTTRACK, "Next Track"},
		{vKb_NUMPADENTER, "Numpad Enter"},
		{vKb_RCONTROL, "Right Ctrl"},
		{vKb_MUTE, "Mute"},
		{vKb_CALCULATOR, "Calculator"},
		{vKb_PLAYPAUSE, "Play / Pause"},
		{vKb_MEDIASTOP, "Media Stop"},
		{vKb_VOLUMEDOWN, "Volume -"},
		{vKb_VOLUMEUP, "Volume +"},
		{vKb_WEBHOME, "Web Home"},
		{vKb_NUMPADCOMMA, "Numpad ,"},
		{vKb_NUMPADDIVIDE, "Numpad /"},
		{vKb_SYSRQ, "SysRq"},
		{vKb_RMENU, "Right Alt"},
		{vKb_PAUSE, "Pause"},
		{vKb_HOME, "Home"},
		{vKb_UP, "Up Arrow"},
		{vKb_PAGEUP, "PgUp"},
		{vKb_LEFT, "Left Arrow"},
		{vKb_RIGHT, "Right Arrow"},
		{vKb_END, "End"},
		{vKb_DOWN, "Down Arrow"},
		{vKb_PAGEDOWN, "PgDn"},
		{vKb_INSERT, "Insert"},
		{vKb_DELETE_, "Delete"},
		{vKb_LWIN, "Left Windows key"},
		{vKb_RWIN, "Right Windows key"},
		{vKb_APPS, "AppMenu key"},
		{vKb_POWER, "System Power"},
		{vKb_SLEEP, "System Sleep"},
		{vKb_WAKE, "System Wake"},
		{vKb_WEBSEARCH, "Web Search"},
		{vKb_WEBFAVORITES, "Web Favorites"},
		{vKb_WEBREFRESH, "Web Refresh"},
		{vKb_WEBSTOP, "Web Stop"},
		{vKb_WEBFORWARD, "Web Forward"},
		{vKb_WEBBACK, "Web Back"},
		{vKb_MYCOMPUTER, "My Computer"},
		{vKb_MAIL, "Mail"},
		{vKb_MEDIASELECT, "Media Select"},
		{NONE_DONTUSE, ""},
		{vM_LEFTBTN, "Mouse Left"},
		{vM_RIGHTBTN, "Mouse Right"},
		{vM_MIDDLEBTN, "Mouse Middle"},
		{vM_XBTN1, "Mouse BTN 1"},
		{vM_XBTN2, "Mouse BTN 2"},
		{vM_XBTN3, "Mouse BTN 3"},
		{vM_XBTN4, "Mouse BTN 4"},
		{vM_XBTN5, "Mouse BTN 5"}
	};
}

DiInputManager::~DiInputManager()
{
	if (m_DiKeyboardDevice)
	{
		m_DiKeyboardDevice->Unacquire();
		m_DiKeyboardDevice->Release();
		m_DiKeyboardDevice = nullptr;
	}
	if (m_DiMouseDevice)
	{
		m_DiMouseDevice->Unacquire();
		m_DiMouseDevice->Release();
		m_DiMouseDevice = nullptr;
	}
	if (m_DirectInputInstance)
	{
		m_DirectInputInstance->Release();
		m_DirectInputInstance = nullptr;
	}
}

void DiInputManager::Init(const HWND hostWindow)
{
	DirectInput8Create(
		GetModuleHandle(nullptr), // Get the handle of the current module.
		DIRECTINPUT_VERSION, // Use DirectInput version 8.
		IID_IDirectInput8, // Use the IDirectInput8 interface.
		reinterpret_cast<void**>(&m_DirectInputInstance),
		nullptr); // No need to set a security descriptor.

	// Create a keyboard device and set its data format.
	m_DirectInputInstance->CreateDevice(GUID_SysKeyboard, &m_DiKeyboardDevice, nullptr);
	m_DiKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);

	// Set the cooperative level of the keyboard device.
	m_DiKeyboardDevice->SetCooperativeLevel(hostWindow, CooperativeLevelFlags);

	m_DiKeyboardDevice->Acquire();

	// Create a mouse device and set its data format.
	m_DirectInputInstance->CreateDevice(GUID_SysMouse, &m_DiMouseDevice, nullptr);
	m_DiMouseDevice->SetDataFormat(&c_dfDIMouse2);

	// Set the cooperative level of the mouse device.
	m_DiMouseDevice->SetCooperativeLevel(hostWindow, CooperativeLevelFlags);
	m_DiMouseDevice->Acquire();
}

void DiInputManager::UpdateDeviceStates()
{
	HRESULT result = m_DiKeyboardDevice->GetDeviceState(
		sizeof(m_CurrentKeyboardState),
		&m_CurrentKeyboardState);

	if (FAILED(result))
		ERR("Failed to get Keyboard state");

	result = m_DiMouseDevice->GetDeviceState(
		sizeof(m_CurrentMouseState),
		&m_CurrentMouseState);

	if (FAILED(result))
		ERR("Failed to get Mouse state");
}

void DiInputManager::RetainDeviceStates()
{
	memcpy_s(m_LastKeyboardState, sizeof(m_LastKeyboardState),
	         m_CurrentKeyboardState, sizeof(m_CurrentKeyboardState));
	m_LastMouseState = m_CurrentMouseState;
}

void DiInputManager::ReacquireDevices() const
{
	m_DiKeyboardDevice->Unacquire();
	m_DiMouseDevice->Unacquire();

	m_DiKeyboardDevice->Acquire();
	m_DiMouseDevice->Acquire();
}

bool DiInputManager::IsInputDown(const DiInput input) const
{
	if (input > 0xEE)
		return IsMouseButtonDown(input);
	return IsKeyDown(input);
}

bool DiInputManager::IsInputUp(const DiInput input) const
{
	if (input > 0xEE)
		return IsMouseButtonUp(input);
	return IsKeyUp(input);
}

bool DiInputManager::IsInputJustPressed(const DiInput input) const
{
	if (input > 0xEE)
		return IsMouseButtonJustPressed(input);
	return IsKeyJustPressed(input);
}

bool DiInputManager::IsInputJustReleased(const DiInput input) const
{
	if (input > 0xEE)
		return IsMouseButtonJustReleased(input);
	return IsKeyJustReleased(input);
}

bool DiInputManager::IsKeyDown(const DiInput key) const
{
	return IsStateByteSet(m_CurrentKeyboardState[key]);
}

bool DiInputManager::IsKeyUp(const DiInput key) const
{
	return !IsStateByteSet(m_CurrentKeyboardState[key]);
}

bool DiInputManager::IsKeyJustPressed(const DiInput key) const
{
	return IsStateByteSet(m_CurrentKeyboardState[key])
		&& !IsStateByteSet(m_LastKeyboardState[key]);
}

bool DiInputManager::IsKeyJustReleased(const DiInput key) const
{
	return !IsStateByteSet(m_CurrentKeyboardState[key])
		&& IsStateByteSet(m_LastKeyboardState[key]);
}

bool DiInputManager::IsKeyCombinationJustPressed(const std::vector<DiInput>& keys) const
{
	// Initialize the flag to false
	bool justPressedFlag = false;

	// Loop through each key in the combination
	for (const auto key : keys)
	{
		// If any key in the combination is not pressed, the combination is not just pressed
		if (!IsKeyDown(key))
		{
			return false;
		}

		// If the flag is not set, check if the key has just been pressed
		if (!justPressedFlag)
		{
			if (IsKeyJustPressed(key))
			{
				justPressedFlag = true;
			}
		}
	}

	// If the flag is set, the key combination has just been pressed
	if (justPressedFlag)
	{
		return true;
	}

	// Otherwise, the key combination has not been just pressed
	return false;
}

bool DiInputManager::IsMouseButtonDown(const DiInput button) const
{
	return IsStateByteSet(m_CurrentMouseState.rgbButtons[CompMouseEnum(button)]);
}

bool DiInputManager::IsMouseButtonUp(const DiInput button) const
{
	return !IsStateByteSet(m_CurrentMouseState.rgbButtons[CompMouseEnum(button)]);
}

bool DiInputManager::IsMouseButtonJustPressed(const DiInput button) const
{
	return IsStateByteSet(m_CurrentMouseState.rgbButtons[CompMouseEnum(button)])
		&& !IsStateByteSet(m_LastMouseState.rgbButtons[CompMouseEnum(button)]);
}

bool DiInputManager::IsMouseButtonJustReleased(const DiInput button) const
{
	return !IsStateByteSet(m_CurrentMouseState.rgbButtons[CompMouseEnum(button)])
		&& IsStateByteSet(m_LastMouseState.rgbButtons[CompMouseEnum(button)]);
}

bool DiInputManager::HasMouseMoved() const
{
	return m_CurrentMouseState.lX != 0 || m_CurrentMouseState.lY != 0;
}

DiInputManager::MouseDeltas DiInputManager::GetMouseDeltas() const
{
	return {m_CurrentMouseState.lX, m_CurrentMouseState.lY};
}

long DiInputManager::GetMouseDeltaX() const
{
	return m_CurrentMouseState.lX;
}

long DiInputManager::GetMouseDeltaY() const
{
	return m_CurrentMouseState.lY;
}

long DiInputManager::GetScrollWheelDelta() const
{
	return m_CurrentMouseState.lZ;
}


std::pair<DiInputManager::DiInput, std::string> DiInputManager::GetNextInputInternal(
	const std::vector<DiInput>& excludes,
	const DiInput exitKey,
	const unsigned int cycleTimeout,
	const unsigned int cycleLatencyMs,
	const bool allowKeyboard,
	const bool allowMouse)
{
	unsigned int cycles = 0;
	while (true)
	{
		RetainDeviceStates();
		UpdateDeviceStates();

		for (auto keyPair : DiKeyNames)
		{

			if (keyPair.first == exitKey && (
				keyPair.first < 0xEE && IsKeyJustPressed(keyPair.first) ||
				keyPair.first > 0xEE && IsMouseButtonJustPressed(keyPair.first)
				)) {
				return { static_cast<DiInput>(0xEE), "NONE" };
			}

			if (!allowMouse && keyPair.first >= 0xEE) continue;
			if (!allowKeyboard && keyPair.first <= 0xEE) continue;

			if (std::find(excludes.begin(), excludes.end(), keyPair.first) != excludes.end()) continue;


			if (allowKeyboard && keyPair.first < 0xEE && IsKeyJustPressed(keyPair.first))
			{
				return keyPair;
			}

			if (allowMouse && keyPair.first > 0xEE && IsMouseButtonJustPressed(keyPair.first))
			{
				return keyPair;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(cycleLatencyMs));

		if (cycleTimeout > 0 && cycleTimeout < cycles)
		{
			INF("Breaking getinput early.");
			break;
		}

		cycles++;
	}

	return {static_cast<DiInput>(0xEE), "NONE"};
}


std::pair<DiInputManager::DiInput, std::string> DiInputManager::GetNextKey(
	const std::vector<DiInput>& excludes,
	const DiInput exitKey,
	const unsigned cycleTimeout,
	const unsigned cycleLatencyMs)
{
	return GetNextInputInternal(
		excludes, exitKey, cycleTimeout,
		cycleLatencyMs, true, false);
}

std::pair<DiInputManager::DiInput, std::string> DiInputManager::GetNextMouseButton(
	const std::vector<DiInput>& excludes,
	const DiInput exitKey,
	const unsigned cycleTimeout,
	const unsigned cycleLatencyMs)
{
	return GetNextInputInternal(
		excludes, exitKey, cycleTimeout,
		cycleLatencyMs, false, true);
}

std::pair<DiInputManager::DiInput, std::string> DiInputManager::GetNextInput(
	const std::vector<DiInput>& excludes,
    const DiInput exitKey,
    const unsigned cycleTimeout,
    const unsigned cycleLatencyMs)
{
	return GetNextInputInternal(
		excludes, exitKey, cycleTimeout,
		cycleLatencyMs, true, true);
}
