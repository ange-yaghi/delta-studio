#ifndef KEY_MAPS_H
#define KEY_MAPS_H

#include "yds_keyboard.h"

class ysKeyMaps
{

public:

	ysKeyMaps() { m_windowsKeyMap = 0; }
	~ysKeyMaps() {}

	static const ysKeyboard::KEY_CODE *GetWindowsKeyMap();

protected:

	static ysKeyboard::KEY_CODE *m_windowsKeyMap;

};

#endif