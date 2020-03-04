#include "../include/collision_object.h"
#include "../include/rigid_body.h"

dbasic::CollisionObject::CollisionObject() : ysObject("CollisionObject") {
    m_primitiveHandle = NULL;
    m_type = COLLISION_OBJECT_TYPE_UNDEFINED;
    m_mode = COLLISION_OBJECT_MODE_COLLISION_FINE;

    m_relativePosition = ysMath::Constants::Zero;
    m_relativeOrientation = ysMath::LoadIdentity();

    m_collisionLayerMask = 0xFFFFFFFF;
    m_layerMask = 0xFFFFFFFF;

    m_parent = NULL;

    m_msg = NULL;
}

dbasic::CollisionObject::CollisionObject(COLLISION_OBJECT_TYPE type) : ysObject("CollisionObject") {
    m_primitiveHandle = NULL;
    m_type = type;
    m_mode = COLLISION_OBJECT_MODE_COLLISION_FINE;

    m_relativePosition = ysMath::Constants::Zero;
    m_relativeOrientation = ysMath::LoadIdentity();

    m_parent = NULL;

    m_msg = NULL;
}

dbasic::CollisionObject::~CollisionObject() {
    /* void */
}

void dbasic::CollisionObject::ConfigureBox() {
    BoxPrimitive *prim = GetAsBox();

    prim->Orientation = ysMath::MatMult(m_relativeOrientation, m_parent->GetOrientationMatrix());
    prim->Position = ysMath::MatMult(m_parent->GetTransform(), ysMath::ExtendVector(m_relativePosition));
}

void dbasic::CollisionObject::ConfigureCircle() {
    CirclePrimitive *prim = GetAsCircle();

    prim->Position = ysMath::MatMult(m_parent->GetTransform(), ysMath::ExtendVector(m_relativePosition));
}

void dbasic::CollisionObject::ConfigurePrimitive() {
    switch (m_type) {
    case COLLISION_OBJECT_TYPE_BOX:
        ConfigureBox();
        break;

    case COLLISION_OBJECT_TYPE_CIRCLE:
        ConfigureCircle();
        break;
    }
}

bool dbasic::CollisionObject::CheckCollisionMask(const CollisionObject *object) const {
    if ((object->m_collisionLayerMask & m_layerMask) ||
        (object->m_layerMask & m_collisionLayerMask)) {

        return true;
    }

    return false;
}
