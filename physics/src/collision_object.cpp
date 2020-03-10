#include "../include/collision_object.h"
#include "../include/rigid_body.h"

dphysics::CollisionObject::CollisionObject() : ysObject("CollisionObject") {
    m_primitiveHandle = NULL;
    m_type = Type::Undefined;
    m_mode = Mode::Fine;

    m_relativePosition = ysMath::Constants::Zero;
    m_relativeOrientation = ysMath::LoadIdentity();

    m_collisionLayerMask = 0xFFFFFFFF;
    m_layerMask = 0xFFFFFFFF;

    m_parent = NULL;

    m_msg = NULL;
}

dphysics::CollisionObject::CollisionObject(Type type) : ysObject("CollisionObject") {
    m_primitiveHandle = NULL;
    m_type = type;
    m_mode = Mode::Fine;

    m_relativePosition = ysMath::Constants::Zero;
    m_relativeOrientation = ysMath::LoadIdentity();

    m_collisionLayerMask = 0xFFFFFFFF;
    m_layerMask = 0xFFFFFFFF;

    m_parent = NULL;

    m_msg = NULL;
}

dphysics::CollisionObject::~CollisionObject() {
    /* void */
}

void dphysics::CollisionObject::ConfigureBox() {
    BoxPrimitive *prim = GetAsBox();

    prim->Orientation = ysMath::MatMult(m_relativeOrientation, m_parent->GetOrientationMatrix());
    prim->Position = ysMath::MatMult(m_parent->GetTransform(), ysMath::ExtendVector(m_relativePosition));
}

void dphysics::CollisionObject::ConfigureCircle() {
    CirclePrimitive *prim = GetAsCircle();

    prim->Position = ysMath::MatMult(m_parent->GetTransform(), ysMath::ExtendVector(m_relativePosition));
}

void dphysics::CollisionObject::ConfigurePrimitive() {
    switch (m_type) {
    case Type::Box:
        ConfigureBox();
        break;

    case Type::Circle:
        ConfigureCircle();
        break;
    }
}

bool dphysics::CollisionObject::CheckCollisionMask(const CollisionObject *object) const {
    if ((object->m_collisionLayerMask & m_layerMask) ||
        (object->m_layerMask & m_collisionLayerMask)) {

        return true;
    }

    return false;
}
