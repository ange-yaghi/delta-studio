#ifndef YDS_KEY_MAPS_H
#define YDS_KEY_MAPS_H

#include "yds_keyboard.h"

class ysKeyMaps {
public:
	ysKeyMaps() { m_windowsKeyMap = nullptr; }
	~ysKeyMaps() {}

	static const ysKeyboard::KEY_CODE *GetWindowsKeyMap();

protected:
	static ysKeyboard::KEY_CODE *m_windowsKeyMap;
};

#endif /* YDS_KEY_MAPS_H */
