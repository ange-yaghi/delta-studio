#include "../include/audio_asset.h"

dbasic::AudioAsset::AudioAsset() {
    m_buffer = nullptr;
    m_volume = 1.0f;
}

dbasic::AudioAsset::~AudioAsset() {
    /* void */
}
