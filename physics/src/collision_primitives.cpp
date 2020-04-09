#include "../include/collision_primitives.h"

#include "../include/rigid_body.h"

#include <algorithm>
#include <stdlib.h>

dphysics::Collision::Collision() : ysObject("Collision") {
    m_penetration = 0.0f;
    m_normal = ysMath::Constants::Zero;
    m_body1 = nullptr;
    m_body2 = nullptr;

    m_collisionObject1 = nullptr;
    m_collisionObject2 = nullptr;

    m_sensor = false;
}

dphysics::Collision::Collision(Collision &collision) : ysObject("Collision") {
    m_penetration = collision.m_penetration;
    m_normal = collision.m_normal;
    m_position = collision.m_position;

    m_body1 = collision.m_body1;
    m_body2 = collision.m_body2;

    m_collisionObject1 = collision.m_collisionObject1;
    m_collisionObject2 = collision.m_collisionObject2;

    m_sensor = collision.m_sensor;

    m_relativePosition[0] = collision.m_relativePosition[0];
    m_relativePosition[1] = collision.m_relativePosition[1];
}

dphysics::Collision::~Collision() {
    /* void */
}

void dphysics::Collision::UpdateInternals() {
    for (int i = 0; i < 2; i++) {
        if (m_bodies[i] != nullptr) {
            m_relativePosition[i] = ysMath::Sub(m_position, m_bodies[i]->GetPosition());
        }
    }
}

dphysics::Collision &dphysics::Collision::operator=(dphysics::Collision &collision) {
    m_penetration = collision.m_penetration;
    m_normal = collision.m_normal;
    m_position = collision.m_position;

    m_body1 = collision.m_body1;
    m_body2 = collision.m_body2;

    m_collisionObject1 = collision.m_collisionObject1;
    m_collisionObject2 = collision.m_collisionObject2;

    m_sensor = collision.m_sensor;

    m_relativePosition[0] = collision.m_relativePosition[0];
    m_relativePosition[1] = collision.m_relativePosition[1];

    return *this;
}

bool dphysics::Collision::IsGhost() const {
    if (m_collisionObject1 != nullptr && m_collisionObject1->GetParent()->IsGhost()) return true;
    if (m_collisionObject2 != nullptr && m_collisionObject2->GetParent()->IsGhost()) return true;
    return false;
}

void dphysics::BoxPrimitive::GetBounds(ysVector &minPoint, ysVector &maxPoint) const {
    ysVector point1 = ysMath::Add(ysMath::MatMult(Orientation, ysMath::LoadVector(HalfWidth, HalfHeight)), Position);
    ysVector point2 = ysMath::Add(ysMath::MatMult(Orientation, ysMath::LoadVector(-HalfWidth, HalfHeight)), Position);
    ysVector point3 = ysMath::Add(ysMath::MatMult(Orientation, ysMath::LoadVector(HalfWidth, -HalfHeight)), Position);
    ysVector point4 = ysMath::Add(ysMath::MatMult(Orientation, ysMath::LoadVector(-HalfWidth, -HalfHeight)), Position);

    maxPoint = ysMath::ComponentMax(point1, point2);
    maxPoint = ysMath::ComponentMax(maxPoint, point3);
    maxPoint = ysMath::ComponentMax(maxPoint, point4);

    minPoint = ysMath::ComponentMin(point1, point2);
    minPoint = ysMath::ComponentMin(minPoint, point3);
    minPoint = ysMath::ComponentMin(minPoint, point4);
}

void dphysics::CirclePrimitive::GetBounds(ysVector &minPoint, ysVector &maxPoint) const {
    maxPoint = ysMath::Add(Position, ysMath::LoadVector(Radius, Radius));
    minPoint = ysMath::Add(Position, ysMath::LoadVector(-Radius, -Radius));
}
