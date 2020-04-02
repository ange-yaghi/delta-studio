#ifndef YDS_ANIMATION_CURVE_H
#define YDS_ANIMATION_CURVE_H

#include <map>
#include <string>

class ysAnimationTarget;

class ysAnimationCurve {
public:
    enum class CurveType {
        Undefined = 0x0,

        LocationVec = 0x1,
        RotationQuat = 0x2,

        LocationX = 0x3,
        LocationY = 0x4,
        LocationZ = 0x5,

        RotationQuatW = 0x6,
        RotationQuatX = 0x7,
        RotationQuatY = 0x8,
        RotationQuatZ = 0x9
    };

public:
    ysAnimationCurve();
    ~ysAnimationCurve();

    float Sample(float s);
    float GetRestValue();

    void SetTarget(const std::string &target) { m_target = target; }
    std::string GetTarget() const { return m_target; }

    void SetCurveType(CurveType curveType) { m_curveType = curveType; }
    CurveType GetCurveType() const { return m_curveType; }

    void AddSamplePoint(float s, float v);
    int GetSampleCount() const { return (int)m_samples.size(); }

    void Attach(ysAnimationTarget *target);

protected:
    std::map<float, float> m_samples;
    std::string m_target;
    CurveType m_curveType;
};

#endif /* YDS_ANIMATION_CURVE_H */
