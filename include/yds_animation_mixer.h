#ifndef YDS_ANIMATION_MIXER_H
#define YDS_ANIMATION_MIXER_H

#include "yds_animation_curve.h"
#include "yds_math.h"

#include <vector>

class ysAnimationAction;

class ysAnimationChannel {
public:
    static constexpr int SegmentStackSize = 16;

public:
    struct Segment {
        float CurrentOffset;
        float Amplitude;
        float FadeIn;

        float FadeStartAmplitude;
        float FadeOutT0;
        float FadeOutT1;

        bool Fading;

        ysAnimationAction *Action;
    };

public:
    ysAnimationChannel();
    ~ysAnimationChannel();

    void Reset();
    void Update(float dt);

    void AddSegment(ysAnimationAction *action, float fadeIn);

    void SetAmplitude(float amplitude) { m_amplitude = amplitude; }
    float GetAmplitude() const { return m_amplitude; }

protected:
    float m_amplitude;

protected:
    void KillSegment(int segment);

    Segment m_segmentStack[SegmentStackSize];
    int m_currentStackPointer;
};

class ysAnimationMixer {
public:
    ysAnimationMixer();
    ~ysAnimationMixer();

    ysAnimationCurve *m_curve;

};

#endif /* YDS_ANIMATION_MIXER_H */
