#include "../include/yds_animation_curve.h"

#include "../include/yds_animation_target.h"

ysAnimationCurve::ysAnimationCurve() {
    m_curveType = CurveType::Undefined;
}

ysAnimationCurve::~ysAnimationCurve() {
    /* void */
}

float ysAnimationCurve::Sample(float s) {
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

float ysAnimationCurve::GetRestValue() {
    switch (m_curveType) {
    case CurveType::LocationX:
    case CurveType::LocationY:
    case CurveType::LocationZ:
        return 0.0f;
    case CurveType::RotationQuatW:
        return 1.0f;
    case CurveType::RotationQuatX:
    case CurveType::RotationQuatY:
    case CurveType::RotationQuatZ:
        return 0.0f;
    default:
        return 0.0f;
    }
}

void ysAnimationCurve::AddSamplePoint(float s, float v) {
    m_samples[s] = v;
}

void ysAnimationCurve::Attach(ysAnimationTarget *target) {
    switch (m_curveType) {
    case CurveType::LocationX:
        target->SetLocationCurve(this, 0);
        break;
    case CurveType::LocationY:
        target->SetLocationCurve(this, 1);
        break;
    case CurveType::LocationZ:
        target->SetLocationCurve(this, 2);
        break;
    case CurveType::RotationQuatW:
        target->SetRotationCurve(this, 0);
        break;
    case CurveType::RotationQuatX:
        target->SetRotationCurve(this, 1);
        break;
    case CurveType::RotationQuatY:
        target->SetRotationCurve(this, 2);
        break;
    case CurveType::RotationQuatZ:
        target->SetRotationCurve(this, 3);
        break;
    case CurveType::LocationVec:
        /* Not implemented */
        break;
    case CurveType::RotationQuat:
        /* Not implemented */
        break;
    }
}
