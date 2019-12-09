#ifndef YDS_KEYBOARD_H
#define YDS_KEYBOARD_H

#include "yds_window_system_object.h"

class ysKey {
public:
    ysKey();
    ~ysKey();

    enum KEY_STATE {
        KEY_UNDEFINED,

        KEY_UP,
        KEY_DOWN,

        KEY_UP_TRANS,
        KEY_DOWN_TRANS
    };

    enum KEY_CONF {
        KEY_CONF_UNDEFINED,

        KEY_CONF_RIGHT,
        KEY_CONF_LEFT
    };

    KEY_STATE	m_state;
    KEY_CONF	m_configuration;

    void Reset();

    bool IsDown() const { return m_state == KEY_DOWN || m_state == KEY_DOWN_TRANS; }
};

class ysKeyboard {
    static const char CHAR_MAP[];
    static const char CHAR_MAP_MOD[];

public:
    enum KEY_CODE {
        KEY_BACK,
        KEY_TAB,

        KEY_CLEAR,

        KEY_RETURN,
        KEY_SHIFT,
        KEY_CONTROL,

        KEY_MENU,
        KEY_PAUSE,
        KEY_CAPSLOCK,

        KEY_ESCAPE,
        KEY_CONVERT,
        KEY_NONCONVERT,
        KEY_ACCEPT,
        KEY_MODECHANGE,

        KEY_SPACE,
        KEY_PAGEUP,
        KEY_PAGEDOWN,
        KEY_END,
        KEY_HOME,

        KEY_LEFT,
        KEY_UP,
        KEY_RIGHT,
        KEY_DOWN,

        KEY_SELECT,
        KEY_PRINT,
        KEY_PRINTSCREEN,
        KEY_INSERT,
        KEY_DELETE,
        KEY_HELP,

        KEY_0,
        KEY_1,
        KEY_2,
        KEY_3,
        KEY_4,
        KEY_5,
        KEY_6,
        KEY_7,
        KEY_8,
        KEY_9,

        KEY_A,
        KEY_B,
        KEY_C,
        KEY_D,
        KEY_E,
        KEY_F,
        KEY_G,
        KEY_H,
        KEY_I,
        KEY_J,
        KEY_K,
        KEY_L,
        KEY_M,
        KEY_N,
        KEY_O,
        KEY_P,
        KEY_Q,
        KEY_R,
        KEY_S,
        KEY_T,
        KEY_U,
        KEY_V,
        KEY_W,
        KEY_X,
        KEY_Y,
        KEY_Z,

        KEY_LEFTWIN,
        KEY_RIGHTWIN,
        KEY_APPS,

        KEY_SLEEP,

        KEY_NUMPAD0,
        KEY_NUMPAD1,
        KEY_NUMPAD2,
        KEY_NUMPAD3,
        KEY_NUMPAD4,
        KEY_NUMPAD5,
        KEY_NUMPAD6,
        KEY_NUMPAD7,
        KEY_NUMPAD8,
        KEY_NUMPAD9,

        KEY_MULTIPLY,
        KEY_ADD,
        KEY_SEPARATOR,
        KEY_SUBTRACT,
        KEY_DECIMAL,
        KEY_DIVIDE,

        KEY_F1,
        KEY_F2,
        KEY_F3,
        KEY_F4,
        KEY_F5,
        KEY_F6,
        KEY_F7,
        KEY_F8,
        KEY_F10,
        KEY_F11,
        KEY_F12,
        KEY_F13,
        KEY_F14,
        KEY_F15,
        KEY_F16,
        KEY_F17,
        KEY_F18,
        KEY_F19,
        KEY_F20,
        KEY_F21,
        KEY_F22,
        KEY_F23,
        KEY_F24,

        KEY_NUMLOCK,
        KEY_SCROLL_LOCK,

        KEY_LSHIFT,
        KEY_RSHIFT,
        KEY_LCONTROL,
        KEY_RCONTROL,
        KEY_LMENU,
        KEY_RMENU,

        KEY_BROWSER_BACK,
        KEY_BROWSER_FORWARD,
        KEY_BROWSER_REFRESH,
        KEY_BROWSER_STOP,

        KEY_BROWSER_SEARCH,
        KEY_BROWSER_FAVORITES,
        KEY_BROWSER_HOME,

        KEY_VOLUME_MUTE,
        KEY_VOLUME_DOWN,
        KEY_VOLUME_UP,
        KEY_MEDIA_NEXT_TRACK,
        KEY_MEDIA_PREV_TRACK,
        KEY_MEDIA_STOP,
        KEY_MEDIA_PLAY_PAUSE,
        KEY_LAUNCH_MAIL,
        KEY_LAUNCH_MEDIA_SELECT,

        KEY_LAUNCH_APP1,
        KEY_LAUNCH_APP2,

        KEY_OEM_PLUS,
        KEY_OEM_COMMA,
        KEY_OEM_MINUS,
        KEY_OEM_PERIOD,
        KEY_OEM_CLEAR,

        KEY_OEM_1,
        KEY_OEM_2,
        KEY_OEM_3,
        KEY_OEM_4,
        KEY_OEM_5,
        KEY_OEM_6,
        KEY_OEM_7,
        KEY_OEM_8,

        KEY_PLAY,
        KEY_ZOOM,

        KEY_FUNCTION,

        KEY_UNDEFINED,
        KEY_COUNT = KEY_UNDEFINED
    };

public:
    ysKeyboard();
    ~ysKeyboard();

    void RegisterKeyMap(const KEY_CODE *map) { m_keyMap = map; }
    inline KEY_CODE GetKeyMap(int sysCode) { return m_keyMap[sysCode]; }

    bool ProcessKeyTransition(KEY_CODE key, ysKey::KEY_STATE state = ysKey::KEY_DOWN_TRANS);

    void SetKeyState(KEY_CODE key, ysKey::KEY_STATE state, ysKey::KEY_CONF conf);
    const ysKey *GetKey(KEY_CODE key) const { return &m_keys[key]; }

    const char *GetInputBuffer() const { return m_inputBuffer; }
    void ClearInputBuffer();

protected:
    const KEY_CODE *m_keyMap;
    ysKey *m_keys;

    char m_inputBuffer[256];
    int m_inputBufferOffset;
};

#endif /* YDS_KEYBOARD_H */
