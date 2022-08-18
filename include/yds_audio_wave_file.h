#ifndef YDS_AUDIO_WAVE_FILE_H
#define YDS_AUDIO_WAVE_FILE_H

// This header facilitates x-platform WAV reading by
// declaring a ysAudioWaveFile that will delegate to
// the correct backend depending on platform.

#ifdef _WIN32
#include "yds_windows_audio_wave_file.h"
using ysAudioWaveFile = ysWindowsAudioWaveFile;
#else
#include "yds_sdl_audio_wave_file.h"
using ysAudioWaveFile = ysSdlAudioWaveFile;
#endif

#endif /* YDS_AUDIO_WAVE_FILE_H */
