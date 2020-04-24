#include "../include/yds_animation_action.h"

#include "../include/yds_animation_target.h"

ysAnimationAction::ysAnimationAction() : ysObject("ysAnimationAction") {
    m_curveCount = 0;
    m_length = 0.0f;
}

ysAnimationAction::~ysAnimationAction() {
    for (auto group : m_curves) {
        for (ysAnimationCurve *curve : group.second) {
            delete curve;
        }
    }
}

ysAnimationCurve *ysAnimationAction::NewCurve(const std::string &target) {
    m_curves.emplace(target, std::vector<ysAnimationCurve *>());

    ysAnimationCurve *newCurve = new ysAnimationCurve;
    newCurve->SetTarget(target);
    m_curves[target].push_back(newCurve);

    ++m_curveCount;

    return newCurve;
}

ysAnimationCurve *ysAnimationAction::GetCurve(const std::string &target, ysAnimationCurve::CurveType type) {
    auto f = m_curves.find(target);
    if (f == m_curves.end()) return nullptr;

    std::vector<ysAnimationCurve *> &curveList = f->second;
    for (ysAnimationCurve *curve : curveList) {
        if (curve->GetCurveType() == type) {
            return curve;
        }
    }

    return nullptr;
}

int ysAnimationAction::GetCurveCount() const {
    return m_curveCount;
}

bool ysAnimationAction::IsAnimated(const std::string &objectName) const {
    auto f = m_curves.find(objectName);
    if (f == m_curves.end()) return false;
    else return !(*f).second.empty();
}

void ysAnimationAction::Bind(const std::string &objectName, ysAnimationTarget *target) {
    auto f = m_curves.find(objectName);
    if (f == m_curves.end()) return;
    else {
        std::vector<ysAnimationCurve *> &curves = (*f).second;
        for (ysAnimationCurve *curve : curves) {
            curve->Attach(target);
        }
    }
}
