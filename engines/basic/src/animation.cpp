#include "../include/animation.h"

dbasic::Animation::Animation() {
    m_textures = NULL;

    m_mode = PAUSED;
    m_clock = 0.0f;

    m_frame = 0;
    m_nFrames = 0;
}

dbasic::Animation::~Animation() {
    /* void */
}

void dbasic::Animation::SetRate(float rate) {
    m_invRate = 1.0f / rate;
}

void dbasic::Animation::SetMode(ANIMATION_MODE mode) {
    m_mode = mode;
}

ysTexture *dbasic::Animation::GetCurrentFrame() {
    return m_textures[m_frame];
}

void dbasic::Animation::SetFrame(int frame) {
    m_frame = frame;
}

void dbasic::Animation::Update(float timePassed) {
    if (m_mode == PAUSED) return;

    m_clock += timePassed;

    if (m_clock >= m_invRate) {
        int frames = (int)(floor(m_clock / m_invRate) + 0.5);

        m_clock -= m_invRate * frames;

        m_frame += frames;

        if (m_frame >= m_nFrames) {
            if (m_mode == PLAY) {
                m_frame = m_nFrames - 1;
            }
            else if (m_mode == LOOP) {
                m_frame = m_frame % m_nFrames;
            }
        }

        if (m_frame < 0) {
            m_frame = 0;
        }
    }
}
