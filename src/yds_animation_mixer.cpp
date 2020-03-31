#include "../include/yds_animation_mixer.h"

#include "../include/yds_animation_action.h"
#include "../include/yds_animation_action_binding.h"

ysAnimationChannel::ysAnimationChannel() {
    m_amplitude = 1.0f;

    Reset();
}

ysAnimationChannel::~ysAnimationChannel() {
    /* void */
}

void ysAnimationChannel::Reset() {
    m_currentStackPointer = 0;

    for (int i = 0; i < SegmentStackSize; ++i) {
        m_segmentStack[i].Action = nullptr;
    }
}

void ysAnimationChannel::Sample() {
    // Update segment that is fading in first
    for (int i = 0; i < SegmentStackSize; ++i) {
        if (m_segmentStack[i].Action != nullptr) {
            if (m_segmentStack[i].Fading) {
                if (m_segmentStack[i].FadeOutT0 <= 0) {
                    continue;
                }
            }

            float d = 0 - m_segmentStack[i].CurrentOffset;
            float t = m_segmentStack[i].FadeIn;

            if (d < 0) continue;

            if (d < t) {
                m_segmentStack[i].Amplitude = (d / t);
            }
            else {
                m_segmentStack[i].Amplitude = 1.0f;
            }
        }
    }

    // Update segments that are fading out
    for (int i = 0; i < SegmentStackSize; ++i) {
        if (m_segmentStack[i].Action != nullptr && m_segmentStack[i].Fading) {
            float d = 0 - m_segmentStack[i].FadeOutT0;
            float t = m_segmentStack[i].FadeOutT1 - m_segmentStack[i].FadeOutT0;

            if (d < 0) continue;

            if (t == 0 || d >= t) {
                KillSegment(i);
            }
            else if (d > 0) {
                float s = 1.0f - d / t;
                m_segmentStack[i].Amplitude = m_segmentStack[i].FadeStartAmplitude * s;
            }
        }
    }

    for (int i = 0; i < SegmentStackSize; ++i) {
        Segment &segment = m_segmentStack[i];
        if (segment.Action != nullptr) {
            float s = 0 - segment.CurrentOffset;

            if (s >= 0) {
                segment.Action->Sample(s, segment.Amplitude);
            }
        }
    }
}

void ysAnimationChannel::Advance(float dt) {
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

void ysAnimationChannel::AddSegment(ysAnimationActionBinding *action, float fadeIn) {
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

    IncrementStackPointer();
}

void ysAnimationChannel::AddSegmentAtEnd(ysAnimationActionBinding *action, float fadeIn) {
    int i = m_currentStackPointer;
    int last = GetPrevious();

    float offset = 0.0f;
    if (last != -1) {
        offset = m_segmentStack[last].Action->GetAction()->GetLength() + m_segmentStack[last].CurrentOffset;
    }

    m_segmentStack[i].Action = action;
    m_segmentStack[i].FadeIn = fadeIn;
    m_segmentStack[i].Amplitude = 0.0f;
    m_segmentStack[i].CurrentOffset = offset;
    m_segmentStack[i].Fading = false;

    for (int j = 0; j < SegmentStackSize; ++j) {
        if (j != i && m_segmentStack[j].Action != nullptr) {
            m_segmentStack[j].FadeStartAmplitude = m_segmentStack[j].Amplitude;
            m_segmentStack[j].FadeOutT0 = offset;
            m_segmentStack[j].FadeOutT1 = offset + fadeIn;
            m_segmentStack[j].Fading = true;
        }
    }

    IncrementStackPointer();
}

int ysAnimationChannel::GetPrevious() const {
    int i = m_currentStackPointer - 1;
    int prev = (i + SegmentStackSize) % SegmentStackSize;

    if (m_segmentStack[prev].Action == nullptr) return -1;
    else return prev;
}

int ysAnimationChannel::GetActiveSegments() const {
    int total = 0;
    for (int i = 0; i < SegmentStackSize; ++i) {
        if (m_segmentStack[i].Action != nullptr) {
            ++total;
        }
    }

    return total;
}

float ysAnimationChannel::ProbeTotalAmplitude() const {
    float total = 0.0f;
    for (int i = 0; i < SegmentStackSize; ++i) {
        if (m_segmentStack[i].Action != nullptr) {
            total += m_segmentStack[i].Amplitude;
        }
    }

    return total;
}

void ysAnimationChannel::KillSegment(int segment) {
    m_segmentStack[segment].Action = nullptr;
}

void ysAnimationChannel::IncrementStackPointer() {
    m_currentStackPointer = (m_currentStackPointer + 1) % SegmentStackSize;
}


ysAnimationMixer::ysAnimationMixer() {
    /* void */
}

ysAnimationMixer::~ysAnimationMixer() {
    for (ysAnimationChannel *channel : m_channels) {
        delete channel;
    }
}

ysAnimationChannel *ysAnimationMixer::NewChannel() {
    ysAnimationChannel *newChannel = new ysAnimationChannel;
    m_channels.push_back(newChannel);

    return newChannel;
}

void ysAnimationMixer::Sample() {
    for (ysAnimationChannel *channel : m_channels) {
        channel->Sample();
    }
}

void ysAnimationMixer::Advance(float dt) {
    for (ysAnimationChannel *channel : m_channels) {
        channel->Advance(dt);
    }
}
