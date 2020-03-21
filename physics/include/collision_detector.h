#ifndef DELTA_BASIC_COLLISION_DETECTOR_H
#define DELTA_BASIC_COLLISION_DETECTOR_H

#include "collision_primitives.h"
#include "rigid_body.h"

namespace dphysics {

    class CollisionDetector {
    public:
        CollisionDetector();
        ~CollisionDetector();

        bool BoxBoxCollision(Collision &collision, RigidBody *body1, RigidBody *body2, BoxPrimitive *box1, BoxPrimitive *box2);
        bool CircleBoxCollision(Collision &collision, RigidBody *body1, RigidBody *body2, CirclePrimitive *circle, BoxPrimitive *box);
        bool CircleCircleCollision(Collision &collision, RigidBody *body1, RigidBody *body2, CirclePrimitive *circle1, CirclePrimitive *circle2);
        bool RayBoxCollision(Collision &collision, RigidBody *body1, RigidBody *body2, RayPrimitive *ray, BoxPrimitive *box);
        bool RayCircleCollision(Collision &collision, RigidBody *body1, RigidBody *body2, RayPrimitive *ray, CirclePrimitive *circle);

        bool CircleCircleIntersect(RigidBody *body1, RigidBody *body2, CirclePrimitive *circle1, CirclePrimitive *circle2);

    protected:
        bool _BoxBoxCollision(Collision *collision, BoxPrimitive *body1, BoxPrimitive *body2);
        bool _BoxBoxEdgeDetect(Collision *collisions, BoxPrimitive *body1, BoxPrimitive *body2);
    };

} /* namesapce dbasic */

#endif /* DELTA_BASIC_COLLISION_DETECTOR_H */
