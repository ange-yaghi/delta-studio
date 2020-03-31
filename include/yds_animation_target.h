#ifndef YDS_ANIMATION_TARGET_H
#define YDS_ANIMATION_TARGET_H

#include "yds_math.h"

class ysAnimationCurve;

struct TransformTarget {
    void ClearFlags() {
        Animated[0] = Animated[1] = Animated[2] = Animated[3] = false;
    }

    void ClearRotation(const ysQuaternion &q) {
        ClearFlags();

        Data[0] = ysMath::GetQuatW(q);
        Data[1] = ysMath::GetQuatX(q);
        Data[2] = ysMath::GetQuatY(q);
        Data[3] = ysMath::GetQuatZ(q);
    }

    void ClearLocation(const ysVector &t) {
        ClearFlags();

        Data[0] = ysMath::GetX(t);
        Data[1] = ysMath::GetY(t);
        Data[2] = ysMath::GetZ(t);
        Data[3] = 0.0f;
    }

    void Accumulate(float t, int index) {
        if (!Animated[index]) {
            Data[index] = t;
            Animated[index] = true;
            return;
        }
        else {
            Data[index] += t;
        }
    }

    ysVector GetLocationResult() {
        return ysMath::LoadVector(Data[0], Data[1], Data[2], 0.0f);
    }

    ysQuaternion GetQuaternionResult() {
        return ysMath::LoadVector(Data[0], Data[1], Data[2], Data[3]);
    }

protected:
    float Data[4];
    bool Animated[4];
};

class ysAnimationTarget {
public:
    ysAnimationTarget();
    ~ysAnimationTarget();

    void SetLocationTarget(TransformTarget *target) { m_locationTarget = target; }
    void SetRotationTarget(TransformTarget *target) { m_rotationTarget = target; }

    void Sample(float s, float amplitude);

    void SetLocationCurve(ysAnimationCurve *curve, int index);
    void SetRotationCurve(ysAnimationCurve *curve, int index);
    
protected:
    TransformTarget *m_locationTarget;
    TransformTarget *m_rotationTarget;
    ysAnimationCurve *m_locationCurves[4];
    ysAnimationCurve *m_rotationCurves[4];
};

#endif /* YDS_ANIMATION_TARGET_H */
