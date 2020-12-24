#include "../include/yds_transform.h"

ysTransform::ysTransform() {
    m_position = ysMath::Constants::Zero3;
    m_orientation = ysMath::Constants::QuatIdentity;

    m_parent = nullptr;
}

ysTransform::~ysTransform() {
    /* void */
}

void ysTransform::SetPosition(const ysVector &position) {
    m_position = position;
}

void ysTransform::SetOrientation(const ysQuaternion &q) {
    m_orientation = q;
}

ysMatrix ysTransform::GetWorldTransform() {
    ysMatrix localTransformation = ysMath::LoadMatrix(m_orientation, m_position);
    if (m_parent == nullptr) {
        return localTransformation;
    }
    else {
        return ysMath::MatMult(m_parent->GetWorldTransform(), localTransformation);
    }
}

ysVector ysTransform::WorldToLocalSpace(const ysVector &p) {
    ysVector d;
    if (m_parent == nullptr) d = p;
    else d = m_parent->WorldToLocalSpace(p);

    return ysMath::QuatTransformInverse(m_orientation, ysMath::Sub(d, m_position));
}

ysVector ysTransform::WorldToParentSpace(const ysVector &p) {
    return LocalToParentSpace(WorldToLocalSpace(p));
}

ysVector ysTransform::LocalToWorldSpace(const ysVector &p) {
    const ysVector d = ysMath::Add(ysMath::QuatTransform(m_orientation, p), m_position);
    
    if (m_parent == nullptr) return d;
    else return m_parent->LocalToWorldSpace(d);
}

ysVector ysTransform::LocalToParentSpace(const ysVector &p) {
    ysVector world = LocalToWorldSpace(p);
    
    if (m_parent == nullptr) return world;
    else return m_parent->WorldToLocalSpace(world);
}

ysVector ysTransform::WorldToLocalDirection(const ysVector &dir) {
    ysQuaternion worldOrientation = GetWorldOrientation();
    return ysMath::QuatTransformInverse(worldOrientation, dir);
}

ysVector ysTransform::WorldToParentDirection(const ysVector &dir) {
    return LocalToParentDirection(WorldToLocalDirection(dir));
}

ysVector ysTransform::LocalToWorldDirection(const ysVector &dir) {
    const ysQuaternion worldOrientation = GetWorldOrientation();
    return ysMath::QuatTransform(worldOrientation, dir);
}

ysVector ysTransform::LocalToParentDirection(const ysVector &dir) {
    const ysVector world = LocalToWorldDirection(dir);

    if (m_parent == nullptr) return world;
    else return m_parent->WorldToLocalDirection(world);
}

ysVector ysTransform::ParentToLocalSpace(const ysVector &p) {
    if (m_parent == nullptr) return WorldToLocalSpace(p);
    else return WorldToLocalSpace(ParentToWorldSpace(p));
}

ysVector ysTransform::ParentToWorldSpace(const ysVector &p) {
    if (m_parent == nullptr) return p;
    else return m_parent->LocalToWorldSpace(p);
}

ysVector ysTransform::ParentToLocalDirection(const ysVector &p) {
    if (m_parent == nullptr) return WorldToLocalDirection(p);
    else return WorldToLocalDirection(ParentToWorldDirection(p));
}

ysVector ysTransform::ParentToWorldDirection(const ysVector &p) {
    if (m_parent == nullptr) return p;
    else return m_parent->LocalToWorldDirection(p);
}

ysQuaternion ysTransform::WorldToLocalOrientation(const ysQuaternion &q) {
    return ysMath::QuatMultiply(
        ysMath::QuatInvert(ysMath::Normalize(GetWorldOrientation())), q);
}

ysQuaternion ysTransform::WorldToParentOrientation(const ysQuaternion &q) {
    if (m_parent == nullptr) return q;
    else return ysMath::QuatMultiply(
        ysMath::QuatInvert(ysMath::Normalize(m_parent->GetWorldOrientation())), q);
}

ysQuaternion ysTransform::GetOrientationParentSpace() const {
    return m_orientation;
}

ysQuaternion ysTransform::GetWorldOrientation() {
    if (m_parent == nullptr) return m_orientation;
    else {
        return ysMath::QuatMultiply(m_parent->GetWorldOrientation(), m_orientation);
    }
}

ysVector ysTransform::GetPositionParentSpace() const {
    return m_position;
}

ysVector ysTransform::GetWorldPosition() {
    if (m_parent == nullptr) return m_position;
    else {
        ysQuaternion worldOrientation = m_parent->GetWorldOrientation();
        ysVector worldOffset = m_parent->GetWorldPosition();

        return ysMath::Add(
            ysMath::QuatTransform(worldOrientation, m_position),
            worldOffset);
    }
}

void ysTransform::SetParent(ysTransform *parent) {
    m_parent = parent;
}

bool ysTransform::IsValid() {
    if (!ysMath::IsValid(m_orientation)) return false;
    if (!ysMath::IsValid(m_position)) return false;
    return true;
}
