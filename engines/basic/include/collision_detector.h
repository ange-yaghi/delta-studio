#ifndef DELTA_BASIC_COLLISION_DETECTOR_H
#define DELTA_BASIC_COLLISION_DETECTOR_H

#include "collision_primitives.h"
#include "rigid_body.h"

namespace dbasic {

    class CollisionDetector {
    public:
        CollisionDetector();
        ~CollisionDetector();

        bool BoxBoxCollision(Collision &collision, RigidBody *body1, RigidBody *body2, BoxPrimitive *box1, BoxPrimitive *box2);
        bool CircleCircleCollision(Collision &collision, RigidBody *body1, RigidBody *body2, CirclePrimitive *circle1, CirclePrimitive *circle2);

        bool CircleCircleIntersect(RigidBody *body1, RigidBody *body2, CirclePrimitive *circle1, CirclePrimitive *circle2);

    protected:
        bool _BoxBoxCollision(Collision *collision, BoxPrimitive *body1, BoxPrimitive *body2);
        bool _BoxBoxEdgeDetect(Collision *collisions, BoxPrimitive *body1, BoxPrimitive *body2);
    };

} /* namesapce dbasic */

#endif /* DELTA_BASIC_COLLISION_DETECTOR_H */
