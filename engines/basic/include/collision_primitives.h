#ifndef DELTA_BASIC_COLLISION_PRIMITIVES_H
#define DELTA_BASIC_COLLISION_PRIMITIVES_H

#include "delta_core.h"

namespace dbasic {

    struct BoxPrimitive {
        ysMatrix Orientation;
        ysVector Position;
        float HalfWidth;
        float HalfHeight;
    };

    struct CirclePrimitive {
        ysVector Position;
        float RadiusSquared;
    };

    class RigidBody;
    class CollisionObject;
    class RigidBodySystem;

    class Collision : public ysObject {
        friend RigidBodySystem;

    public:
        Collision();
        Collision(Collision &collision);
        ~Collision();

        float m_penetration;
        ysVector m_normal;
        ysVector m_position;

        union {
            struct {
                RigidBody *m_body1;
                RigidBody *m_body2;
            };

            RigidBody *m_bodies[2];
        };

        union {
            struct {
                CollisionObject *m_collisionObject1;
                CollisionObject *m_collisionObject2;
            };

            CollisionObject *m_collisionObjects[2];
        };

        bool m_sensor;

        void UpdateInternals();

        Collision &operator=(Collision &collision);

    protected:

        ysVector m_relativePosition[2];
    };

} /* namespace dbasic */

#endif /* DELTA_BASIC_COLLISION_PRIMITIVES_H */
