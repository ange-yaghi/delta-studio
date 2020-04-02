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
    struct ActionSettings {
        float Speed = 1.0f;
        float FadeIn = 0.0f;
        float LeftClip = 0.0f;
        float RightClip = 0.0f;

        bool Clip = false;
    };

    static ActionSettings DefaultSettings;

    struct Segment {
        bool IsActive() const { return Action != nullptr; }

        ysAnimationActionBinding *Action;
        float Speed;
        float FadeIn;
        float LeftClip;
        float RightClip;

        float CurrentOffset;
        float Amplitude;
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

    ysAnimationActionBinding *GetCurrentAction() const;
    bool IsActionComplete() const;
    float GetPlayhead() const;
    void AddSegment(
        ysAnimationActionBinding *action, const ActionSettings &settings = DefaultSettings);
    void AddSegmentAtOffset(
        ysAnimationActionBinding *action, float offset, const ActionSettings &settings = DefaultSettings);
    void AddSegmentAtEnd(
        ysAnimationActionBinding *action, const ActionSettings &settings = DefaultSettings);

    void Balance();

    void SetAmplitude(float amplitude) { m_amplitude = amplitude; }
    float GetAmplitude() const { return m_amplitude; }

    void Mute() { m_amplitude = 0; }
    bool IsMuted() const { return m_amplitude == 0; }

    int GetActiveSegments() const;

    // Testing probes
    float ProbeTotalAmplitude() const;

protected:
    float m_amplitude;

protected:
    float ComputeSegmentPlayhead(const Segment &segment) const;

    float GetTotalAmplitude(int *activeSegments) const;

    void KillSegment(int segment);
    void IncrementStackPointer();

    int GetPrevious() const;

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
