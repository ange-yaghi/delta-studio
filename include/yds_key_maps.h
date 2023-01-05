#ifndef YDS_KEY_MAPS_H
#define YDS_KEY_MAPS_H

#include "yds_keyboard.h"

class ysKeyMaps {
public:
    ysKeyMaps() {}
    ~ysKeyMaps() {}

    static const ysKey::Code *GetWindowsKeyMap();
    static const ysKey::Code *GetSdlKeyMap();

protected:
    static ysKey::Code *m_windowsKeyMap;
    static ysKey::Code *m_sdlKeyMap;
};

#endif /* YDS_KEY_MAPS_H */
