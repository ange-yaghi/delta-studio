#include "../include/collision_object.h"

#include "../include/rigid_body.h"

dphysics::CollisionObject::CollisionObject() : ysObject("CollisionObject") {
    m_primitiveHandle = nullptr;
    m_type = Type::Undefined;
    m_mode = Mode::Fine;

    m_relativePosition = ysMath::Constants::Zero;
    m_relativeOrientation = ysMath::LoadIdentity();

    m_collisionLayerMask = 0xFFFFFFFF;
    m_layer = 0x0;

    m_parent = nullptr;

    m_msg = 0;
}

dphysics::CollisionObject::CollisionObject(Type type) : ysObject("CollisionObject") {
    m_primitiveHandle = nullptr;
    m_type = type;
    m_mode = Mode::Fine;

    m_relativePosition = ysMath::Constants::Zero;
    m_relativeOrientation = ysMath::LoadIdentity();

    m_collisionLayerMask = 0xFFFFFFFF;
    m_layer = 0x0;

    m_parent = nullptr;

    m_msg = 0;
}

dphysics::CollisionObject::~CollisionObject() {
    /* void */
}

void dphysics::CollisionObject::GetBounds(ysVector &minPoint, ysVector &maxPoint) const {
    switch (m_type) {
    case Type::Box:
        GetAsBox()->GetBounds(minPoint, maxPoint);
        break;
    case Type::Circle:
        GetAsCircle()->GetBounds(minPoint, maxPoint);
        break;
    default:
        minPoint = ysMath::Constants::Zero;
        maxPoint = ysMath::Constants::Zero;
    }
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

void dphysics::CollisionObject::ConfigureRay() {
    RayPrimitive *prim = GetAsRay();

    ysMatrix orientation = ysMath::MatMult(m_relativeOrientation, m_parent->GetOrientationMatrix());
    prim->Position = ysMath::MatMult(m_parent->GetTransform(), ysMath::ExtendVector(m_relativePosition));
    prim->Direction = ysMath::MatMult(orientation, ysMath::ExtendVector(prim->RelativeDirection));
}

void dphysics::CollisionObject::ConfigurePrimitive() {
    switch (m_type) {
    case Type::Box:
        ConfigureBox();
        break;
    case Type::Circle:
        ConfigureCircle();
        break;
    case Type::Ray:
        ConfigureRay();
        break;
    }
}

void dphysics::CollisionObject::SetCollidesWith(int layer, bool collides) {
    if (layer < 0 || layer >= 32) return;

    m_collisionLayerMask &= ~(0x1 << layer);
    
    if (collides) m_collisionLayerMask |= (0x1 << layer);
}

bool dphysics::CollisionObject::CollidesWith(int layer) const {
    if (layer < 0 || layer >= 32) return false;

    int bit = 0x1 << layer;
    return (m_collisionLayerMask & bit) > 0;
}

void dphysics::CollisionObject::SetLayer(int layer) {
    m_layer = layer;
}

bool dphysics::CollisionObject::CheckCollisionMask(const CollisionObject *object) const {
    return CollidesWith(object->GetLayer()) || object->CollidesWith(GetLayer());
}
