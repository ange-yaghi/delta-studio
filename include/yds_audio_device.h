#ifndef YDS_AUDIO_DEVICE_H
#define YDS_AUDIO_DEVICE_H

#include "yds_audio_system_object.h"
#include "yds_audio_parameters.h"

#include "yds_window.h"

class ysAudioBuffer;

class ysAudioDevice : public ysAudioSystemObject {
    friend class ysAudioSystem;

public:
    ysAudioDevice();
    ysAudioDevice(AUDIO_SYSTEM_API API);
    ~ysAudioDevice();

    bool IsConnected() const { return m_connected; }
    void SetDeviceName(const char *newName) { strcpy_s(m_deviceName, 256, newName); }

    // TEMP: Will need to be more detailed
    virtual ysAudioBuffer *CreateBuffer(const ysAudioParameters *parameters, SampleOffset size) = 0;
    virtual ysAudioBuffer *CreateDuplicate(ysAudioBuffer *buffer) = 0;

    virtual void ProcessAudioBuffers() = 0;

    virtual ysError DestroyAudioBuffer(ysAudioBuffer *&buffer);
    void DestroyAudioBuffers();

protected:
    char m_deviceName[256];
    ysWindow *m_windowAssociation;

    ysDynamicArray<ysAudioBuffer, 4> m_audioBuffers;

    bool m_connected;
};

#endif /* YDS_AUDIO_DEVICE_H */
