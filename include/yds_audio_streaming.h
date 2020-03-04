#ifndef YDS_AUDIO_STREAMING_H
#define YDS_AUDIO_STREAMING_H

#include "yds_base.h"
#include "yds_error_codes.h"
#include "yds_audio_parameters.h"

class ysAudioFile;
class ysAudioBuffer;

class ysStreamingAudio : public ysObject {
public:
    ysStreamingAudio();
    ~ysStreamingAudio();

    ysError InitializeBuffer();
    ysError Update();

    ysError AttachAudioFile(ysAudioFile *file);
    ysError AttachAudioBuffer(ysAudioBuffer *buffer);

    ysError Seek(SampleOffset offset);

protected:
    ysError UpdateMetrics();

    void Transfer();

    int m_subdivisions;
    int m_currentWriteSubdivision;
    int m_currentReadSubdivision;

    ysAudioFile *m_file;
    ysAudioBuffer *m_buffer;

    SampleOffset m_fileOffset;
};

#endif /* YDS_AUDIO_STREAMING_H */
