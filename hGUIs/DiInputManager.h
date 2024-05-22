#pragma once

class DiInputManager
{
public:
	enum DiInput : BYTE
	{
		vKb_ESCAPE = 0x01,
		vKb_1 = 0x02,
		vKb_2 = 0x03,
		vKb_3 = 0x04,
		vKb_4 = 0x05,
		vKb_5 = 0x06,
		vKb_6 = 0x07,
		vKb_7 = 0x08,
		vKb_8 = 0x09,
		vKb_9 = 0x0A,
		vKb_0 = 0x0B,
		vKb_MINUS = 0x0C,
		/* - on main keyboard */
		vKb_EQUALS = 0x0D,
		vKb_BACKSAPCE = 0x0E,
		/* backspace */
		vKb_TAB = 0x0F,
		vKb_Q = 0x10,
		vKb_W = 0x11,
		vKb_E = 0x12,
		vKb_R = 0x13,
		vKb_T = 0x14,
		vKb_Y = 0x15,
		vKb_U = 0x16,
		vKb_I = 0x17,
		vKb_O = 0x18,
		vKb_P = 0x19,
		vKb_LBRACKET = 0x1A,
		vKb_RBRACKET = 0x1B,
		vKb_RETURN = 0x1C,
		/* Enter on main keyboard */
		vKb_LCONTROL = 0x1D,
		vKb_A = 0x1E,
		vKb_S = 0x1F,
		vKb_D = 0x20,
		vKb_F = 0x21,
		vKb_G = 0x22,
		vKb_H = 0x23,
		vKb_J = 0x24,
		vKb_K = 0x25,
		vKb_L = 0x26,
		vKb_SEMICOLON = 0x27,
		vKb_APOSTROPHE = 0x28,
		vKb_GRAVE = 0x29,
		/* accent grave */
		vKb_LSHIFT = 0x2A,
		vKb_BACKSLASH = 0x2B,
		vKb_Z = 0x2C,
		vKb_X = 0x2D,
		vKb_C = 0x2E,
		vKb_V = 0x2F,
		vKb_B = 0x30,
		vKb_N = 0x31,
		vKb_M = 0x32,
		vKb_COMMA = 0x33,
		vKb_PERIOD = 0x34,
		/* . on main keyboard */
		vKb_SLASH = 0x35,
		/* / on main keyboard */
		vKb_RSHIFT = 0x36,
		vKb_NUMPADMULTIPLY = 0x37,
		/* * on numeric keypad */
		vKb_LALT = 0x38,
		/* left Alt */
		vKb_SPACE = 0x39,
		vKb_CAPSLOCK = 0x3A,
		vKb_F1 = 0x3B,
		vKb_F2 = 0x3C,
		vKb_F3 = 0x3D,
		vKb_F4 = 0x3E,
		vKb_F5 = 0x3F,
		vKb_F6 = 0x40,
		vKb_F7 = 0x41,
		vKb_F8 = 0x42,
		vKb_F9 = 0x43,
		vKb_F10 = 0x44,
		vKb_NUMLOCK = 0x45,
		vKb_SCROLL = 0x46,
		/* Scroll Lock */
		vKb_NUMPAD7 = 0x47,
		vKb_NUMPAD8 = 0x48,
		vKb_NUMPAD9 = 0x49,
		vKb_NUMPADSUBTRACT = 0x4A,
		/* - on numeric keypad */
		vKb_NUMPAD4 = 0x4B,
		vKb_NUMPAD5 = 0x4C,
		vKb_NUMPAD6 = 0x4D,
		vKb_NUMPADADD = 0x4E,
		/* + on numeric keypad */
		vKb_NUMPAD1 = 0x4F,
		vKb_NUMPAD2 = 0x50,
		vKb_NUMPAD3 = 0x51,
		vKb_NUMPAD0 = 0x52,
		vKb_NUMPADDECIMAL = 0x53,
		/* . on numeric keypad */
		vKb_OEM_102 = 0x56,
		/* <> or \| on RT 102-key keyboard (Non-U.S.) */
		vKb_F11 = 0x57,
		vKb_F12 = 0x58,
		vKb_F13 = 0x64,
		/*                     (NEC PC98) */
		vKb_F14 = 0x65,
		/*                     (NEC PC98) */
		vKb_F15 = 0x66,
		/*                     (NEC PC98) */
		vKb_KANA = 0x70,
		/* (Japanese keyboard)            */
		vKb_ABNT_C1 = 0x73,
		/* /? on Brazilian keyboard */
		vKb_CONVERT = 0x79,
		/* (Japanese keyboard)            */
		vKb_NOCONVERT = 0x7B,
		/* (Japanese keyboard)            */
		vKb_YEN = 0x7D,
		/* (Japanese keyboard)            */
		vKb_ABNT_C2 = 0x7E,
		/* Numpad . on Brazilian keyboard */
		vKb_NUMPADEQUALS = 0x8D,
		/* = on numeric keypad (NEC PC98) */
		vKb_PREVTRACK = 0x90,
		/* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard */
		vKb_AT = 0x91,
		/*                     (NEC PC98) */
		vKb_COLON = 0x92,
		/*                     (NEC PC98) */
		vKb_UNDERLINE = 0x93,
		/*                     (NEC PC98) */
		vKb_KANJI = 0x94,
		/* (Japanese keyboard)            */
		vKb_STOP = 0x95,
		/*                     (NEC PC98) */
		vKb_AX = 0x96,
		/*                     (Japan AX) */
		vKb_UNLABELED = 0x97,
		/*                        (J3100) */
		vKb_NEXTTRACK = 0x99,
		/* Next Track */
		vKb_NUMPADENTER = 0x9C,
		/* Enter on numeric keypad */
		vKb_RCONTROL = 0x9D,
		vKb_MUTE = 0xA0,
		/* Mute */
		vKb_CALCULATOR = 0xA1,
		/* Calculator */
		vKb_PLAYPAUSE = 0xA2,
		/* Play / Pause */
		vKb_MEDIASTOP = 0xA4,
		/* Media Stop */
		vKb_VOLUMEDOWN = 0xAE,
		/* Volume - */
		vKb_VOLUMEUP = 0xB0,
		/* Volume + */
		vKb_WEBHOME = 0xB2,
		/* Web home */
		vKb_NUMPADCOMMA = 0xB3,
		/* , on numeric keypad (NEC PC98) */
		vKb_NUMPADDIVIDE = 0xB5,
		/* / on numeric keypad */
		vKb_SYSRQ = 0xB7,
		vKb_RMENU = 0xB8,
		/* right Alt */
		vKb_PAUSE = 0xC5,
		/* Pause */
		vKb_HOME = 0xC7,
		/* Home on arrow keypad */
		vKb_UP = 0xC8,
		/* UpArrow on arrow keypad */
		vKb_PAGEUP = 0xC9,
		/* PgUp on arrow keypad */
		vKb_LEFT = 0xCB,
		/* LeftArrow on arrow keypad */
		vKb_RIGHT = 0xCD,
		/* RightArrow on arrow keypad */
		vKb_END = 0xCF,
		/* End on arrow keypad */
		vKb_DOWN = 0xD0,
		/* DownArrow on arrow keypad */
		vKb_PAGEDOWN = 0xD1,
		/* PgDn on arrow keypad */
		vKb_INSERT = 0xD2,
		/* Insert on arrow keypad */
		vKb_DELETE_ = 0xD3,
		/* Delete on arrow keypad */
		vKb_LWIN = 0xDB,
		/* Left Windows key */
		vKb_RWIN = 0xDC,
		/* Right Windows key */
		vKb_APPS = 0xDD,
		/* AppMenu key */
		vKb_POWER = 0xDE,
		/* System Power */
		vKb_SLEEP = 0xDF,
		/* System Sleep */
		vKb_WAKE = 0xE3,
		/* System Wake */
		vKb_WEBSEARCH = 0xE5,
		/* Web Search */
		vKb_WEBFAVORITES = 0xE6,
		/* Web Favorites */
		vKb_WEBREFRESH = 0xE7,
		/* Web Refresh */
		vKb_WEBSTOP = 0xE8,
		/* Web Stop */
		vKb_WEBFORWARD = 0xE9,
		/* Web Forward */
		vKb_WEBBACK = 0xEA,
		/* Web Back */
		vKb_MYCOMPUTER = 0xEB,
		/* My Computer */
		vKb_MAIL = 0xEC,
		/* Mail */
		vKb_MEDIASELECT = 0xED,
		/* Media Select */
		NONE_DONTUSE = 0xEE,
		vM_LEFTBTN = 0xEE + 0x00,
		vM_RIGHTBTN = 0xEE + 0x01,
		vM_MIDDLEBTN = 0xEE + 0x02,
		vM_XBTN1 = 0xEE + 0x03,
		vM_XBTN2 = 0xEE + 0x04,
		vM_XBTN3 = 0xEE + 0x05,
		vM_XBTN4 = 0xEE + 0x06,
		vM_XBTN5 = 0xEE + 0x07
	};

	std::map<DiInput, std::string> DiKeyNames;
	std::chrono::time_point<std::chrono::system_clock> debounce_time{};
	unsigned long long debounce_length = 300;
	bool in_debounce_cycle = false;


private:
	LPDIRECTINPUT8 m_DirectInputInstance;

	LPDIRECTINPUTDEVICE8 m_DiKeyboardDevice;
	BYTE m_CurrentKeyboardState[256]{};
	BYTE m_LastKeyboardState[256]{};

	LPDIRECTINPUTDEVICE8 m_DiMouseDevice;
	DIMOUSESTATE2 m_CurrentMouseState{};
	DIMOUSESTATE2 m_LastMouseState{};
	LPPOINT m_MouseAbsolutePosition;

	const DWORD CooperativeLevelFlags = DISCL_BACKGROUND | DISCL_NONEXCLUSIVE;

	static bool IsStateByteSet(BYTE byte);
	static BYTE CompMouseEnum(DiInput button);
	std::pair<DiInputManager::DiInput, std::string> GetNextInputInternal(
		const std::vector<DiInput>& excludes,
		DiInput exitKey, unsigned cycleTimeout,
		unsigned cycleLatencyMs, bool allowKeyboard,
		bool allowMouse);

public:
	DiInputManager();
	~DiInputManager();

	struct MouseDeltas
	{
		long X;
		long Y;
	};

	/**
	 * Initializes the DirectInput8 instance and all connected devices.
	 *
	 * @param hostWindow The window handle for the host application.
	 */
	void Init(HWND hostWindow);

	/**
	 * Updates the state of all acquired devices.
	 */
	void UpdateDeviceStates();

	/**
	 * Retains the current state of all acquired devices.
	 */
	void RetainDeviceStates();

	/**
	 * Reacquires all connected devices.
	 */
	void ReacquireDevices() const;

	/**
	 * Checks whether the specified input is currently being held down.
	 *
	 * @param input The input to check.
	 * @return true if the input is being held down, false otherwise.
	 */
	bool IsInputDown(DiInput input) const;

	/**
	 * Checks whether the specified input is currently not being held down.
	 *
	 * @param input The input to check.
	 * @return true if the input is not being held down, false otherwise.
	 */
	bool IsInputUp(DiInput input) const;

	/**
	 * Checks whether the specified input has just been pressed (i.e., was not down previously).
	 *
	 * @param input The input to check.
	 * @return true if the input has just been pressed, false otherwise.
	 */
	bool IsInputJustPressed(DiInput input) const;

	/**
	 * Checks whether the specified input has just been released (i.e., is not down anymore).
	 *
	 * @param input The input to check.
	 * @return true if the input has just been released, false otherwise.
	 */
	bool IsInputJustReleased(DiInput input) const;


	/**
	 * Determines whether a specific keyboard key is currently pressed down.
	 *
	 * @param key The virtual key code of the key to check.
	 * @return True if the key is currently pressed down, false otherwise.
	 */
	bool IsKeyDown(DiInput key) const;

	/**
	 * Determines whether a specific keyboard key is currently released.
	 *
	 * @param key The virtual key code of the key to check.
	 * @return True if the key is currently released, false otherwise.
	 */
	bool IsKeyUp(DiInput key) const;

	/**
	 * Determines whether a specific keyboard key was just pressed down.
	 *
	 * @param key The virtual key code of the key to check.
	 * @return True if the key was just pressed down, false otherwise.
	 */
	bool IsKeyJustPressed(DiInput key) const;

	/**
	 * Determines whether a specific keyboard key was just released.
	 *
	 * @param key The virtual key code of the key to check.
	 * @return True if the key was just released, false otherwise.
	 */
	bool IsKeyJustReleased(DiInput key) const;

	/**
	 * Determines whether the given key combination has just been pressed.
	 *
	 * @param keys A vector containing the key codes of the keys in the combination.
	 * @return True if the key combination has just been pressed, false otherwise.
	 */
	bool IsKeyCombinationJustPressed(const std::vector<DiInput>& keys) const;


	/**
	 * Determines whether a specific mouse button is currently pressed down.
	 *
	 * @param button The virtual button code of the button to check.
	 * @return True if the button is currently pressed down, false otherwise.
	 */
	bool IsMouseButtonDown(DiInput button) const;

	/**
	 * Determines whether a specific mouse button is currently released.
	 *
	 * @param button The virtual button code of the button to check.
	 * @return True if the button is currently released, false otherwise.
	 */
	bool IsMouseButtonUp(DiInput button) const;

	/**
	 * Determines whether a specific mouse button was just pressed down.
	 *
	 * @param button The virtual button code of the button to check.
	 * @return True if the button was just pressed down, false otherwise.
	 */
	bool IsMouseButtonJustPressed(DiInput button) const;


	/**
	 * Determines whether a specific mouse button was just released.
	 *
	 * @param button The virtual button code of the button to check.
	 * @return True if the button was just released, false otherwise.
	 */
	bool IsMouseButtonJustReleased(DiInput button) const;

	/**
	 * Determines whether the mouse has moved since the last time its state was updated.
	 *
	 * @return True if the mouse has moved, false otherwise.
	 */
	bool HasMouseMoved() const;

	/**
	 * Gets the change in the mouse's position since the last time its state was updated.
	 *
	 * @return A `MouseDeltas` struct that contains the change in the mouse's position along the x and y axes.
	 */
	MouseDeltas GetMouseDeltas() const;

	/**
	 * Gets the change in the mouse's position along the x axis since the last time its state was updated.
	 *
	 * @return The change in the mouse's position along the x axis.
	 */
	long GetMouseDeltaX() const;

	/**
	 * Gets the change in the mouse's position along the y axis since the last time its state was updated.
	 *
	 * @return The change in the mouse's position along the y axis.
	 */
	long GetMouseDeltaY() const;

	/**
	 * Retrieves the amount that the scroll wheel has been moved since the last call to this function.
	 *
	 * @return The scroll wheel delta, which can be positive, negative, or zero.
	 */
	long GetScrollWheelDelta() const;

	void DeBounce();

	/**
	 * Waits for the next keyboard event and returns information about it.
	 * The function ignores any keys in the `excludes` vector.
	 *
	 * @param excludes The list of keys to exclude.
	 * @param exitKey The key that will cause the function to exit early. If set to `NONE_DONTUSE`, the function will not exit early.
	 * @param cycleTimeout The number of cycles to wait before timing out. A cycle is defined as waiting `cycleLatencyMs` milliseconds for input.
	 * @param cycleLatencyMs The latency (in milliseconds) of each cycle.
	 *
	 * @return A pair containing the key and a string describing the event (e.g., "A key down").
	 */
	std::pair<DiInput, std::string> GetNextKey(
		const std::vector<DiInput>& excludes = {},
		const DiInput exitKey = DiInput::NONE_DONTUSE,
		const unsigned cycleTimeout = 0,
		const unsigned cycleLatencyMs = 10);

	/**
	 * Waits for the next mouse button event and returns information about it.
	 * The function ignores any mouse buttons in the `excludes` vector.
	 *
	 * @param excludes The list of mouse buttons to exclude.
	 * @param exitKey The key that will cause the function to exit early. If set to `NONE_DONTUSE`, the function will not exit early.
	 * @param cycleTimeout The number of cycles to wait before timing out. A cycle is defined as waiting `cycleLatencyMs` milliseconds for input.
	 * @param cycleLatencyMs The latency (in milliseconds) of each cycle.
	 *
	 * @return A pair containing the mouse button and a string describing the event (e.g., "left mouse button down").
	 */
	std::pair<DiInput, std::string> GetNextMouseButton(
		const std::vector<DiInput>& excludes = {},
		const DiInput exitKey = DiInput::NONE_DONTUSE,
		const unsigned cycleTimeout = 1000,
		const unsigned cycleLatencyMs = 10);

	/**
	 * Waits for the next input event and returns information about it.
	 * The function ignores any inputs in the `excludes` vector.
	 *
	 * @param excludes The list of inputs to exclude.
	 * @param exitKey The key that will cause the function to exit early. If set to `NONE_DONTUSE`, the function will not exit early.
	 * @param cycleTimeout The number of cycles to wait before timing out. A cycle is defined as waiting `cycleLatencyMs` milliseconds for input.
	 * @param cycleLatencyMs The latency (in milliseconds) of each cycle.
	 *
	 * @return A pair containing the input and a string describing the event (e.g., "left mouse button down").
	 */
	std::pair<DiInput, std::string> GetNextInput(
		const std::vector<DiInput>& excludes = {},
		const DiInput exitKey = DiInput::NONE_DONTUSE,
		const unsigned cycleTimeout = 1000,
		const unsigned cycleLatencyMs = 10);
};
