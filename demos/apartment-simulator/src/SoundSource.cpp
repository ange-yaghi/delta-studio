#include "../include/SoundSource.h"

#include <math.h>
#include <algorithm>

SoundSource::SoundSource() {
    m_buffer = nullptr;
    m_volume = 0.0f;
    m_falloffDistance = 0.0f;
    m_falloffStart = 0.0f;

    m_falloff = false;
}

SoundSource::~SoundSource() {
    /* void */
}

void SoundSource::Update(ysVector &cameraPosition, ysVector &cameraDirection, ysVector &cameraUp) {
    ysVector delta = ysMath::Sub(cameraPosition, m_position);
    ysVector dir = ysMath::Normalize(delta);

    ysVector right = ysMath::Cross(cameraDirection, cameraUp);
    ysVector left = ysMath::Negate(right);

    ysVector frontAmount = ysMath::Dot(dir, cameraDirection);
    ysVector rightAmount = ysMath::Dot(dir, right);
    ysVector leftAmount = ysMath::Dot(dir, left);

    ysVector distance = ysMath::Magnitude(delta);

    float d_s = ysMath::GetScalar(distance);
    float f_s = -ysMath::GetScalar(frontAmount);

    float dot_r = ysMath::GetScalar(rightAmount);
    float r_s = ((dot_r + 1) / 2.0f) * 0.1f + 0.9f;
    float l_s = ((-dot_r + 1) / 2.0f) * 0.1f + 0.9f;

    float pan = (r_s * 1.0f + l_s * -1.0f) / (r_s + l_s);
    float volume = m_volume;

    // Front/back
    float f_weight = (f_s + 1.0f) / 2.0f;
    volume *= (f_weight * 1.0f + (1 - f_weight) * 0.5f);

    if (m_falloff) {
        float falloff = 1 / (powf(2, std::max(d_s - m_falloffStart, 0.0f) / m_falloffDistance));
        volume *= falloff;
    }

    m_buffer->SetVolume(volume);
    m_buffer->SetPan(pan);
}
