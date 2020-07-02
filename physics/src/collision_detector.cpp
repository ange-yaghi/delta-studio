#include "../include/collision_detector.h"

#include <float.h>
#include <math.h>
#include <algorithm>

#define THRESH_0_POSITIVE 10e-9
#define THRESH_0_NEGATIVE (-THRESH_0_POSITIVE)

dphysics::CollisionDetector::CollisionDetector() {
    /* void */
}

dphysics::CollisionDetector::~CollisionDetector() {
    /* void */
}

bool dphysics::CollisionDetector::CircleCircleIntersect(RigidBody *body1, RigidBody *body2, CirclePrimitive *circle1, CirclePrimitive *circle2) {
    ysVector dif = ysMath::Sub(circle1->Position, circle2->Position);
    ysVector distSq = ysMath::MagnitudeSquared3(dif);

    float combinedRadius = circle1->Radius + circle2->Radius;

    if (ysMath::GetScalar(distSq) <= (combinedRadius * combinedRadius)) {
        return true;
    }

    return false;
}

int dphysics::CollisionDetector::BoxBoxCollision(Collision *collisions, RigidBody *body1, RigidBody *body2, BoxPrimitive *box1, BoxPrimitive *box2) {
    Collision collisions1[2];
    Collision collisions2[2];

    int n1 = _BoxBoxCollision(collisions1, box1, box2);
    int n2 = _BoxBoxCollision(collisions2, box2, box1);

    float minPenetration1 = (n1 > 0)
        ? collisions1->m_penetration
        : FLT_MAX;
    float minPenetration2 = (n2 > 0)
        ? collisions2->m_penetration
        : FLT_MAX;

    int n = 0;
    if (n1 > 0) {
        if (minPenetration2 >= minPenetration1) {
            for (int i = 0; i < n1; ++i) {
                collisions[i] = collisions1[i];
                collisions[i].m_body1 = body1;
                collisions[i].m_body2 = body2;
            }
            n = n1;
        }
    }

    if (n2 > 0) {
        if (minPenetration2 <= minPenetration1) {
            for (int i = 0; i < n2; ++i) {
                collisions[i] = collisions2[i];
                collisions[i].m_body1 = body2;
                collisions[i].m_body2 = body1;
            }
            n = n2;
        }
    }

    return n;
}

int dphysics::CollisionDetector::CircleBoxCollision(Collision *collisions, RigidBody *body1, RigidBody *body2, CirclePrimitive *circle, BoxPrimitive *box) {
    constexpr float epsilon = 1E-5f;

    ysVector relativePosition = ysMath::Sub(circle->Position, box->Position);
    relativePosition = ysMath::QuatTransformInverse(box->Orientation, relativePosition);

    float closestX = min(max(ysMath::GetX(relativePosition), -box->HalfWidth), box->HalfWidth);
    float closestY = min(max(ysMath::GetY(relativePosition), -box->HalfHeight), box->HalfHeight);

    ysVector closestPoint = ysMath::LoadVector(closestX, closestY, ysMath::GetZ(relativePosition));
    ysVector realPosition = ysMath::QuatTransform(box->Orientation, closestPoint);
    realPosition = ysMath::Add(realPosition, box->Position);
    
    float d0 = ysMath::GetScalar(ysMath::MagnitudeSquared3(ysMath::Sub(circle->Position, box->Position)));
    float d2 = ysMath::GetScalar(ysMath::MagnitudeSquared3(ysMath::Sub(circle->Position, realPosition)));
    if (d2 > circle->Radius * circle->Radius) return 0;

    ysVector normal;
    if (d0 <= epsilon) {
        normal = ysMath::Mul(ysMath::Constants::XAxis, ysMath::LoadScalar(0.001f));
    }
    else if (d2 <= epsilon) {
        normal = ysMath::Mask(ysMath::Sub(circle->Position, box->Position), ysMath::Constants::MaskOffW);
        normal = ysMath::Mask(normal, ysMath::Constants::MaskOffZ);
    }
    else {
        normal = ysMath::Mask(ysMath::Sub(circle->Position, realPosition), ysMath::Constants::MaskOffW);
        normal = ysMath::Mask(normal, ysMath::Constants::MaskOffZ);
    }

    collisions[0].m_normal = ysMath::Normalize(normal);
    collisions[0].m_body1 = body1;
    collisions[0].m_body2 = body2;
    collisions[0].m_penetration = circle->Radius - ysMath::GetScalar(ysMath::Magnitude(normal));
    collisions[0].m_position = realPosition;
    collisions[0].m_collisionType = Collision::CollisionType::Generic;
    
    return 1;
}

int dphysics::CollisionDetector::CircleCircleCollision(Collision *collisions, RigidBody *body1, RigidBody *body2, CirclePrimitive *circle1, CirclePrimitive *circle2) {
    ysVector delta = ysMath::Sub(circle2->Position, circle1->Position);
    delta = ysMath::Mask(delta, ysMath::Constants::MaskOffZ);

    ysVector direction = ysMath::Normalize(delta);
    ysVector distance = ysMath::Magnitude(delta);

    float s_distance = ysMath::GetScalar(distance);

    // Special case handling
    if (s_distance == 0) {
        direction = ysMath::Constants::XAxis;
        s_distance = 0.01f;
    }

    float combinedRadius = circle1->Radius + circle2->Radius;

    if ((s_distance * s_distance) < combinedRadius * combinedRadius) {
        collisions[0].m_body1 = body1;
        collisions[0].m_body2 = body2;
        collisions[0].m_normal = ysMath::Negate3(direction);
        collisions[0].m_penetration = circle1->Radius + circle2->Radius - s_distance;
        collisions[0].m_position =
            ysMath::Add(circle1->Position, ysMath::Mul(direction, ysMath::LoadScalar(circle1->Radius)));
        collisions[0].m_collisionType = Collision::CollisionType::Generic;

        return true;
    }

    return false;
}

int dphysics::CollisionDetector::RayBoxCollision(Collision *collisions, RigidBody *body1, RigidBody *body2, RayPrimitive *ray, BoxPrimitive *box) {
    /* TODO */

    return 0;
}

int dphysics::CollisionDetector::RayCircleCollision(Collision *collisions, RigidBody *body1, RigidBody *body2, RayPrimitive *ray, CirclePrimitive *circle) {
    ysVector D = ray->Direction;
    ysVector P = ray->Position;
    ysVector C = circle->Position;
    ysVector DP = ysMath::Sub(P, C);

    float D_dot_DP = ysMath::GetScalar(ysMath::Dot(D, DP));
    float D_mag = ysMath::GetScalar(ysMath::MagnitudeSquared3(D));
    float DP_mag = ysMath::GetScalar(ysMath::MagnitudeSquared3(DP));

    float delta = D_dot_DP * D_dot_DP - D_mag * (DP_mag - circle->Radius * circle->Radius);
    if (delta < 0) return false;

    float t1 = (-D_dot_DP + delta) / D_mag;
    float t2 = (-D_dot_DP - delta) / D_mag;

    if (ray->MaxDistance > 0) {
        if (t1 > ray->MaxDistance && t2 > ray->MaxDistance) return false;
    }

    float closest = 0;
    if (t1 < 0 && t2 < 0) return false;
    else if (t1 < 0) closest = t2;
    else if (t2 < 0) closest = t1;
    else closest = min(t1, t2);

    collisions[0].m_body1 = body1;
    collisions[0].m_body2 = body2;
    collisions[0].m_penetration = closest;
    collisions[0].m_position = ray->Position;
    
    return true;
}

int dphysics::CollisionDetector::_BoxBoxCollision(Collision *collisions, BoxPrimitive *body1, BoxPrimitive *body2) {
    ysVector point1 = ysMath::Add(ysMath::QuatTransform(body1->Orientation, ysMath::LoadVector(body1->HalfWidth, body1->HalfHeight)), body1->Position);
    ysVector point2 = ysMath::Add(ysMath::QuatTransform(body1->Orientation, ysMath::LoadVector(-body1->HalfWidth, body1->HalfHeight)), body1->Position);
    ysVector point3 = ysMath::Add(ysMath::QuatTransform(body1->Orientation, ysMath::LoadVector(body1->HalfWidth, -body1->HalfHeight)), body1->Position);
    ysVector point4 = ysMath::Add(ysMath::QuatTransform(body1->Orientation, ysMath::LoadVector(-body1->HalfWidth, -body1->HalfHeight)), body1->Position);

    ysQuaternion invBody2 = ysMath::QuatInvert(body2->Orientation);
    ysQuaternion final = ysMath::QuatMultiply(invBody2, body1->Orientation);

    ysVector edgeNormal_1 = ysMath::QuatTransform(final, ysMath::LoadVector(1.0f, 0.0f));
    ysVector edgeNormal_2 = ysMath::QuatTransform(final, ysMath::LoadVector(-1.0f, 0.0f));
    ysVector edgeNormal_3 = ysMath::QuatTransform(final, ysMath::LoadVector(0.0f, 1.0f));
    ysVector edgeNormal_4 = ysMath::QuatTransform(final, ysMath::LoadVector(0.0f, -1.0f));

    ysVector normal1_1 = edgeNormal_1;
    ysVector normal2_1 = edgeNormal_2;
    ysVector normal3_1 = edgeNormal_1;
    ysVector normal4_1 = edgeNormal_2;

    ysVector normal1_2 = edgeNormal_3;
    ysVector normal2_2 = edgeNormal_3;
    ysVector normal3_2 = edgeNormal_4;
    ysVector normal4_2 = edgeNormal_4;

    ysVector points[] = { point1, point2, point3, point4 };
    ysVector normals_1[] = { normal1_1, normal2_1, normal3_1, normal4_1 };
    ysVector normals_2[] = { normal1_2, normal2_2, normal3_2, normal4_2 };

    float penetrationX[4];
    float penetrationY[4];

    ysVector xaxis = ysMath::Constants::XAxis;
    ysVector yaxis = ysMath::Constants::YAxis;
    xaxis = ysMath::QuatTransform(body2->Orientation, xaxis);
    yaxis = ysMath::QuatTransform(body2->Orientation, yaxis);

    ysVector del = ysMath::Sub(body2->Position, body1->Position);
    ysVector delDir = ysMath::Normalize(del);

    for (int i = 0; i < 4; i++) {
        ysVector relPoint = ysMath::Sub(points[i], body2->Position);

        ysVector projx = ysMath::Dot(relPoint, xaxis);
        ysVector projy = ysMath::Dot(relPoint, yaxis);

        penetrationX[i] = ysMath::GetScalar(projx);
        penetrationY[i] = ysMath::GetScalar(projy);
    }

    float smallestPenetration = FLT_MAX;
    Collision rawCollisions[4];

    for (int i = 0; i < 4; i++) {
        float norX_1 = ysMath::GetX(normals_1[i]);
        float norY_1 = ysMath::GetY(normals_1[i]);
        float norX_2 = ysMath::GetX(normals_2[i]);
        float norY_2 = ysMath::GetY(normals_2[i]);

        Collision &col = rawCollisions[i];
        rawCollisions[i].m_collisionType = Collision::CollisionType::Unknown;

        if (abs(penetrationX[i]) <= body2->HalfWidth && abs(penetrationY[i]) <= body2->HalfHeight) {
            float actualPenetrationX = body2->HalfWidth - abs(penetrationX[i]);
            float actualPenetrationY = body2->HalfHeight - abs(penetrationY[i]);

            bool foundValid = false;
            if (norX_2 >= THRESH_0_NEGATIVE && norX_1 >= THRESH_0_NEGATIVE && penetrationX[i] < 0) {
                smallestPenetration = min(actualPenetrationX, smallestPenetration);

                col.m_normal = ysMath::Negate(xaxis);
                col.m_penetration = actualPenetrationX;
                col.m_position = points[i];
                col.m_collisionType = Collision::CollisionType::PointFace;
                col.m_feature1 = i;
                col.m_feature2 = -1;

                foundValid = true;
            }
            else if (norX_2 <= THRESH_0_POSITIVE && norX_1 <= THRESH_0_POSITIVE && penetrationX[i] > 0) {
                smallestPenetration = min(actualPenetrationX, smallestPenetration);

                col.m_normal = xaxis;
                col.m_penetration = actualPenetrationX;
                col.m_position = points[i];
                col.m_collisionType = Collision::CollisionType::PointFace;
                col.m_feature1 = i;
                col.m_feature2 = -1;

                foundValid = true;
            }
            if (actualPenetrationY < actualPenetrationX || !foundValid) {
                if (norY_2 >= THRESH_0_NEGATIVE && norY_1 >= THRESH_0_NEGATIVE && penetrationY[i] < 0) {
                    smallestPenetration = min(actualPenetrationY, smallestPenetration);

                    col.m_normal = ysMath::Negate(yaxis);
                    col.m_penetration = actualPenetrationY;
                    col.m_position = points[i];
                    col.m_collisionType = Collision::CollisionType::PointFace;
                    col.m_feature1 = i;
                    col.m_feature2 = -1;
                }
                else if (norY_2 <= THRESH_0_POSITIVE && norY_1 <= THRESH_0_POSITIVE && penetrationY[i] > 0) {
                    smallestPenetration = min(actualPenetrationY, smallestPenetration);

                    col.m_normal = yaxis;
                    col.m_penetration = actualPenetrationY;
                    col.m_position = points[i];
                    col.m_collisionType = Collision::CollisionType::PointFace;
                    col.m_feature1 = i;
                    col.m_feature2 = -1;
                }
            }
        }
    }

    int nCollisions = 0;
    for (int i = 0; i < 4; ++i) {
        if (nCollisions >= 2) break;
        if (rawCollisions[i].m_collisionType == Collision::CollisionType::Unknown) continue;

        if (std::abs(rawCollisions[i].m_penetration - smallestPenetration) < 1E-4) {
            collisions[nCollisions] = rawCollisions[i];
            ++nCollisions;
        }
    }

    return nCollisions;
}
