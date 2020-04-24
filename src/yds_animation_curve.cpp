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
        return next->second.v;
    }

    auto prev = next; --prev;
    if (next == m_samples.end()) {
        return prev->second.v;
    }

    CurveHandle &handle1 = prev->second;
    CurveHandle &handle2 = next->second;

    if (handle1.mode == CurveHandle::InterpolationMode::Linear) {
        // Linear interpolation
        float dist = (handle2.s - handle1.s);
        float sdist = s - handle1.s;

        float w1 = (sdist / dist);
        float w0 = 1.0f - w1;

        return handle1.v * w0 + handle2.v * w1;
    }
    else if (handle1.mode == CurveHandle::InterpolationMode::Bezier) {
        // Cubic bezier interpolation
        float t = Bezier_t(s, 
            handle1.s, handle1.r_handle_x, handle2.l_handle_x, handle2.s);

        float B_t_y =
            (1 - t) * (1 - t) * (1 - t) * handle1.v +
            3 * t * (1 - t) * (1 - t) * handle1.r_handle_y +
            3 * t * t * (1 - t) * handle2.l_handle_y +
            t * t * t * handle2.v;

        return B_t_y;
    }
    else {
        return 0.0f;
    }
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

void ysAnimationCurve::AddSamplePoint( const ysAnimationCurve::CurveHandle &handle) {
    m_samples[handle.s] = handle;
}

void ysAnimationCurve::AddLinearSamplePoint(float s, float t) {
    CurveHandle handle;
    handle.mode = CurveHandle::InterpolationMode::Linear;
    handle.l_handle_x = handle.l_handle_y = 0.0f;
    handle.r_handle_x = handle.r_handle_y = 0.0f;
    handle.s = s;
    handle.v = t;

    AddSamplePoint(handle);
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

float ysAnimationCurve::Bezier_t(float x, float p0_x, float p1_x, float p2_x, float p3_x) {
    // B(t) = (1 - t)^3 * P0 + 3t(1 - t)^2 * P1 + 3t^2(1 - t) * P2 + t^3 * P3

    constexpr float Epsilon = 0.001f;
    constexpr int MaxIterations = 20;

    float l = 0.0f;
    float r = 1.0f;

    for (int i = 0; i < MaxIterations; ++i) {
        float t = (l + r) / 2.0f;

        float B_t_x =
            (1 - t) * (1 - t) * (1 - t) * p0_x +
            3 * t * (1 - t) * (1 - t) * p1_x +
            3 * t * t * (1 - t) * p2_x +
            t * t * t * p3_x;

        float d = std::abs(B_t_x - x);
        if (d < Epsilon) return t;

        if (B_t_x < x) {
            l = t;
        }
        else if (B_t_x > x) {
            r = t;
        }
    }

    return (l + r) / 2.0f;
}
