#include "../include/collision_primitives.h"
#include "../include/rigid_body.h"

#include <stdlib.h>

dbasic::Collision::Collision() : ysObject("Collision") {
    m_penetration = 0.0f;
    m_normal = ysMath::Constants::Zero;
    m_body1 = NULL;
    m_body2 = NULL;

    m_collisionObject1 = NULL;
    m_collisionObject2 = NULL;

    m_sensor = false;
}

dbasic::Collision::Collision(Collision &collision) : ysObject("Collision") {
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

dbasic::Collision::~Collision() {
    /* void */
}

void dbasic::Collision::UpdateInternals() {
    for (int i = 0; i < 2; i++) {
        if (m_bodies[i] != NULL) {
            m_relativePosition[i] = ysMath::Sub(m_position, m_bodies[i]->GetPosition());
        }
    }
}

dbasic::Collision &dbasic::Collision::operator=(dbasic::Collision &collision) {
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
