#ifndef YDS_ANIMATION_ACTION_H
#define YDS_ANIMATION_ACTION_H

#include "yds_animation_curve.h"

#include "yds_math.h"

#include <vector>

class ysAnimationAction {
public:
    ysAnimationAction();
    ~ysAnimationAction();

    void AddCurve(ysAnimationCurve *curve) { m_curves.push_back(curve); }
    int GetCurveCount() const { return (int)m_curves.size(); }

protected:
    std::vector<ysAnimationCurve *> m_curves;
};

#endif /* YDS_ANIMATION_ACTION_H */
