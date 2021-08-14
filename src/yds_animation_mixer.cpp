#include "../include/yds_animation_mixer.h"

#include "../include/yds_animation_action.h"
#include "../include/yds_animation_action_binding.h"

#include <assert.h>
#include <algorithm>
#include <cmath>

ysAnimationChannel::ActionSettings ysAnimationChannel::DefaultSettings;

ysAnimationChannel::ysAnimationChannel() {
    m_amplitude = 1.0f;

    Reset();
}

ysAnimationChannel::~ysAnimationChannel() {
    /* void */
}

void ysAnimationChannel::Reset() {
    m_currentStackPointer = 0;
    m_queuedAction = nullptr;
    m_actionQueued = false;

    for (int i = 0; i < SegmentStackSize; ++i) {
        m_segmentStack[i].Action = nullptr;
    }
}

void ysAnimationChannel::Sample() {
    HandleQueue();

    // Update segment that is fading in first
    for (int i = 0; i < SegmentStackSize; ++i) {
        if (m_segmentStack[i].IsActive()) {
            if (m_segmentStack[i].Fading) {
                if (m_segmentStack[i].FadeOutT0 <= 0) {
                    continue;
                }
            }

            float d = 0 - m_segmentStack[i].CurrentOffset;
            float t = m_segmentStack[i].FadeIn;

            if (d < 0) continue;

            if (d < t && t != 0) {
                m_segmentStack[i].Amplitude = (d / t);
            }
            else {
                m_segmentStack[i].Amplitude = 1.0f;
            }
        }
    }

    // Update segments that are fading out
    for (int i = 0; i < SegmentStackSize; ++i) {
        if (m_segmentStack[i].IsActive() && m_segmentStack[i].Fading) {
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

    Balance();

    for (int i = 0; i < SegmentStackSize; ++i) {
        Segment &segment = m_segmentStack[i];
        if (segment.IsActive()) {
            float s = ComputeSegmentPlayhead(segment);
            if (segment.IsActive()) {
                segment.Action->Sample(s, segment.Amplitude);
            }
        }
    }
}

void ysAnimationChannel::HandleQueue() {
    if (!HasQueuedSegments()) return;

    int last = GetPrevious();
    if (last == -1) {
        AddSegment(m_queuedAction, m_queuedSettings);
    }
    else {
        Segment &segment = m_segmentStack[last];
        float s = ComputeSegmentPlayhead(segment);
        float offset = (segment.Speed < 0)
            ? s - segment.LeftClip
            : segment.RightClip - s;

        if (offset <= 0) {
            AddSegmentAtOffset(m_queuedAction, offset, m_queuedSettings);
        }
    }

    m_actionQueued = false;
}

void ysAnimationChannel::Advance(float dt) {
    for (int i = 0; i < SegmentStackSize; ++i) {
        if (m_segmentStack[i].IsActive()) {
            m_segmentStack[i].CurrentOffset -= dt;

            if (m_segmentStack[i].Fading) {
                m_segmentStack[i].FadeOutT0 -= dt;
                m_segmentStack[i].FadeOutT1 -= dt;
            }

            m_segmentStack[i].Playhead += m_segmentStack[i].Speed * dt;
        }
    }
}

ysAnimationActionBinding *ysAnimationChannel::GetCurrentAction() const {
    int last = GetPrevious();
    if (last == -1) return nullptr;

    if (last >= SegmentStackSize) {
        int breakHere = 0;
    }

    const Segment &currentSegment = m_segmentStack[last];
    return currentSegment.Action;
}

bool ysAnimationChannel::IsActionComplete() const {
    int index = GetPrevious();
    if (index == -1) return true;

    const Segment &segment = m_segmentStack[index];
    float s = ComputeSegmentPlayhead(segment);

    if (segment.Speed >= 0) {
        return s >= segment.RightClip;
    }
    else {
        return s <= segment.LeftClip;
    }
}

float ysAnimationChannel::GetPlayhead() const {
    int last = GetPrevious();
    if (last == -1) return 0.0f;
    else {
        const Segment &segment = m_segmentStack[last];
        return ComputeSegmentPlayhead(segment);
    }
}

void ysAnimationChannel::AddSegment(ysAnimationActionBinding *action, const ActionSettings &settings) {
    AddSegmentAtOffset(action, 0.0f, settings);
}

void ysAnimationChannel::AddSegmentAtOffset(ysAnimationActionBinding *action, float offset, const ActionSettings &settings) {
    int i = m_currentStackPointer;

    m_segmentStack[i].Action = action;
    m_segmentStack[i].FadeIn = settings.FadeIn;
    m_segmentStack[i].Speed = settings.Speed;
    m_segmentStack[i].Amplitude = 0.0f;
    m_segmentStack[i].CurrentOffset = offset;
    m_segmentStack[i].Fading = false;

    if (settings.Clip) {
        m_segmentStack[i].LeftClip = settings.LeftClip;
        m_segmentStack[i].RightClip = settings.RightClip;
    }
    else {
        m_segmentStack[i].LeftClip = 0.0f;
        m_segmentStack[i].RightClip = action->GetAction()->GetLength();
    }

    if (settings.Speed < 0) {
        m_segmentStack[i].Playhead = m_segmentStack[i].RightClip;
    }
    else {
        m_segmentStack[i].Playhead = m_segmentStack[i].LeftClip;
    }

    for (int j = 0; j < SegmentStackSize; ++j) {
        if (j != i && m_segmentStack[j].IsActive()) {
            if (!m_segmentStack[j].Fading || m_segmentStack[j].FadeOutT0 > offset) {
                m_segmentStack[j].FadeStartAmplitude = m_segmentStack[j].Amplitude;
                m_segmentStack[j].FadeOutT0 = offset;
            }

            if (!m_segmentStack[j].Fading || m_segmentStack[j].FadeOutT1 > offset + settings.FadeIn) {
                m_segmentStack[j].FadeOutT1 = offset + settings.FadeIn;
            }

            m_segmentStack[j].Fading = true;
        }
    }

    IncrementStackPointer();
}

void ysAnimationChannel::Balance() {
    int activeSegments = 0;
    float totalAmplitude = GetTotalAmplitude(&activeSegments);
    if (activeSegments <= 1 || totalAmplitude == 0) return;

    float inv = 1 / totalAmplitude;

    for (int i = 0; i < SegmentStackSize; ++i) {
        if (m_segmentStack[i].IsActive() && m_segmentStack[i].CurrentOffset <= 0) {
            m_segmentStack[i].Amplitude *= inv;
        }
    }
}

void ysAnimationChannel::QueueSegment(ysAnimationActionBinding *action, const ActionSettings &settings) {
    m_actionQueued = true;
    m_queuedAction = action;
    m_queuedSettings = settings;
}

void ysAnimationChannel::ClearQueue() {
    m_actionQueued = false;
    m_queuedAction = nullptr;
}

void ysAnimationChannel::ChangeSpeed(float speed) {
    int last = GetPrevious();
    if (last == -1) return;
    else {
        m_segmentStack[last].Speed = speed;
    }
}

float ysAnimationChannel::GetSpeed() const {
    int last = GetPrevious();
    if (last == -1) return 0.0f;
    else {
        return m_segmentStack[last].Speed;
    }
}

int ysAnimationChannel::GetPrevious() const {
    int i = m_currentStackPointer;
    do {
        int prev = (--i + SegmentStackSize) % SegmentStackSize;
        i = prev;

        if (!m_segmentStack[prev].IsActive()) return -1;
        else {
            if (m_segmentStack[prev].CurrentOffset <= 0) return prev;
        }
    } while (i != m_currentStackPointer);

    return -1;
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
        if (m_segmentStack[i].IsActive() && m_segmentStack[i].CurrentOffset <= 0) {
            total += m_segmentStack[i].Amplitude;
        }
    }

    return total;
}

float ysAnimationChannel::ComputeSegmentPlayhead(const Segment &segment) const {
    return segment.Playhead;
}

float ysAnimationChannel::GetTotalAmplitude(int *activeSegments) const {
    *activeSegments = 0;
    
    float total = 0.0f;
    for (int i = 0; i < SegmentStackSize; ++i) {
        if (m_segmentStack[i].IsActive() && m_segmentStack[i].CurrentOffset <= 0) {
            total += m_segmentStack[i].Amplitude;
            ++(*activeSegments);
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
