#ifndef YDS_WINDOWS_AUDIO_WAVE_FILE_H
#define YDS_WINDOWS_AUDIO_WAVE_FILE_H

#include "yds_audio_file.h"

#define NOMINMAX

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)

//#include "win32/windows_modular.h"

// TODO: - 

#elif defined(_WIN64)

#include <Windows.h>

class ysWindowsAudioWaveFile : public ysAudioFile {
public:
    ysWindowsAudioWaveFile();
    ~ysWindowsAudioWaveFile();

    virtual Error OpenFile(const wchar_t *fname) override;
    virtual Error CloseFile();

protected:
    virtual Error GenericRead(SampleOffset offset, SampleOffset size, void *buffer);

    HMMIO m_fileHandle;
    unsigned int m_dataSegmentOffset;
};

#endif /* Windows */

#endif /* YDS_WINDOWS_AUDIO_WAVE_FILE_H */
