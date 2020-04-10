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

ysVector ysTransform::WorldToLocalSpace(const ysVector &p) {
    ysVector origin = GetWorldPosition();
    ysQuaternion worldOrientation = GetWorldOrientation();

    ysVector d = ysMath::Sub(p, origin);
    return ysMath::QuatTransformInverse(worldOrientation, d);
}

ysVector ysTransform::LocalToWorldSpace(const ysVector &p) {
    ysVector origin = GetWorldPosition();
    ysQuaternion worldOrientation = GetWorldOrientation();

    ysVector d = ysMath::QuatTransform(worldOrientation, p);
    return ysMath::Add(d, origin);
}

ysVector ysTransform::WorldToLocalDirection(const ysVector &dir) {
    ysQuaternion worldOrientation = GetWorldOrientation();
    return ysMath::QuatTransformInverse(worldOrientation, dir);
}

ysVector ysTransform::LocalToWorldDirection(const ysVector &dir) {
    ysQuaternion worldOrientation = GetWorldOrientation();
    return ysMath::QuatTransform(worldOrientation, dir);
}

ysQuaternion ysTransform::GetLocalOrientation() const {
    return m_orientation;
}

ysQuaternion ysTransform::GetWorldOrientation() {
    if (m_parent == nullptr) return m_orientation;
    else {
        return ysMath::QuatMultiply(m_parent->GetWorldOrientation(), m_orientation);
    }
}

ysVector ysTransform::GetLocalPosition() const {
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
