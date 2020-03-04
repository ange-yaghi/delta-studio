#ifndef YDS_MONITOR_H
#define YDS_MONITOR_H

#include "yds_window_system_object.h"

class ysMonitor : public ysWindowSystemObject {
public:
    ysMonitor();
    ysMonitor(Platform platform);
    virtual ~ysMonitor();

    void InitializeDeviceName(int maxLength);

    void SetOrigin(int x, int y);
    void SetSize(int w, int h);
    void SetDeviceName(const char *name);

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    int GetOriginX() const { return m_originx; }
    int GetOriginY() const { return m_originy; }

    const char *GetDeviceName() const { return m_deviceName; }

protected:
    char *m_deviceName;
    int m_maxDeviceNameLength;

    int m_originx;
    int m_originy;

    int m_width;
    int m_height;
};

#endif /* YDS_MONITOR_H */
