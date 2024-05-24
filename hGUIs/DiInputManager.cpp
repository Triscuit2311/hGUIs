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
		{vKb_ESCAPE, L"Esc"},
		{vKb_1, L"1"},
		{vKb_2, L"2"},
		{vKb_3, L"3"},
		{vKb_4, L"4"},
		{vKb_5, L"5"},
		{vKb_6, L"6"},
		{vKb_7, L"7"},
		{vKb_8, L"8"},
		{vKb_9, L"9"},
		{vKb_0, L"0"},
		{vKb_MINUS, L"-"},
		{vKb_EQUALS, L"="},
		{vKb_BACKSAPCE, L"Backspace"},
		{vKb_TAB, L"Tab"},
		{vKb_Q, L"Q"},
		{vKb_W, L"W"},
		{vKb_E, L"E"},
		{vKb_R, L"R"},
		{vKb_T, L"T"},
		{vKb_Y, L"Y"},
		{vKb_U, L"U"},
		{vKb_I, L"I"},
		{vKb_O, L"O"},
		{vKb_P, L"P"},
		{vKb_LBRACKET, L"["},
		{vKb_RBRACKET, L"]"},
		{vKb_RETURN, L"Enter"},
		{vKb_LCONTROL, L"Ctrl"},
		{vKb_A, L"A"},
		{vKb_S, L"S"},
		{vKb_D, L"D"},
		{vKb_F, L"F"},
		{vKb_G, L"G"},
		{vKb_H, L"H"},
		{vKb_J, L"J"},
		{vKb_K, L"K"},
		{vKb_L, L"L"},
		{vKb_SEMICOLON, L";"},
		{vKb_APOSTROPHE, L"'"},
		{vKb_GRAVE, L"`"},
		{vKb_LSHIFT, L"Left Shift"},
		{vKb_BACKSLASH, L"\\"},
		{vKb_Z, L"Z"},
		{vKb_X, L"X"},
		{vKb_C, L"C"},
		{vKb_V, L"V"},
		{vKb_B, L"B"},
		{vKb_N, L"N"},
		{vKb_M, L"M"},
		{vKb_COMMA, L","},
		{vKb_PERIOD, L"."},
		{vKb_SLASH, L"/"},
		{vKb_RSHIFT, L"Right Shift"},
		{vKb_NUMPADMULTIPLY, L"Numpad *"},
		{vKb_LALT, L"Left Alt"},
		{vKb_SPACE, L"Spacebar"},
		{vKb_CAPSLOCK, L"Caps Lock"},
		{vKb_F1, L"F1"},
		{vKb_F2, L"F2"},
		{vKb_F3, L"F3"},
		{vKb_F4, L"F4"},
		{vKb_F5, L"F5"},
		{vKb_F6, L"F6"},
		{vKb_F7, L"F7"},
		{vKb_F8, L"F8"},
		{vKb_F9, L"F9"},
		{vKb_F10, L"F10"},
		{vKb_NUMLOCK, L"Num Lock"},
		{vKb_SCROLL, L"Scroll Lock"},
		{vKb_NUMPAD7, L"Numpad 7"},
		{vKb_NUMPAD8, L"Numpad 8"},
		{vKb_NUMPAD9, L"Numpad 9"},
		{vKb_NUMPADSUBTRACT, L"Numpad -"},
		{vKb_NUMPAD4, L"Numpad 4"},
		{vKb_NUMPAD5, L"Numpad 5"},
		{vKb_NUMPAD6, L"Numpad 6"},
		{vKb_NUMPADADD, L"Numpad +"},
		{vKb_NUMPAD1, L"Numpad 1"},
		{vKb_NUMPAD2, L"Numpad 2"},
		{vKb_NUMPAD3, L"Numpad 3"},
		{vKb_NUMPAD0, L"Numpad 0"},
		{vKb_NUMPADDECIMAL, L"Numpad ."},
		{vKb_OEM_102, L"OEM 102"},
		{vKb_F11, L"F11"},
		{vKb_F12, L"F12"},
		{vKb_F13, L"F13"},
		{vKb_F14, L"F14"},
		{vKb_F15, L"F15"},
		{vKb_KANA, L"Kana"},
		{vKb_ABNT_C1, L"/?"},
		{vKb_CONVERT, L"Convert"},
		{vKb_NOCONVERT, L"No Convert"},
		{vKb_YEN, L"Yen"},
		{vKb_ABNT_C2, L"Numpad ."},
		{vKb_NUMPADEQUALS, L"Numpad ="},
		{vKb_PREVTRACK, L"Previous Track"},
		{vKb_AT, L"@"},
		{vKb_COLON, L":"},
		{vKb_UNDERLINE, L"_"},
		{vKb_KANJI, L"Kanji"},
		{vKb_STOP, L"Stop"},
		{vKb_AX, L"AX"},
		{vKb_UNLABELED, L"Unlabeled"},
		{vKb_NEXTTRACK, L"Next Track"},
		{vKb_NUMPADENTER, L"Numpad Enter"},
		{vKb_RCONTROL, L"Right Ctrl"},
		{vKb_MUTE, L"Mute"},
		{vKb_CALCULATOR, L"Calculator"},
		{vKb_PLAYPAUSE, L"Play / Pause"},
		{vKb_MEDIASTOP, L"Media Stop"},
		{vKb_VOLUMEDOWN, L"Volume -"},
		{vKb_VOLUMEUP, L"Volume +"},
		{vKb_WEBHOME, L"Web Home"},
		{vKb_NUMPADCOMMA, L"Numpad ,"},
		{vKb_NUMPADDIVIDE, L"Numpad /"},
		{vKb_SYSRQ, L"SysRq"},
		{vKb_RMENU, L"Right Alt"},
		{vKb_PAUSE, L"Pause"},
		{vKb_HOME, L"Home"},
		{vKb_UP, L"Up Arrow"},
		{vKb_PAGEUP, L"PgUp"},
		{vKb_LEFT, L"Left Arrow"},
		{vKb_RIGHT, L"Right Arrow"},
		{vKb_END, L"End"},
		{vKb_DOWN, L"Down Arrow"},
		{vKb_PAGEDOWN, L"PgDn"},
		{vKb_INSERT, L"Insert"},
		{vKb_DELETE_, L"Delete"},
		{vKb_LWIN, L"Left Windows key"},
		{vKb_RWIN, L"Right Windows key"},
		{vKb_APPS, L"AppMenu key"},
		{vKb_POWER, L"System Power"},
		{vKb_SLEEP, L"System Sleep"},
		{vKb_WAKE, L"System Wake"},
		{vKb_WEBSEARCH, L"Web Search"},
		{vKb_WEBFAVORITES, L"Web Favorites"},
		{vKb_WEBREFRESH, L"Web Refresh"},
		{vKb_WEBSTOP, L"Web Stop"},
		{vKb_WEBFORWARD, L"Web Forward"},
		{vKb_WEBBACK, L"Web Back"},
		{vKb_MYCOMPUTER, L"My Computer"},
		{vKb_MAIL, L"Mail"},
		{vKb_MEDIASELECT, L"Media Select"},
		{NONE_DONTUSE, L""},
		{vM_LEFTBTN, L"Mouse Left"},
		{vM_RIGHTBTN, L"Mouse Right"},
		{vM_MIDDLEBTN, L"Mouse Middle"},
		{vM_XBTN1, L"Mouse BTN 1"},
		{vM_XBTN2, L"Mouse BTN 2"},
		{vM_XBTN3, L"Mouse BTN 3"},
		{vM_XBTN4, L"Mouse BTN 4"},
		{vM_XBTN5, L"Mouse BTN 5"}
	};
}


std::wstring DiInputManager::GetInputName(const DiInputManager::DiInput key)
{
	if (!DiKeyNames.contains(key)) { return L"NONE"; }
	return DiKeyNames.at(key);
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
	auto curr = std::chrono::system_clock::now();
	if(std::chrono::duration_cast<std::chrono::milliseconds>(curr - debounce_time).count() >= debounce_length)
	{
		in_debounce_cycle = false;
	}

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
	if (in_debounce_cycle) { return false; }

	if (input > 0xEE)
		return IsMouseButtonDown(input);
	return IsKeyDown(input);
}

bool DiInputManager::IsInputUp(const DiInput input) const
{
	if (in_debounce_cycle) { return false; }

	if (input > 0xEE)
		return IsMouseButtonUp(input);
	return IsKeyUp(input);
}

bool DiInputManager::IsInputJustPressed(const DiInput input) const
{
	if (in_debounce_cycle) { return false; }

	if (input > 0xEE)
		return IsMouseButtonJustPressed(input);
	return IsKeyJustPressed(input);
}

bool DiInputManager::IsInputJustReleased(const DiInput input) const
{
	if (in_debounce_cycle) { return false; }

	if (input > 0xEE)
		return IsMouseButtonJustReleased(input);
	return IsKeyJustReleased(input);
}

bool DiInputManager::IsKeyDown(const DiInput key) const
{
	if (in_debounce_cycle) { return false; }

	return IsStateByteSet(m_CurrentKeyboardState[key]);
}

bool DiInputManager::IsKeyUp(const DiInput key) const
{
	if (in_debounce_cycle) { return false; }

	return !IsStateByteSet(m_CurrentKeyboardState[key]);
}

bool DiInputManager::IsKeyJustPressed(const DiInput key) const
{
	if (in_debounce_cycle) { return false; }

	return IsStateByteSet(m_CurrentKeyboardState[key])
		&& !IsStateByteSet(m_LastKeyboardState[key]);
}

bool DiInputManager::IsKeyJustReleased(const DiInput key) const
{
	if (in_debounce_cycle) { return false; }

	return !IsStateByteSet(m_CurrentKeyboardState[key])
		&& IsStateByteSet(m_LastKeyboardState[key]);
}

bool DiInputManager::IsKeyCombinationJustPressed(const std::vector<DiInput>& keys) const
{
	if (in_debounce_cycle) { return false; }

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
	if (in_debounce_cycle) { return false; }
	return IsStateByteSet(m_CurrentMouseState.rgbButtons[CompMouseEnum(button)]);
}

bool DiInputManager::IsMouseButtonUp(const DiInput button) const
{
	if (in_debounce_cycle) { return false; }

	return !IsStateByteSet(m_CurrentMouseState.rgbButtons[CompMouseEnum(button)]);
}

bool DiInputManager::IsMouseButtonJustPressed(const DiInput button) const
{
	if (in_debounce_cycle) { return false; }

	return IsStateByteSet(m_CurrentMouseState.rgbButtons[CompMouseEnum(button)])
		&& !IsStateByteSet(m_LastMouseState.rgbButtons[CompMouseEnum(button)]);
}

bool DiInputManager::IsMouseButtonJustReleased(const DiInput button) const
{
	if (in_debounce_cycle) { return false; }

	return !IsStateByteSet(m_CurrentMouseState.rgbButtons[CompMouseEnum(button)])
		&& IsStateByteSet(m_LastMouseState.rgbButtons[CompMouseEnum(button)]);
}

bool DiInputManager::HasMouseMoved() const
{
	if (in_debounce_cycle) { return false; }
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

void DiInputManager::DeBounce()
{
	this->debounce_time = std::chrono::system_clock::now();
	in_debounce_cycle = true;
}


std::pair<DiInputManager::DiInput, std::wstring> DiInputManager::GetNextInputInternal(
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
				return { static_cast<DiInput>(0xEE), L"NONE" };
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

	return {static_cast<DiInput>(0xEE), L"NONE"};
}



bool DiInputManager::ScanInputs(
	DiInput& out,
	const bool allowKeyboard,
	const bool allowMouse
	){

	RetainDeviceStates();
	UpdateDeviceStates();


	for (const auto key : DiKeyNames | std::views::keys)
	{
		if (!allowMouse && key >= 0xEE) continue;
		if (!allowKeyboard && key <= 0xEE) continue;
		if (key == DiInput::vKb_ESCAPE)
		{
			continue;
		}

		if (allowKeyboard && key < 0xEE && IsKeyDown(key))
		{
			out = key;
			return true;
		}

		if (allowMouse && key > 0xEE && IsMouseButtonDown(key))
		{
			out = key;
			return true;
		}
	}

	return false;
}


std::pair<DiInputManager::DiInput, std::wstring> DiInputManager::GetNextKey(
	const std::vector<DiInput>& excludes,
	const DiInput exitKey,
	const unsigned cycleTimeout,
	const unsigned cycleLatencyMs)
{
	return GetNextInputInternal(
		excludes, exitKey, cycleTimeout,
		cycleLatencyMs, true, false);
}

std::pair<DiInputManager::DiInput, std::wstring> DiInputManager::GetNextMouseButton(
	const std::vector<DiInput>& excludes,
	const DiInput exitKey,
	const unsigned cycleTimeout,
	const unsigned cycleLatencyMs)
{
	return GetNextInputInternal(
		excludes, exitKey, cycleTimeout,
		cycleLatencyMs, false, true);
}

std::pair<DiInputManager::DiInput, std::wstring> DiInputManager::GetNextInput(
	const std::vector<DiInput>& excludes,
    const DiInput exitKey,
    const unsigned cycleTimeout,
    const unsigned cycleLatencyMs)
{
	return GetNextInputInternal(
		excludes, exitKey, cycleTimeout,
		cycleLatencyMs, true, true);
}
