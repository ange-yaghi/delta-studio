#include "../include/yds_sdl_audio_system.h"

#include "../include/yds_sdl_audio_device.h"

#include <SDL2/SDL.h>

ysSdlAudioSystem::ysSdlAudioSystem() : ysAudioSystem(API::Sdl) {
    // Make sure that the audio system is up
    SDL_Init(SDL_INIT_AUDIO);
}

ysSdlAudioSystem::~ysSdlAudioSystem() {
    /* void */
}

void ysSdlAudioSystem::EnumerateDevices() {
    ysAudioSystem::EnumerateDevices();

    // The output format we want
    SDL_AudioSpec desired{};
    desired.freq = 44100;
    desired.format = AUDIO_S16;
    desired.channels = 1; // TODO: multi-channel support
    desired.samples = 4096;

    // TODO: use callback-based handling for more accurate latency info

    auto CreateDevice = [&](const char *name) {
        // Try and open the device
        SDL_AudioSpec obtained;
        SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(name, 0, &desired, &obtained, 0);
        if (deviceId == 0) return;

        // Create a new device to track it
        auto *primaryDevice = m_devices.NewGeneric<ysSdlAudioDevice>();
        primaryDevice->m_deviceId = deviceId;
        primaryDevice->m_outputFormat = obtained;
    };

    // SDL_GetNumAudioDevices() detects devices under the hood
    const int numDevices = SDL_GetNumAudioDevices(0);

    // Add our primary device
    CreateDevice(nullptr);

    // We could iterate over all available devices, however we only need
    // the primary one at the moment
#if 0
    for (int i = 0; i < numDevices; i++) {
        // Try and open the device
        const char *name = SDL_GetAudioDeviceName(i, 0);
        CreateDevice(name);
    }
#endif
}

void ysSdlAudioSystem::ConnectDevice(ysAudioDevice *device, ysWindow *windowAssociation) {
    ysAudioSystem::ConnectDevice(device, windowAssociation);
}

void ysSdlAudioSystem::ConnectDeviceConsole(ysAudioDevice *device) {
    ysAudioSystem::ConnectDeviceConsole(device);
}

void ysSdlAudioSystem::DisconnectDevice(ysAudioDevice *device) {
    ysAudioSystem::DisconnectDevice(device);
}
