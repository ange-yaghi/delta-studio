#include "../include/yds_animation_curve.h"

ysAnimationCurve::ysAnimationCurve() {
    m_curveType = CurveType::Undefined;
}

ysAnimationCurve::~ysAnimationCurve() {
    /* void */
}

inline float ysAnimationCurve::Sample(float s) {
    if (m_samples.empty()) {
        return 0.0f;
    }

    auto next = m_samples.lower_bound(s);
    if (next == m_samples.begin()) {
        return next->second;
    }

    auto prev = next; --prev;
    if (next == m_samples.end()) {
        return prev->second;
    }

    float dist = (next->first - prev->first);
    float sdist = s - prev->first;

    float w1 = (sdist / dist);
    float w0 = 1.0f - w1;

    return prev->second * w0 + next->second * w1;
}

inline void ysAnimationCurve::AddSamplePoint(float s, float v) {
    m_samples[s] = v;
}
