#include "../include/character.h"

dbasic::Character::Character(const char *typeName) : ysObject(typeName) {
    m_skeleton = nullptr;
    m_animationData = nullptr;
    m_assetManager = nullptr;
    m_renderSkeleton = nullptr;
}

dbasic::Character::Character() : ysObject("Character") {
    m_skeleton = nullptr;
    m_animationData = nullptr;
    m_assetManager = nullptr;
    m_renderSkeleton = nullptr;
}

dbasic::Character::~Character() {
    /* void */
}

void dbasic::Character::Update() {
    m_animationController.Update();
    m_skeleton->Update();
    m_renderSkeleton->Update();
}

void dbasic::Character::SetSkeleton(Skeleton *skeleton) {
    m_skeleton = skeleton;
}

void dbasic::Character::SetRenderSkeleton(RenderSkeleton *renderSkeleton) {
    m_renderSkeleton = renderSkeleton;
}

void dbasic::Character::SetAnimationData(AnimationExportData *animationData) {
    m_animationData = animationData;
}

void dbasic::Character::ConstructBoneController(const char *boneName, AnimationGroup *group) {
    Bone *bone = m_skeleton->FindBone(boneName);
    AnimationObjectController *newController = m_assetManager->BuildAnimationObjectController(bone->GetName(), &bone->Transform);

    group->AddAnimationController(newController);
}

void dbasic::Character::ConstructNodeController(const char *nodeName, AnimationGroup *group) {
    RenderNode *node = m_renderSkeleton->FindNode(nodeName);
    AnimationObjectController *newController = m_assetManager->BuildAnimationObjectController(node->GetName(), &node->Transform);

    group->AddAnimationController(newController);
}

dbasic::AnimationGroup *dbasic::Character::GetAnimationController() {
    return &m_animationController;
}

void dbasic::Character::SetAssetManager(AssetManager *assetManager) {
    m_assetManager = assetManager;
}
