#include "../include/yds_key_maps.h"

#include <Windows.h>

ysKey::Code *ysKeyMaps::m_windowsKeyMap;

const ysKey::Code *ysKeyMaps::GetWindowsKeyMap() {
    if (m_windowsKeyMap == NULL) {
        m_windowsKeyMap = new ysKey::Code[256];
        for (int i = 0; i < 256; ++i) m_windowsKeyMap[i] = ysKey::Code::Undefined;

        m_windowsKeyMap[VK_BACK] = ysKey::Code::Back;
        m_windowsKeyMap[VK_TAB] = ysKey::Code::Tab;

        m_windowsKeyMap[VK_CLEAR] = ysKey::Code::Clear;

        m_windowsKeyMap[VK_RETURN] = ysKey::Code::Return;
        m_windowsKeyMap[VK_SHIFT] = ysKey::Code::Shift;
        m_windowsKeyMap[VK_CONTROL] = ysKey::Code::Control;

        m_windowsKeyMap[VK_MENU] = ysKey::Code::Menu;
        m_windowsKeyMap[VK_PAUSE] = ysKey::Code::Pause;
        m_windowsKeyMap[VK_CAPITAL] = ysKey::Code::CapsLock;

        m_windowsKeyMap[VK_ESCAPE] = ysKey::Code::Escape;
        m_windowsKeyMap[VK_CONVERT] = ysKey::Code::Convert;
        m_windowsKeyMap[VK_NONCONVERT] = ysKey::Code::NonConvert;
        m_windowsKeyMap[VK_ACCEPT] = ysKey::Code::Accept;
        m_windowsKeyMap[VK_MODECHANGE] = ysKey::Code::ModeChange;

        m_windowsKeyMap[VK_SPACE] = ysKey::Code::Space;
        m_windowsKeyMap[VK_PRIOR] = ysKey::Code::PageUp;
        m_windowsKeyMap[VK_NEXT] = ysKey::Code::PageDown;
        m_windowsKeyMap[VK_END] = ysKey::Code::End;
        m_windowsKeyMap[VK_HOME] = ysKey::Code::Home;

        m_windowsKeyMap[VK_LEFT] = ysKey::Code::Left;
        m_windowsKeyMap[VK_UP] = ysKey::Code::Up;
        m_windowsKeyMap[VK_RIGHT] = ysKey::Code::Right;
        m_windowsKeyMap[VK_DOWN] = ysKey::Code::Down;

        m_windowsKeyMap[VK_SELECT] = ysKey::Code::Select;
        m_windowsKeyMap[VK_PRINT] = ysKey::Code::Print;
        m_windowsKeyMap[VK_SNAPSHOT] = ysKey::Code::PrintScreen;
        m_windowsKeyMap[VK_INSERT] = ysKey::Code::Insert;
        m_windowsKeyMap[VK_DELETE] = ysKey::Code::Delete;
        m_windowsKeyMap[VK_HELP] = ysKey::Code::Help;

        // 0 - 9 keys
        for (int i = 0; i < 10; ++i) m_windowsKeyMap[0x30 + i] = (ysKey::Code)((int)ysKey::Code::N0 + i);

        // A - Z keys
        for (int i = 0; i < 26; ++i) m_windowsKeyMap[0x41 + i] = (ysKey::Code)((int)ysKey::Code::A + i);

        m_windowsKeyMap[VK_LWIN] = ysKey::Code::LeftWin;
        m_windowsKeyMap[VK_RWIN] = ysKey::Code::RightWin;
        m_windowsKeyMap[VK_APPS] = ysKey::Code::Apps;

        m_windowsKeyMap[VK_SLEEP] = ysKey::Code::Sleep;

        // Numpad keys
        for (int i = 0; i < 10; ++i) m_windowsKeyMap[VK_NUMPAD0 + i] = (ysKey::Code)((int)ysKey::Code::Numpad0 + i);

        m_windowsKeyMap[VK_ADD] = ysKey::Code::Add;
        m_windowsKeyMap[VK_SEPARATOR] = ysKey::Code::Separator;
        m_windowsKeyMap[VK_SUBTRACT] = ysKey::Code::Subtract;
        m_windowsKeyMap[VK_DECIMAL] = ysKey::Code::Decimal;
        m_windowsKeyMap[VK_DIVIDE] = ysKey::Code::Divide;
        m_windowsKeyMap[VK_MULTIPLY] = ysKey::Code::Multiply;

        // F keys
        for (int i = 0; i < 24; ++i) m_windowsKeyMap[VK_F1 + i] = (ysKey::Code)((int)ysKey::Code::F1 + i);

        m_windowsKeyMap[VK_NUMLOCK] = ysKey::Code::NumLock;
        m_windowsKeyMap[VK_SCROLL] = ysKey::Code::ScrollLock;
        m_windowsKeyMap[VK_LSHIFT] = ysKey::Code::LeftShift;
        m_windowsKeyMap[VK_RSHIFT] = ysKey::Code::RightShift;
        m_windowsKeyMap[VK_LCONTROL] = ysKey::Code::LeftControl;
        m_windowsKeyMap[VK_RCONTROL] = ysKey::Code::RightControl;
        m_windowsKeyMap[VK_LMENU] = ysKey::Code::LeftMenu;
        m_windowsKeyMap[VK_RMENU] = ysKey::Code::RightMenu;

        m_windowsKeyMap[VK_BROWSER_BACK] = ysKey::Code::BrowserBack;
        m_windowsKeyMap[VK_BROWSER_FORWARD] = ysKey::Code::BrowserForward;
        m_windowsKeyMap[VK_BROWSER_REFRESH] = ysKey::Code::BrowserRefresh;
        m_windowsKeyMap[VK_BROWSER_STOP] = ysKey::Code::BrowserStop;

        m_windowsKeyMap[VK_BROWSER_SEARCH] = ysKey::Code::BrowserSearch;
        m_windowsKeyMap[VK_BROWSER_FAVORITES] = ysKey::Code::BrowserFavorites;
        m_windowsKeyMap[VK_BROWSER_HOME] = ysKey::Code::BrowserHome;

        m_windowsKeyMap[VK_VOLUME_MUTE] = ysKey::Code::VolumeMute;
        m_windowsKeyMap[VK_VOLUME_DOWN] = ysKey::Code::VolumeDown;
        m_windowsKeyMap[VK_VOLUME_UP] = ysKey::Code::VolumeUp;
        m_windowsKeyMap[VK_MEDIA_NEXT_TRACK] = ysKey::Code::MediaNextTrack;
        m_windowsKeyMap[VK_MEDIA_PREV_TRACK] = ysKey::Code::MediaPrevTrack;
        m_windowsKeyMap[VK_MEDIA_STOP] = ysKey::Code::MediaStop;
        m_windowsKeyMap[VK_MEDIA_PLAY_PAUSE] = ysKey::Code::MediaPlay;
        m_windowsKeyMap[VK_LAUNCH_MAIL] = ysKey::Code::LaunchMail;
        m_windowsKeyMap[VK_LAUNCH_MEDIA_SELECT] = ysKey::Code::LaunchMediaSelect;

        m_windowsKeyMap[VK_LAUNCH_APP1] = ysKey::Code::LaunchApp1;
        m_windowsKeyMap[VK_LAUNCH_APP2] = ysKey::Code::LaunchApp2;

        m_windowsKeyMap[VK_OEM_PLUS] = ysKey::Code::OEM_Plus;
        m_windowsKeyMap[VK_OEM_COMMA] = ysKey::Code::OEM_Comma;
        m_windowsKeyMap[VK_OEM_MINUS] = ysKey::Code::OEM_Minus;
        m_windowsKeyMap[VK_OEM_PERIOD] = ysKey::Code::OEM_Period;
        m_windowsKeyMap[VK_OEM_CLEAR] = ysKey::Code::OEM_Clear;

        m_windowsKeyMap[VK_OEM_1] = ysKey::Code::OEM_1;
        m_windowsKeyMap[VK_OEM_2] = ysKey::Code::OEM_2;
        m_windowsKeyMap[VK_OEM_3] = ysKey::Code::OEM_3;
        m_windowsKeyMap[VK_OEM_4] = ysKey::Code::OEM_4;
        m_windowsKeyMap[VK_OEM_5] = ysKey::Code::OEM_5;
        m_windowsKeyMap[VK_OEM_6] = ysKey::Code::OEM_6;
        m_windowsKeyMap[VK_OEM_7] = ysKey::Code::OEM_7;
        m_windowsKeyMap[VK_OEM_8] = ysKey::Code::OEM_8;

        m_windowsKeyMap[VK_PLAY] = ysKey::Code::Play;
        m_windowsKeyMap[VK_ZOOM] = ysKey::Code::Zoom;

        m_windowsKeyMap[0xff] = ysKey::Code::Function;
    }

    return m_windowsKeyMap;
}
