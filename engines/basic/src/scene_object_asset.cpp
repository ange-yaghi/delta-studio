#include "../include/scene_object_asset.h"

#include "../include/asset_manager.h"

dbasic::SceneObjectAsset::SceneObjectAsset() : ysObject("SceneObjectAsset") {
    SetName("");

    m_parent = -1;
    m_skeletonIndex = -1;
    m_type = ysObjectData::TYPE_UNDEFINED;

    m_localTransform = ysMath::LoadIdentity();
    m_localOrientation = ysMath::Constants::QuatIdentity;
    m_localPosition = ysMath::Constants::Zero;

    m_geometry = NULL;
    m_manager = NULL;
    m_material = NULL;
}

dbasic::SceneObjectAsset::~SceneObjectAsset() {
    /* void */
}

ysMatrix dbasic::SceneObjectAsset::GetWorldTransform() const {
    SceneObjectAsset *parent = m_manager->GetSceneObject(m_parent);
    return (parent != NULL) ? ysMath::MatMult(parent->GetWorldTransform(), m_localTransform) : m_localTransform;
}

void dbasic::SceneObjectAsset::ApplyTransformation(const ysMatrix &transform) {
    m_localTransform = ysMath::MatMult(m_localTransform, transform);
}

ysVector dbasic::SceneObjectAsset::GetWorldPosition() const {
    ysVector result = (m_parent == -1) ? m_localPosition : ysMath::MatMult(m_manager->GetSceneObject(m_parent)->GetWorldTransform(), ysMath::ExtendVector(m_localPosition));
    return result;
}