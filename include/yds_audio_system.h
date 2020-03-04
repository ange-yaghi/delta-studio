#ifndef YDS_AUDIO_SYSTEM_H
#define YDS_AUDIO_SYSTEM_H

#include "yds_audio_system_object.h"
#include "yds_window.h"

class ysAudioDevice;

class ysAudioSystem : public ysAudioSystemObject {
protected:
    ysAudioSystem();
    ysAudioSystem(AUDIO_SYSTEM_API API);
    virtual ~ysAudioSystem();

public:
    static ysError CreateAudioSystem(ysAudioSystem **newAudioSystem, AUDIO_SYSTEM_API API);

    virtual void EnumerateDevices();
    virtual void ConnectDevice(ysAudioDevice *device, ysWindow *windowAssociation);
    virtual void ConnectDeviceConsole(ysAudioDevice *device);
    virtual void DisconnectDevice(ysAudioDevice *device);

    ysAudioDevice *GetPrimaryDevice();
    ysAudioDevice *GetAuxDevice(int device);
    int GetDeviceCount() const { return m_devices.GetNumObjects(); }

protected:
    ysDynamicArray<ysAudioDevice, 4> m_devices;
};

#endif /* YDS_AUDIO_SYSTEM_H */
