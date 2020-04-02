#include "../include/yds_animation_action_binding.h"

#include "../include/yds_animation_target.h"
#include "../include/yds_animation_action.h"

ysAnimationActionBinding::ysAnimationActionBinding() {
    m_action = nullptr;
}

ysAnimationActionBinding::~ysAnimationActionBinding() {
    for (ysAnimationTarget *target : m_targets) {
        delete target;
    }
}

void ysAnimationActionBinding::Sample(float s, float amplitude) {
    for (ysAnimationTarget *target : m_targets) {
        target->Sample(s, amplitude);
    }
}

void ysAnimationActionBinding::SampleRest(float amplitude) {
    for (ysAnimationTarget *target : m_targets) {
        target->SampleRest(amplitude);
    }
}

void ysAnimationActionBinding::AddTarget(const std::string &name, TransformTarget *locationTarget, TransformTarget *rotationTarget) {
    ysAnimationTarget *newTarget = new ysAnimationTarget;
    newTarget->SetLocationTarget(locationTarget);
    newTarget->SetRotationTarget(rotationTarget);
    m_action->Bind(name, newTarget);

    m_targets.push_back(newTarget);
}
