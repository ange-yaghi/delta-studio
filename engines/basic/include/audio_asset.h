#ifndef DELTA_BASIC_AUDIO_ASSET_H
#define DELTA_BASIC_AUDIO_ASSET_H

#include "delta_core.h"

#include <string>

namespace dbasic {

    class AudioAsset : public ysObject {
    public:
        AudioAsset();
        virtual ~AudioAsset();

        void SetBuffer(ysAudioBuffer *buffer) { m_buffer = buffer; }
        ysAudioBuffer *GetBuffer() const { return m_buffer; }

        void SetName(const char *name) { m_name = name; }
        std::string GetName() const { return m_name; }

        void SetVolume(float volume) { m_volume = volume; }
        inline float Volume() const { return m_volume; }

    protected:
        float m_volume;
        ysAudioBuffer *m_buffer;
        std::string m_name;
    };

} /* namespace dbasic */

#endif /* DELTA_BASIC_AUDIO_ASSET_H */
