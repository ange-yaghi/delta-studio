#include "../include/yds_animation_mixer.h"

ysAnimationChannel::ysAnimationChannel() {
    Reset();
}

ysAnimationChannel::~ysAnimationChannel() {
    /* void */
}

void ysAnimationChannel::Reset() {
    for (int i = 0; i < SegmentStackSize; ++i) {
        m_segmentStack[i].Action = nullptr;
    }
}

void ysAnimationChannel::Update(float dt) {
    for (int i = 0; i < SegmentStackSize; ++i) {
        if (m_segmentStack[i].Fading) {
            float d = 0 - m_segmentStack[i].FadeOutT0;
            float t = m_segmentStack[i].FadeOutT1 - m_segmentStack[i].FadeOutT0;

            if (t == 0 || d >= t) {
                KillSegment(i);
            }
            else {
                float s = 1.0f - t / d;
                m_segmentStack[i].Amplitude = m_segmentStack[i].FadeStartAmplitude * s;
            }
        }
    }
    
    for (int i = 0; i < SegmentStackSize; ++i) {
        if (m_segmentStack[i].Action != nullptr) {
            m_segmentStack[i].CurrentOffset -= dt;

            if (m_segmentStack[i].Fading) {
                m_segmentStack[i].FadeOutT0 -= dt;
                m_segmentStack[i].FadeOutT1 -= dt;
            }
        }
    }
}

void ysAnimationChannel::AddSegment(ysAnimationAction *action, float fadeIn) {
    int i = m_currentStackPointer;

    m_segmentStack[i].Action = action;
    m_segmentStack[i].FadeIn = fadeIn;
    m_segmentStack[i].Amplitude = 0.0f;
    m_segmentStack[i].CurrentOffset = 0.0f;
    m_segmentStack[i].Fading = false;

    for (int j = 0; j < SegmentStackSize; ++j) {
        if (j != i && m_segmentStack[j].Action != nullptr) {
            m_segmentStack[j].FadeStartAmplitude = m_segmentStack[j].Amplitude;
            m_segmentStack[j].FadeOutT0 = 0.0f;
            m_segmentStack[j].FadeOutT1 = fadeIn;
            m_segmentStack[j].Fading = true;
        }
    }

    ++m_currentStackPointer;
}

void ysAnimationChannel::KillSegment(int segment) {
    m_segmentStack[segment].Action = nullptr;
}

ysAnimationMixer::ysAnimationMixer() {

}

ysAnimationMixer::~ysAnimationMixer() {

}
