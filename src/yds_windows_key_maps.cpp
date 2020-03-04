#include "../include/yds_key_maps.h"

#include <Windows.h>

ysKeyboard::KEY_CODE *ysKeyMaps::m_windowsKeyMap;

const ysKeyboard::KEY_CODE *ysKeyMaps::GetWindowsKeyMap() {
    if (m_windowsKeyMap == NULL) {
        m_windowsKeyMap = new ysKeyboard::KEY_CODE[256];
        for (int i = 0; i < 256; i++) m_windowsKeyMap[i] = ysKeyboard::KEY_UNDEFINED;

        m_windowsKeyMap[VK_BACK] = ysKeyboard::KEY_BACK;
        m_windowsKeyMap[VK_TAB] = ysKeyboard::KEY_TAB;

        m_windowsKeyMap[VK_CLEAR] = ysKeyboard::KEY_CLEAR;

        m_windowsKeyMap[VK_RETURN] = ysKeyboard::KEY_RETURN;
        m_windowsKeyMap[VK_SHIFT] = ysKeyboard::KEY_SHIFT;
        m_windowsKeyMap[VK_CONTROL] = ysKeyboard::KEY_CONTROL;

        m_windowsKeyMap[VK_MENU] = ysKeyboard::KEY_MENU;
        m_windowsKeyMap[VK_PAUSE] = ysKeyboard::KEY_PAUSE;
        m_windowsKeyMap[VK_CAPITAL] = ysKeyboard::KEY_CAPSLOCK;

        m_windowsKeyMap[VK_ESCAPE] = ysKeyboard::KEY_ESCAPE;
        m_windowsKeyMap[VK_CONVERT] = ysKeyboard::KEY_CONVERT;
        m_windowsKeyMap[VK_NONCONVERT] = ysKeyboard::KEY_NONCONVERT;
        m_windowsKeyMap[VK_ACCEPT] = ysKeyboard::KEY_ACCEPT;
        m_windowsKeyMap[VK_MODECHANGE] = ysKeyboard::KEY_MODECHANGE;

        m_windowsKeyMap[VK_SPACE] = ysKeyboard::KEY_SPACE;
        m_windowsKeyMap[VK_PRIOR] = ysKeyboard::KEY_PAGEUP;
        m_windowsKeyMap[VK_NEXT] = ysKeyboard::KEY_PAGEDOWN;
        m_windowsKeyMap[VK_END] = ysKeyboard::KEY_END;
        m_windowsKeyMap[VK_HOME] = ysKeyboard::KEY_HOME;

        m_windowsKeyMap[VK_LEFT] = ysKeyboard::KEY_LEFT;
        m_windowsKeyMap[VK_UP] = ysKeyboard::KEY_UP;
        m_windowsKeyMap[VK_RIGHT] = ysKeyboard::KEY_RIGHT;
        m_windowsKeyMap[VK_DOWN] = ysKeyboard::KEY_DOWN;

        m_windowsKeyMap[VK_SELECT] = ysKeyboard::KEY_SELECT;
        m_windowsKeyMap[VK_PRINT] = ysKeyboard::KEY_PRINT;
        m_windowsKeyMap[VK_SNAPSHOT] = ysKeyboard::KEY_PRINTSCREEN;
        m_windowsKeyMap[VK_INSERT] = ysKeyboard::KEY_INSERT;
        m_windowsKeyMap[VK_DELETE] = ysKeyboard::KEY_DELETE;
        m_windowsKeyMap[VK_HELP] = ysKeyboard::KEY_HELP;

        // 0 - 9 keys
        for (int i = 0; i < 10; i++) m_windowsKeyMap[0x30 + i] = (ysKeyboard::KEY_CODE)(ysKeyboard::KEY_0 + i);

        // A - Z keys
        for (int i = 0; i < 26; i++) m_windowsKeyMap[0x41 + i] = (ysKeyboard::KEY_CODE)(ysKeyboard::KEY_A + i);

        m_windowsKeyMap[VK_LWIN] = ysKeyboard::KEY_LEFTWIN;
        m_windowsKeyMap[VK_RWIN] = ysKeyboard::KEY_RIGHTWIN;
        m_windowsKeyMap[VK_APPS] = ysKeyboard::KEY_APPS;

        m_windowsKeyMap[VK_SLEEP] = ysKeyboard::KEY_SLEEP;

        // Numpad keys
        for (int i = 0; i < 10; i++) m_windowsKeyMap[VK_NUMPAD0 + i] = (ysKeyboard::KEY_CODE)(ysKeyboard::KEY_NUMPAD0 + i);

        m_windowsKeyMap[VK_ADD] = ysKeyboard::KEY_ADD;
        m_windowsKeyMap[VK_SEPARATOR] = ysKeyboard::KEY_SEPARATOR;
        m_windowsKeyMap[VK_SUBTRACT] = ysKeyboard::KEY_SUBTRACT;
        m_windowsKeyMap[VK_DECIMAL] = ysKeyboard::KEY_DECIMAL;
        m_windowsKeyMap[VK_DIVIDE] = ysKeyboard::KEY_DIVIDE;
        m_windowsKeyMap[VK_MULTIPLY] = ysKeyboard::KEY_MULTIPLY;

        // F keys
        for (int i = 0; i < 24; i++) m_windowsKeyMap[VK_F1 + i] = (ysKeyboard::KEY_CODE)(ysKeyboard::KEY_F1 + i);

        m_windowsKeyMap[VK_NUMLOCK] = ysKeyboard::KEY_NUMLOCK;
        m_windowsKeyMap[VK_SCROLL] = ysKeyboard::KEY_SCROLL_LOCK;
        m_windowsKeyMap[VK_LSHIFT] = ysKeyboard::KEY_LSHIFT;
        m_windowsKeyMap[VK_RSHIFT] = ysKeyboard::KEY_RSHIFT;
        m_windowsKeyMap[VK_LCONTROL] = ysKeyboard::KEY_LCONTROL;
        m_windowsKeyMap[VK_RCONTROL] = ysKeyboard::KEY_RCONTROL;
        m_windowsKeyMap[VK_LMENU] = ysKeyboard::KEY_LMENU;
        m_windowsKeyMap[VK_RMENU] = ysKeyboard::KEY_RMENU;

        m_windowsKeyMap[VK_BROWSER_BACK] = ysKeyboard::KEY_BROWSER_BACK;
        m_windowsKeyMap[VK_BROWSER_FORWARD] = ysKeyboard::KEY_BROWSER_FORWARD;
        m_windowsKeyMap[VK_BROWSER_REFRESH] = ysKeyboard::KEY_BROWSER_REFRESH;
        m_windowsKeyMap[VK_BROWSER_STOP] = ysKeyboard::KEY_BROWSER_STOP;

        m_windowsKeyMap[VK_BROWSER_SEARCH] = ysKeyboard::KEY_BROWSER_SEARCH;
        m_windowsKeyMap[VK_BROWSER_FAVORITES] = ysKeyboard::KEY_BROWSER_FAVORITES;
        m_windowsKeyMap[VK_BROWSER_HOME] = ysKeyboard::KEY_BROWSER_HOME;

        m_windowsKeyMap[VK_VOLUME_MUTE] = ysKeyboard::KEY_VOLUME_MUTE;
        m_windowsKeyMap[VK_VOLUME_DOWN] = ysKeyboard::KEY_VOLUME_DOWN;
        m_windowsKeyMap[VK_VOLUME_UP] = ysKeyboard::KEY_VOLUME_UP;
        m_windowsKeyMap[VK_MEDIA_NEXT_TRACK] = ysKeyboard::KEY_MEDIA_NEXT_TRACK;
        m_windowsKeyMap[VK_MEDIA_PREV_TRACK] = ysKeyboard::KEY_MEDIA_PREV_TRACK;
        m_windowsKeyMap[VK_MEDIA_STOP] = ysKeyboard::KEY_MEDIA_STOP;
        m_windowsKeyMap[VK_MEDIA_PLAY_PAUSE] = ysKeyboard::KEY_MEDIA_PLAY_PAUSE;
        m_windowsKeyMap[VK_LAUNCH_MAIL] = ysKeyboard::KEY_LAUNCH_MAIL;
        m_windowsKeyMap[VK_LAUNCH_MEDIA_SELECT] = ysKeyboard::KEY_LAUNCH_MEDIA_SELECT;

        m_windowsKeyMap[VK_LAUNCH_APP1] = ysKeyboard::KEY_LAUNCH_APP1;
        m_windowsKeyMap[VK_LAUNCH_APP2] = ysKeyboard::KEY_LAUNCH_APP2;

        m_windowsKeyMap[VK_OEM_PLUS] = ysKeyboard::KEY_OEM_PLUS;
        m_windowsKeyMap[VK_OEM_COMMA] = ysKeyboard::KEY_OEM_COMMA;
        m_windowsKeyMap[VK_OEM_MINUS] = ysKeyboard::KEY_OEM_MINUS;
        m_windowsKeyMap[VK_OEM_PERIOD] = ysKeyboard::KEY_OEM_PERIOD;
        m_windowsKeyMap[VK_OEM_CLEAR] = ysKeyboard::KEY_OEM_CLEAR;

        m_windowsKeyMap[VK_OEM_1] = ysKeyboard::KEY_OEM_1;
        m_windowsKeyMap[VK_OEM_2] = ysKeyboard::KEY_OEM_2;
        m_windowsKeyMap[VK_OEM_3] = ysKeyboard::KEY_OEM_3;
        m_windowsKeyMap[VK_OEM_4] = ysKeyboard::KEY_OEM_4;
        m_windowsKeyMap[VK_OEM_5] = ysKeyboard::KEY_OEM_5;
        m_windowsKeyMap[VK_OEM_6] = ysKeyboard::KEY_OEM_6;
        m_windowsKeyMap[VK_OEM_7] = ysKeyboard::KEY_OEM_7;
        m_windowsKeyMap[VK_OEM_8] = ysKeyboard::KEY_OEM_8;

        m_windowsKeyMap[VK_PLAY] = ysKeyboard::KEY_PLAY;
        m_windowsKeyMap[VK_ZOOM] = ysKeyboard::KEY_ZOOM;

        m_windowsKeyMap[0xff] = ysKeyboard::KEY_FUNCTION;
    }

    return m_windowsKeyMap;
}
