#ifndef YDS_SDL_AUDIO_WAVE_FILE_H
#define YDS_SDL_AUDIO_WAVE_FILE_H

#include "yds_audio_file.h"

class ysSdlAudioWaveFile : public ysAudioFile {
public:
    ysSdlAudioWaveFile();
    ~ysSdlAudioWaveFile();

    virtual Error OpenFile(const char *fname) override;
    virtual Error CloseFile() override;

protected:
    virtual Error GenericRead(SampleOffset offset, SampleOffset size, void *buffer) override;

    uint32_t m_dataLength;
    uint8_t *m_data;
};

#endif /* YDS_SDL_AUDIO_WAVE_FILE_H */
