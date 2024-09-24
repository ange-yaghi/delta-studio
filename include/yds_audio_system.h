#ifndef YDS_AUDIO_SYSTEM_H
#define YDS_AUDIO_SYSTEM_H

#include "yds_audio_system_object.h"
#include "yds_window.h"
#include "yds_audio_parameters.h"

class ysAudioDevice;
class ysAudioBuffer;
class ysAudioSystem : public ysAudioSystemObject {
protected:
    ysAudioSystem();
    ysAudioSystem(API api);
    virtual ~ysAudioSystem();

public:
    static ysError CreateAudioSystem(ysAudioSystem **newAudioSystem, API api);
    static ysError DestroyAudioSystem(ysAudioSystem **audioSystem);

    virtual ysError EnumerateDevices();
    virtual ysError ConnectDevice(ysAudioDevice *device,
                                  ysWindow *windowAssociation);
    virtual ysError ConnectDevice(ysWindow *windowAssociation,
                                  ysAudioDevice **device);
    virtual ysError ConnectDeviceConsole(ysAudioDevice *device);
    virtual ysError DisconnectDevice(ysAudioDevice *device);

    virtual ysError CreateBuffer(const ysAudioParameters *parameters,
                                 SampleOffset size, ysAudioBuffer **buffer) = 0;
    ysError DestroyAudioBuffers();
    virtual ysError DestroyAudioBuffer(ysAudioBuffer *&buffer);

    ysError UpdateDeviceStates();

    ysAudioDevice *GetPrimaryDevice();
    ysAudioDevice *GetDevice(int device);
    inline int GetDeviceCount() const { return m_devices.GetNumObjects(); }

protected:
    ysDynamicArray<ysAudioBuffer, 4> m_audioBuffers;
    ysDynamicArray<ysAudioDevice, 4> m_devices;
    ysAudioDevice *m_primaryDevice;
};

#endif /* YDS_AUDIO_SYSTEM_H */
