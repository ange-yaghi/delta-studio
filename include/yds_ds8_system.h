#ifndef YDS_DS8_SYSTEM_H
#define YDS_DS8_SYSTEM_H

#include "yds_audio_system.h"
#include "yds_window.h"

#include <dsound.h>

class ysDS8Device;

class ysDS8System : public ysAudioSystem {
public:
    ysDS8System();
    virtual ~ysDS8System();

    virtual ysError EnumerateDevices() override;
    virtual ysError ConnectDevice(ysAudioDevice *device,
                                  ysWindow *windowAssociation) override;
    virtual ysError ConnectDeviceConsole(ysAudioDevice *device) override;
    virtual ysError DisconnectDevice(ysAudioDevice *device) override;
    virtual ysError CreateBuffer(const ysAudioParameters *parameters,
                                 SampleOffset size, ysAudioBuffer **buffer) override;

protected:
    ysDS8Device *DeviceById(GUID deviceGuid);
    ysDS8Device *AddDS8Device();

    static BOOL CALLBACK DirectSoundEnumProc(LPGUID lpGUID, LPCTSTR lpszDesc,
                                             LPCTSTR lpszDrvName,
                                             LPVOID lpContext);
    GUID m_primaryDeviceGuid;
};

#endif /* YDS_DS8_SYSTEM_H */
