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
    void SetLogicalSize(int w, int h);
    void SetPhysicalSize(int w, int h);
    void SetDeviceName(const wchar_t *name);

    int GetPhysicalWidth() const { return m_physicalWidth; }
    int GetPhysicalHeight() const { return m_physicalHeight; }

    int GetLogicalWidth() const { return m_logicalWidth; }
    int GetLogicalHeight() const { return m_logicalHeight; }

    void CalculateScaling();

    void PhysicalToLogical(int px, int py, int &lx, int &ly) const;
    void LogicalToPhysical(int lx, int ly, int &px, int &py) const;

    int GetOriginX() const { return m_originx; }
    int GetOriginY() const { return m_originy; }

    const wchar_t *GetDeviceName() const { return m_deviceName; }
    bool IsConnected() const { return m_connected; }
    void SetConnected(bool connected) { m_connected = connected; }

protected:
    wchar_t *m_deviceName;
    int m_maxDeviceNameLength;

    int m_originx;
    int m_originy;

    int m_physicalWidth;
    int m_physicalHeight;

    int m_logicalWidth;
    int m_logicalHeight;

    float m_horizontalScaling;
    float m_verticalScaling;

    bool m_connected;
};

#endif /* YDS_MONITOR_H */
