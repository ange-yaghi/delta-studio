#ifndef YDS_AUDIO_DEVICE_H
#define YDS_AUDIO_DEVICE_H

#include "yds_audio_parameters.h"
#include "yds_audio_system_object.h"

#include "yds_window.h"

class ysAudioSource;
class ysAudioBuffer;
class ysAudioSystem;
class ysAudioDeviceProxy;
class ysAudioDevice : public ysAudioSystemObject {
    friend ysAudioSystem;
    friend ysAudioDeviceProxy;

public:
    static constexpr int MaxDeviceNameLength = 256;

public:
    ysAudioDevice();
    ysAudioDevice(API api);
    ~ysAudioDevice();

    inline bool IsConnected() const { return m_connected; }
    inline void SetDeviceName(const wchar_t *newName) {
        wcscpy_s(m_deviceName, MaxDeviceNameLength, newName);
    }
    inline bool Available() { return m_available; }
    inline void SetAvailable(bool available) { m_available = true; }
    inline const wchar_t *GetDeviceName() const { return m_deviceName; }

    virtual ysError CreateSource(const ysAudioParameters *parameters,
                                 SampleOffset size, ysAudioSource **source) = 0;
    virtual ysError CreateSource(ysAudioBuffer *sourceBuffer,
                                 ysAudioSource **source) = 0;

    virtual void UpdateAudioSources() = 0;

    virtual ysError DestroyAudioSource(ysAudioSource *&source);

    inline bool InUse() const { return m_audioSources.GetNumObjects() != 0; }

private:
    void Update();

protected:
    wchar_t m_deviceName[MaxDeviceNameLength];
    ysWindow *m_windowAssociation;

    ysDynamicArray<ysAudioSource, 4> m_audioSources;

    bool m_connected;
    bool m_available;
};

#endif /* YDS_AUDIO_DEVICE_H */
