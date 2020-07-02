#include "../include/ledge_link.h"

#include "../include/collision_primitives.h"
#include "../include/rigid_body.h"

dphysics::LedgeLink::LedgeLink() {
    m_anchorPoint = ysMath::Constants::Zero;
    m_gripLocal = ysMath::Constants::Zero;
}

dphysics::LedgeLink::~LedgeLink() {
    /* void */
}

int dphysics::LedgeLink::GenerateCollisions(Collision *collisionArray) {
    ysVector gripWorld = m_body1->Transform.LocalToWorldSpace(m_gripLocal);

    float gy, ay;
    gy = ysMath::GetY(gripWorld);
    ay = ysMath::GetY(m_anchorPoint);

    if (gy > ay) return 0;

    ysVector delta = ysMath::LoadVector(0.0f, ay - gy, 0.0f);
    ysVector normal = ysMath::Constants::YAxis;

    float penetration = ay - gy;

    Collision &retcol = collisionArray[0];
    retcol.m_body1 = m_body1;
    retcol.m_body2 = m_body2;
    retcol.m_normal = normal;
    retcol.m_penetration = penetration;
    retcol.m_position = m_anchorPoint;
    retcol.m_sensor = false;
    retcol.m_collisionObject1 = nullptr;
    retcol.m_collisionObject2 = nullptr;
    retcol.m_restitution = 0.0f;
    retcol.m_staticFriction = 100.0f;
    retcol.m_dynamicFriction = 100.0f;

    return 1;
}
