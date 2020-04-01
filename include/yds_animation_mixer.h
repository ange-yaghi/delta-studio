#ifndef YDS_ANIMATION_MIXER_H
#define YDS_ANIMATION_MIXER_H

#include "yds_animation_curve.h"
#include "yds_math.h"

#include <vector>

class ysAnimationActionBinding;

class ysAnimationChannel {
public:
    static constexpr int SegmentStackSize = 16;

public:
    struct Segment {
        bool IsActive() const { return Action != nullptr; }

        ysAnimationActionBinding *Action;

        float CurrentOffset;
        float Amplitude;
        float FadeIn;
        float Speed;

        float FadeStartAmplitude;
        float FadeOutT0;
        float FadeOutT1;

        bool Fading;
    };

public:
    ysAnimationChannel();
    ~ysAnimationChannel();

    void Reset();
    void Sample();
    void Advance(float dt);

    void AddSegment(ysAnimationActionBinding *action, float fadeIn, float speed = 1.0f);
    void AddSegmentAtEnd(ysAnimationActionBinding *action, float fadeIn, float speed = 1.0f);

    void SetAmplitude(float amplitude) { m_amplitude = amplitude; }
    float GetAmplitude() const { return m_amplitude; }

    void Mute() { m_amplitude = 0; }
    bool IsMuted() const { return m_amplitude == 0; }

    int GetPrevious() const;

    int GetActiveSegments() const;

    // Testing probes
    float ProbeTotalAmplitude() const;

protected:
    float m_amplitude;

protected:
    void KillSegment(int segment);
    void IncrementStackPointer();

    Segment m_segmentStack[SegmentStackSize];
    int m_currentStackPointer;
};

class ysAnimationMixer {
public:
    ysAnimationMixer();
    ~ysAnimationMixer();

    ysAnimationChannel *NewChannel();
    int GetChannelCount() const { return (int)m_channels.size(); }

    void Sample();
    void Advance(float dt);

protected:
    std::vector<ysAnimationChannel *> m_channels;
};

#endif /* YDS_ANIMATION_MIXER_H */
