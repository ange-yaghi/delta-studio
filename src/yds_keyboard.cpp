#include "../include/yds_keyboard.h"

ysKey::ysKey() {
	m_state = KEY_UP;
	m_configuration = KEY_CONF_UNDEFINED;
}

ysKey::~ysKey() {
    /* void */
}

void ysKey::Reset() {
	m_state = KEY_UP;
	m_configuration = KEY_CONF_UNDEFINED;
}

const char ysKeyboard::CHAR_MAP[] = {
	'\b',	// KEY_BACK
	'\t',	// KEY_TAB

	'\0',	// KEY_CLEAR

	'\n',	// KEY_RETURN
	'\0',	// KEY_SHIFT
	'\0',	// KEY_CONTROL

	'\0',	// KEY_MENU
	'\0',	// KEY_PAUSE
	'\0',	// KEY_CAPSLOCK

	'\0',	// KEY_ESCAPE
	'\0',	// KEY_CONVERT
	'\0',	// KEY_NONCONVERT
	'\0',	// KEY_ACCEPT
	'\0',	// KEY_MODECHANGE

	' ',	// KEY_SPACE
	'\0',	// KEY_PAGEUP
	'\0',	// KEY_PAGEDOWN
	'\0',	// KEY_END
	'\0',	// KEY_HOME

	'\0',	// KEY_LEFT
	'\0',	// KEY_UP
	'\0',	// KEY_RIGHT
	'\0',	// KEY_DOWN

	'\0',	// KEY_SELECT
	'\0',	// KEY_PRINT
	'\0',	// KEY_PRINTSCREEN
	'\0',	// KEY_INSERT
	'\x7F',	// KEY_DELETE
	'\0',	// KEY_HELP

	'0',	// KEY_0
	'1',	// KEY_1
	'2',	// KEY_2
	'3',	// KEY_3
	'4',	// KEY_4
	'5',	// KEY_5
	'6',	// KEY_6
	'7',	// KEY_7
	'8',	// KEY_8
	'9',	// KEY_9

	'a',	// KEY_A
	'b',	// KEY_B
	'c',	// KEY_C
	'd',	// KEY_D
	'e',	// KEY_E
	'f',	// KEY_F
	'g',	// KEY_G
	'h',	// KEY_H
	'i',	// KEY_I
	'j',	// KEY_J
	'k',	// KEY_K
	'l',	// KEY_L
	'm',	// KEY_M
	'n',	// KEY_N
	'o',	// KEY_O
	'p',	// KEY_P
	'q',	// KEY_Q
	'r',	// KEY_R
	's',	// KEY_S
	't',	// KEY_T
	'u',	// KEY_U
	'v',	// KEY_V
	'w',	// KEY_W
	'x',	// KEY_X
	'y',	// KEY_Y
	'z',	// KEY_Z

	'\0',	// KEY_LEFTWIN
	'\0',	// KEY_RIGHTWIN
	'\0',	// KEY_APPS

	'\0',	// KEY_SLEEP

	'0',	// KEY_NUMPAD0
	'1',	// KEY_NUMPAD1
	'2',	// KEY_NUMPAD2
	'3',	// KEY_NUMPAD3
	'4',	// KEY_NUMPAD4
	'5',	// KEY_NUMPAD5
	'6',	// KEY_NUMPAD6
	'7',	// KEY_NUMPAD7
	'8',	// KEY_NUMPAD8
	'9',	// KEY_NUMPAD9

	'*',	// KEY_MULTIPLY
	'+',	// KEY_ADD
	'\0',	// KEY_SEPARATOR
	'-',	// KEY_SUBTRACT
	'.',	// KEY_DECIMAL
	'/',	// KEY_DIVIDE

	'\0',	// KEY_F1
	'\0',	// KEY_F2
	'\0',	// KEY_F3
	'\0',	// KEY_F4
	'\0',	// KEY_F5
	'\0',	// KEY_F6
	'\0',	// KEY_F7
	'\0',	// KEY_F8
	'\0',	// KEY_F10
	'\0',	// KEY_F11
	'\0',	// KEY_F12
	'\0',	// KEY_F13
	'\0',	// KEY_F14
	'\0',	// KEY_F15
	'\0',	// KEY_F16
	'\0',	// KEY_F17
	'\0',	// KEY_F18
	'\0',	// KEY_F19
	'\0',	// KEY_F20
	'\0',	// KEY_F21
	'\0',	// KEY_F22
	'\0',	// KEY_F23
	'\0',	// KEY_F24

	'\0',	// KEY_NUMLOCK
	'\0',	// KEY_SCROLL_LOCK

	'\0',	// KEY_LSHIFT
	'\0',	// KEY_RSHIFT
	'\0',	// KEY_LCONTROL
	'\0',	// KEY_RCONTROL
	'\0',	// KEY_LMENU
	'\0',	// KEY_RMENU

	'\0',	// KEY_BROWSER_BACK
	'\0',	// KEY_BROWSER_FORWARD
	'\0',	// KEY_BROWSER_REFRESH
	'\0',	// KEY_BROWSER_STOP

	'\0',	// KEY_BROWSER_SEARCH
	'\0',	// KEY_BROWSER_FAVORITES
	'\0',	// KEY_BROWSER_HOME

	'\0',	// KEY_VOLUME_MUTE
	'\0',	// KEY_VOLUME_DOWN
	'\0',	// KEY_VOLUME_UP
	'\0',	// KEY_MEDIA_NEXT_TRACK
	'\0',	// KEY_MEDIA_PREV_TRACK
	'\0',	// KEY_MEDIA_STOP
	'\0',	// KEY_MEDIA_PLAY_PAUSE
	'\0',	// KEY_LAUNCH_MAIL
	'\0',	// KEY_LAUNCH_MEDIA_SELECT

	'\0',	// KEY_LAUNCH_APP1
	'\0',	// KEY_LAUNCH_APP2

	'=',	// KEY_OEM_PLUS
	',',	// KEY_OEM_COMMA
	'-',	// KEY_OEM_MINUS
	'.',	// KEY_OEM_PERIOD
	'\0',	// KEY_OEM_CLEAR

	';',	// KEY_OEM_1
	'/',	// KEY_OEM_2
	'`',	// KEY_OEM_3
	'[',	// KEY_OEM_4
	'\\',	// KEY_OEM_5
	']',	// KEY_OEM_6
	'\'',	// KEY_OEM_7
	'\0',	// KEY_OEM_8

	'\0',	// KEY_PLAY
	'\0',	// KEY_ZOOM

	'\0',	// KEY_FUNCTION

};

const char ysKeyboard::CHAR_MAP_MOD[] = {
	'\b',	// KEY_BACK
	'\t',	// KEY_TAB

	'\0',	// KEY_CLEAR

	'\n',	// KEY_RETURN
	'\0',	// KEY_SHIFT
	'\0',	// KEY_CONTROL

	'\0',	// KEY_MENU
	'\0',	// KEY_PAUSE
	'\0',	// KEY_CAPSLOCK

	'\0',	// KEY_ESCAPE
	'\0',	// KEY_CONVERT
	'\0',	// KEY_NONCONVERT
	'\0',	// KEY_ACCEPT
	'\0',	// KEY_MODECHANGE

	' ',	// KEY_SPACE
	'\0',	// KEY_PAGEUP
	'\0',	// KEY_PAGEDOWN
	'\0',	// KEY_END
	'\0',	// KEY_HOME

	'\0',	// KEY_LEFT
	'\0',	// KEY_UP
	'\0',	// KEY_RIGHT
	'\0',	// KEY_DOWN

	'\0',	// KEY_SELECT
	'\0',	// KEY_PRINT
	'\0',	// KEY_PRINTSCREEN
	'\0',	// KEY_INSERT
	'\x7F',	// KEY_DELETE
	'\0',	// KEY_HELP

	')',	// KEY_0
	'!',	// KEY_1
	'@',	// KEY_2
	'#',	// KEY_3
	'$',	// KEY_4
	'%',	// KEY_5
	'^',	// KEY_6
	'&',	// KEY_7
	'*',	// KEY_8
	'(',	// KEY_9

	'A',	// KEY_A
	'B',	// KEY_B
	'C',	// KEY_C
	'D',	// KEY_D
	'E',	// KEY_E
	'F',	// KEY_F
	'G',	// KEY_G
	'H',	// KEY_H
	'I',	// KEY_I
	'J',	// KEY_J
	'K',	// KEY_K
	'L',	// KEY_L
	'M',	// KEY_M
	'N',	// KEY_N
	'O',	// KEY_O
	'P',	// KEY_P
	'Q',	// KEY_Q
	'R',	// KEY_R
	'S',	// KEY_S
	'T',	// KEY_T
	'U',	// KEY_U
	'V',	// KEY_V
	'W',	// KEY_W
	'X',	// KEY_X
	'Y',	// KEY_Y
	'Z',	// KEY_Z

	'\0',	// KEY_LEFTWIN
	'\0',	// KEY_RIGHTWIN
	'\0',	// KEY_APPS

	'\0',	// KEY_SLEEP

	'0',	// KEY_NUMPAD0
	'1',	// KEY_NUMPAD1
	'2',	// KEY_NUMPAD2
	'3',	// KEY_NUMPAD3
	'4',	// KEY_NUMPAD4
	'5',	// KEY_NUMPAD5
	'6',	// KEY_NUMPAD6
	'7',	// KEY_NUMPAD7
	'8',	// KEY_NUMPAD8
	'9',	// KEY_NUMPAD9

	'*',	// KEY_MULTIPLY
	'+',	// KEY_ADD
	'\0',	// KEY_SEPARATOR
	'-',	// KEY_SUBTRACT
	'.',	// KEY_DECIMAL
	'/',	// KEY_DIVIDE

	'\0',	// KEY_F1
	'\0',	// KEY_F2
	'\0',	// KEY_F3
	'\0',	// KEY_F4
	'\0',	// KEY_F5
	'\0',	// KEY_F6
	'\0',	// KEY_F7
	'\0',	// KEY_F8
	'\0',	// KEY_F10
	'\0',	// KEY_F11
	'\0',	// KEY_F12
	'\0',	// KEY_F13
	'\0',	// KEY_F14
	'\0',	// KEY_F15
	'\0',	// KEY_F16
	'\0',	// KEY_F17
	'\0',	// KEY_F18
	'\0',	// KEY_F19
	'\0',	// KEY_F20
	'\0',	// KEY_F21
	'\0',	// KEY_F22
	'\0',	// KEY_F23
	'\0',	// KEY_F24

	'\0',	// KEY_NUMLOCK
	'\0',	// KEY_SCROLL_LOCK

	'\0',	// KEY_LSHIFT
	'\0',	// KEY_RSHIFT
	'\0',	// KEY_LCONTROL
	'\0',	// KEY_RCONTROL
	'\0',	// KEY_LMENU
	'\0',	// KEY_RMENU

	'\0',	// KEY_BROWSER_BACK
	'\0',	// KEY_BROWSER_FORWARD
	'\0',	// KEY_BROWSER_REFRESH
	'\0',	// KEY_BROWSER_STOP

	'\0',	// KEY_BROWSER_SEARCH
	'\0',	// KEY_BROWSER_FAVORITES
	'\0',	// KEY_BROWSER_HOME

	'\0',	// KEY_VOLUME_MUTE
	'\0',	// KEY_VOLUME_DOWN
	'\0',	// KEY_VOLUME_UP
	'\0',	// KEY_MEDIA_NEXT_TRACK
	'\0',	// KEY_MEDIA_PREV_TRACK
	'\0',	// KEY_MEDIA_STOP
	'\0',	// KEY_MEDIA_PLAY_PAUSE
	'\0',	// KEY_LAUNCH_MAIL
	'\0',	// KEY_LAUNCH_MEDIA_SELECT

	'\0',	// KEY_LAUNCH_APP1
	'\0',	// KEY_LAUNCH_APP2

	'+',	// KEY_OEM_PLUS
	'<',	// KEY_OEM_COMMA
	'_',	// KEY_OEM_MINUS
	'>',	// KEY_OEM_PERIOD
	'\0',	// KEY_OEM_CLEAR

	':',	// KEY_OEM_1
	'?',	// KEY_OEM_2
	'~',	// KEY_OEM_3
	'{',	// KEY_OEM_4
	'|',	// KEY_OEM_5
	'}',	// KEY_OEM_6
	'\"',	// KEY_OEM_7
	'\0',	// KEY_OEM_8

	'\0',	// KEY_PLAY
	'\0',	// KEY_ZOOM

	'\0',	// KEY_FUNCTION
};

ysKeyboard::ysKeyboard() {
	m_inputBufferOffset = 0;
	m_inputBuffer[0] = '\0';
	m_keyMap = 0;
	m_keys = new ysKey[256];
}

ysKeyboard::~ysKeyboard() {
	delete [] m_keys;
}

bool ysKeyboard::ProcessKeyTransition(KEY_CODE key, ysKey::KEY_STATE state) {
	if (m_keys[key].m_state == state) {
		if (state == ysKey::KEY_DOWN_TRANS) m_keys[key].m_state = ysKey::KEY_DOWN;
		if (state == ysKey::KEY_UP_TRANS) m_keys[key].m_state = ysKey::KEY_UP;

		return true;
	}

	else return false;
}

void ysKeyboard::ClearInputBuffer() {
	m_inputBuffer[0] = '\0';
	m_inputBufferOffset = 0;
}

void ysKeyboard::SetKeyState(KEY_CODE key, ysKey::KEY_STATE state, ysKey::KEY_CONF conf) {
	m_keys[key].m_state = state;
	m_keys[key].m_configuration = conf;

	// Process the input buffer
	if (state == ysKey::KEY_DOWN_TRANS) {
		const char *keyMap = NULL;

		if (GetKey(KEY_SHIFT)->IsDown()) {
			keyMap = CHAR_MAP_MOD;
		}
		else {
			keyMap = CHAR_MAP;
		}

		if (keyMap[key] != '\0') {
			m_inputBuffer[m_inputBufferOffset] = keyMap[key];
			m_inputBuffer[m_inputBufferOffset + 1] = '\0';

			m_inputBufferOffset++;
		}
	}
}
