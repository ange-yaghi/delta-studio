#include "../include/animation_group.h"

dbasic::AnimationGroup::AnimationGroup() : ysObject("AnimationGroup") {
    m_name[0] = '\0';
}

dbasic::AnimationGroup::~AnimationGroup() {
    /* void */
}

void dbasic::AnimationGroup::Update() {
    // Iterate through all controllers and update them all
    int controllerCount = m_animationControllers.GetNumObjects();
    for (int i = 0; i < controllerCount; i++) {
        m_animationControllers[i]->Update();
    }

    // Iterate through all child groups
    int groupCount = m_animationGroups.GetNumObjects();
    for (int i = 0; i < groupCount; i++) {
        m_animationGroups.Get(i)->Update();
    }
}

void dbasic::AnimationGroup::AddAnimationController(dbasic::AnimationObjectController *controller) {
    // Add an animation controller to this group
    m_animationControllers.New() = controller;
}

dbasic::AnimationGroup *dbasic::AnimationGroup::AddAnimationGroup(const char *groupName) {
    // Add a child animation group
    AnimationGroup *newGroup = m_animationGroups.NewGeneric<AnimationGroup>();

    if (groupName != NULL)
        newGroup->SetName(groupName);

    return newGroup;
}

void dbasic::AnimationGroup::SetName(const char *name) {
    strcpy_s(m_name, 64, name);
}

void dbasic::AnimationGroup::SetFrame(int frame) {
    // Iterate through all controllers and set the frame
    int controllerCount = m_animationControllers.GetNumObjects();
    for (int i = 0; i < controllerCount; i++) {
        m_animationControllers[i]->SetFrame(frame);
    }

    // Iterate through all child groups
    int groupCount = m_animationGroups.GetNumObjects();
    for (int i = 0; i < groupCount; i++) {
        m_animationGroups.Get(i)->SetFrame(frame);
    }
}

void dbasic::AnimationGroup::SetTimeOffset(float timeOffset) {
    // Iterate through all controllers and set the frame
    int controllerCount = m_animationControllers.GetNumObjects();
    for (int i = 0; i < controllerCount; i++) {
        m_animationControllers[i]->SetTimeOffset(timeOffset);
    }

    // Iterate through all child groups
    int groupCount = m_animationGroups.GetNumObjects();
    for (int i = 0; i < groupCount; i++) {
        m_animationGroups.Get(i)->SetTimeOffset(timeOffset);
    }
}
