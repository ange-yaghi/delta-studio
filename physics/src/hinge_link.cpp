#include "../include/hinge_link.h"

#include "../include/rigid_body.h"

dphysics::HingeLink::HingeLink() {
    m_relativePos1 = ysMath::Constants::Zero;
    m_relativePos2 = ysMath::Constants::Zero;
}

dphysics::HingeLink::~HingeLink() {
    /* void */
}

int dphysics::HingeLink::GenerateCollisions(Collision *collisionArray) {
    ysVector actualPosition1 = m_body1->Transform.LocalToWorldSpace(m_relativePos1);
    ysVector actualPosition2 = m_body2->Transform.LocalToWorldSpace(m_relativePos2);

    ysVector delta = ysMath::Sub(actualPosition2, actualPosition1);
    ysVector length = ysMath::Magnitude(delta);
    ysVector normal = ysMath::Div(delta, length);

    float penetration = ysMath::GetScalar(length);

    Collision &retcol = collisionArray[0];
    retcol.m_body1 = m_body1;
    retcol.m_body2 = m_body2;
    retcol.m_normal = normal;
    retcol.m_penetration = penetration;
    retcol.m_position = actualPosition1;
    retcol.m_sensor = false;
    retcol.m_collisionObject1 = nullptr;
    retcol.m_collisionObject2 = nullptr;
    retcol.m_restitution = 0.0f;
    retcol.m_staticFriction = 10.0f;
    retcol.m_dynamicFriction = 10.0f;

    return 1;
}
