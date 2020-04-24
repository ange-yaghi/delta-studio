#include "../include/yds_animation_target.h"

#include "../include/yds_animation_curve.h"

ysAnimationTarget::ysAnimationTarget() {
    for (int i = 0; i < 4; ++i) {
        m_locationCurves[i] = nullptr;
        m_rotationCurves[i] = nullptr;
    }

    m_rotationTarget = nullptr;
    m_locationTarget = nullptr;
}

ysAnimationTarget::~ysAnimationTarget() {
    /* void */
}

void ysAnimationTarget::Sample(float s, float amplitude) {
    for (int i = 0; i < 4; ++i) {
        if (m_locationCurves[i] != nullptr) {
            m_locationTarget->Accumulate(
                m_locationCurves[i]->Sample(s) * amplitude, i);
        }
    }

    if (m_rotationCurves[0] != nullptr) {
        float rotation[4];
        for (int i = 0; i < 4; ++i) {
            rotation[i] = m_rotationCurves[i]->Sample(s);
        }

        ysQuaternion q = ysMath::LoadVector(rotation[0], rotation[1], rotation[2], rotation[3]);
        m_rotationTarget->AccumulateQuaternion(q, amplitude);
    }
}

void ysAnimationTarget::SampleRest(float amplitude) {
    for (int i = 0; i < 4; ++i) {
        if (m_locationCurves[i] != nullptr) {
            m_locationTarget->Accumulate(
                m_locationCurves[i]->GetRestValue() * amplitude, i);
        }

        if (m_rotationCurves[i] != nullptr) {
            m_rotationTarget->Accumulate(
                m_rotationCurves[i]->GetRestValue() * amplitude, i);
        }
    }
}

void ysAnimationTarget::SetLocationCurve(ysAnimationCurve *curve, int index) {
    m_locationCurves[index] = curve;
}

void ysAnimationTarget::SetRotationCurve(ysAnimationCurve *curve, int index) {
    m_rotationCurves[index] = curve;
}
