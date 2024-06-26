#ifndef YDS_DS8_SYSTEM_H
#define YDS_DS8_SYSTEM_H

#include "yds_audio_system.h"
#include "yds_window.h"

class ysDS8Device;

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)

#include <AudioToolbox/AudioToolbox.h>

class ysDS8System : public ysAudioSystem {
public:
    ysDS8System();
    virtual ~ysDS8System();

    virtual ysError EnumerateDevices() override;
    virtual ysError ConnectDevice(ysAudioDevice *device, ysWindow *windowAssociation) override;
    virtual ysError ConnectDeviceConsole(ysAudioDevice *device) override;
    virtual ysError DisconnectDevice(ysAudioDevice *device) override;

protected:
    ysDS8Device *AddDS8Device();

    static OSStatus CoreAudioDeviceProc(AudioDeviceID inDevice, UInt32 inChannel, Boolean isInput, AudioDevicePropertyID inPropertyID, void* inClientData);
    AudioDeviceID m_primaryDeviceID;
    AudioUnit m_outputUnit;
};

#elif defined(_WIN64) /* Windows */

#include <dsound.h>

class ysDS8System : public ysAudioSystem {
public:
    ysDS8System();
    virtual ~ysDS8System();

    virtual ysError EnumerateDevices() override;
    virtual ysError ConnectDevice(ysAudioDevice *device, ysWindow *windowAssociation) override;
    virtual ysError ConnectDeviceConsole(ysAudioDevice *device) override;
    virtual ysError DisconnectDevice(ysAudioDevice *device) override;

protected:
    ysDS8Device *AddDS8Device();

    static BOOL CALLBACK DirectSoundEnumProc(LPGUID lpGUID, LPCTSTR lpszDesc, LPCTSTR lpszDrvName, LPVOID lpContext);
    GUID m_primaryDeviceGuid;
};

#endif /* Windows */

#endif /* YDS_DS8_SYSTEM_H */
