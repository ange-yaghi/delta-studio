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

    if (ysMath::GetScalar(distSq) <= (circle1->RadiusSquared + circle2->RadiusSquared)) {
        return true;
    }

    return false;
}

bool dphysics::CollisionDetector::BoxBoxCollision(Collision &collision, RigidBody *body1, RigidBody *body2, BoxPrimitive *box1, BoxPrimitive *box2) {
    Collision collision1;
    Collision collision2;

    bool collision1Valid = false;
    bool collision2Valid = false;

    collision1Valid = _BoxBoxCollision(&collision1, box1, box2);
    collision2Valid = _BoxBoxCollision(&collision2, box2, box1);

    if (collision1Valid) {
        if (!collision2Valid || (collision2Valid && collision2.m_penetration >= collision1.m_penetration)) {
            collision.m_body1 = body1;
            collision.m_body2 = body2;
            collision.m_normal = collision1.m_normal;
            collision.m_penetration = collision1.m_penetration;
            collision.m_position = collision1.m_position;
        }
    }

    if (collision2Valid) {
        if (!collision1Valid || (collision1Valid && collision1.m_penetration >= collision2.m_penetration)) {
            collision.m_body1 = body2;
            collision.m_body2 = body1;
            collision.m_normal = collision2.m_normal;
            collision.m_penetration = collision2.m_penetration;
            collision.m_position = collision2.m_position;
        }
    }

    return (collision1Valid || collision2Valid);
}

bool dphysics::CollisionDetector::CircleBoxCollision(Collision &collision, RigidBody *body1, RigidBody *body2, CirclePrimitive *circle, BoxPrimitive *box) {
    ysVector relativePosition = ysMath::Sub(circle->Position, box->Position);
    relativePosition = ysMath::MatMult(ysMath::OrthogonalInverse(box->Orientation), relativePosition);

    float closestX = std::min(std::max(ysMath::GetX(relativePosition), -box->HalfWidth), box->HalfWidth);
    float closestY = std::min(std::max(ysMath::GetY(relativePosition), -box->HalfHeight), box->HalfHeight);

    ysVector closestPoint = ysMath::LoadVector(closestX, closestY, ysMath::GetZ(relativePosition));
    ysVector realPosition = ysMath::MatMult(box->Orientation, closestPoint);
    realPosition = ysMath::Add(realPosition, box->Position);
    
    float d2 = ysMath::GetScalar(ysMath::MagnitudeSquared3(ysMath::Sub(circle->Position, realPosition)));
    if (d2 > circle->RadiusSquared) return false;

    ysVector normal = ysMath::Mask(ysMath::Sub(circle->Position, realPosition), ysMath::Constants::MaskOffW);

    collision.m_normal = ysMath::Normalize(normal);
    collision.m_body1 = body1;
    collision.m_body2 = body2;
    collision.m_penetration = std::sqrt(circle->RadiusSquared) - ysMath::GetScalar(ysMath::Magnitude(normal));
    collision.m_position = realPosition;
    
    return true;
}

bool dphysics::CollisionDetector::CircleCircleCollision(Collision &collision, RigidBody *body1, RigidBody *body2, CirclePrimitive *circle1, CirclePrimitive *circle2) {
    ysVector delta = ysMath::Sub(circle2->Position, circle1->Position);
    ysVector direction = ysMath::Normalize(delta);
    ysVector distance = ysMath::Magnitude(delta);

    float s_distance = ysMath::GetScalar(distance);

    // Special case handling
    if (s_distance == 0) {
        direction = ysMath::Constants::XAxis;
        s_distance = 0.01f;
    }

    if ((s_distance * s_distance) > (circle1->RadiusSquared - circle2->RadiusSquared) ||
        (s_distance * s_distance) < (circle1->RadiusSquared + circle2->RadiusSquared)) 
    {
        collision.m_body1 = body1;
        collision.m_body2 = body2;
        collision.m_normal = ysMath::Negate3(direction);
        collision.m_penetration = sqrt(circle1->RadiusSquared) + sqrt(circle2->RadiusSquared) - s_distance;
        collision.m_position =
            ysMath::Add(circle1->Position, ysMath::Mul(direction, ysMath::LoadScalar(std::sqrt(circle1->RadiusSquared))));

        return true;
    }

    return false;
}

bool dphysics::CollisionDetector::RayBoxCollision(Collision &collision, RigidBody *body1, RigidBody *body2, RayPrimitive *ray, BoxPrimitive *box) {
    /* TODO */

    return false;
}

bool dphysics::CollisionDetector::RayCircleCollision(Collision &collision, RigidBody *body1, RigidBody *body2, RayPrimitive *ray, CirclePrimitive *circle) {
    ysVector D = ray->Direction;
    ysVector P = ray->Position;
    ysVector C = circle->Position;
    ysVector DP = ysMath::Sub(P, C);

    float D_dot_DP = ysMath::GetScalar(ysMath::Dot(D, DP));
    float D_mag = ysMath::GetScalar(ysMath::MagnitudeSquared3(D));
    float DP_mag = ysMath::GetScalar(ysMath::MagnitudeSquared3(DP));

    float delta = D_dot_DP * D_dot_DP - D_mag * (DP_mag - circle->RadiusSquared);
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
    else closest = std::min(t1, t2);

    collision.m_body1 = body1;
    collision.m_body2 = body2;
    collision.m_penetration = closest;
    collision.m_position = ray->Position;
    
    return true;
}

bool dphysics::CollisionDetector::_BoxBoxCollision(Collision *collision, BoxPrimitive *body1, BoxPrimitive *body2) {
    bool ret = false;

    ysVector point1 = ysMath::Add(ysMath::MatMult(body1->Orientation, ysMath::LoadVector(body1->HalfWidth, body1->HalfHeight)), body1->Position);
    ysVector point2 = ysMath::Add(ysMath::MatMult(body1->Orientation, ysMath::LoadVector(-body1->HalfWidth, body1->HalfHeight)), body1->Position);
    ysVector point3 = ysMath::Add(ysMath::MatMult(body1->Orientation, ysMath::LoadVector(body1->HalfWidth, -body1->HalfHeight)), body1->Position);
    ysVector point4 = ysMath::Add(ysMath::MatMult(body1->Orientation, ysMath::LoadVector(-body1->HalfWidth, -body1->HalfHeight)), body1->Position);

    ysMatrix invBody2 = ysMath::Transpose(body2->Orientation);
    ysMatrix final = ysMath::MatMult(invBody2, body1->Orientation);

    ysVector edgeNormal_1 = ysMath::MatMult(final, ysMath::LoadVector(1.0f, 0.0f));
    ysVector edgeNormal_2 = ysMath::MatMult(final, ysMath::LoadVector(-1.0f, 0.0f));
    ysVector edgeNormal_3 = ysMath::MatMult(final, ysMath::LoadVector(0.0f, 1.0f));
    ysVector edgeNormal_4 = ysMath::MatMult(final, ysMath::LoadVector(0.0f, -1.0f));

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
    xaxis = ysMath::MatMult(body2->Orientation, xaxis);
    yaxis = ysMath::MatMult(body2->Orientation, yaxis);

    ysVector del = ysMath::Sub(body2->Position, body1->Position);
    ysVector delDir = ysMath::Normalize(del);

    for (int i = 0; i < 4; i++) {
        ysVector relPoint = ysMath::Sub(points[i], body2->Position);

        ysVector projx = ysMath::Dot(relPoint, xaxis);
        ysVector projy = ysMath::Dot(relPoint, yaxis);

        penetrationX[i] = ysMath::GetScalar(projx);
        penetrationY[i] = ysMath::GetScalar(projy);
    }

    Collision &retcol = *collision;
    float smallestPenetration = FLT_MAX;

    for (int i = 0; i < 4; i++) {
        float norX_1 = ysMath::GetX(normals_1[i]);
        float norY_1 = ysMath::GetY(normals_1[i]);
        float norX_2 = ysMath::GetX(normals_2[i]);
        float norY_2 = ysMath::GetY(normals_2[i]);

        if (abs(penetrationX[i]) <= body2->HalfWidth && abs(penetrationY[i]) <= body2->HalfHeight) {
            float actualPenetrationX = body2->HalfWidth - abs(penetrationX[i]);
            float actualPenetrationY = body2->HalfHeight - abs(penetrationY[i]);

            if (actualPenetrationX < smallestPenetration) {
                if (norX_2 >= THRESH_0_NEGATIVE && norX_1 >= THRESH_0_NEGATIVE && penetrationX[i] < 0) {
                    smallestPenetration = actualPenetrationX;
                    ret = true;

                    retcol.m_normal = ysMath::Negate(xaxis);
                    retcol.m_penetration = smallestPenetration;
                    retcol.m_position = points[i];
                }
                else if (norX_2 <= THRESH_0_POSITIVE && norX_1 <= THRESH_0_POSITIVE && penetrationX[i] > 0) {
                    smallestPenetration = actualPenetrationX;
                    ret = true;

                    retcol.m_normal = xaxis;
                    retcol.m_penetration = smallestPenetration;
                    retcol.m_position = points[i];
                }
            }

            if (actualPenetrationY < smallestPenetration) {
                if (norY_2 >= THRESH_0_NEGATIVE && norY_1 >= THRESH_0_NEGATIVE && penetrationY[i] < 0) {
                    smallestPenetration = actualPenetrationY;
                    ret = true;

                    retcol.m_normal = ysMath::Negate(yaxis);
                    retcol.m_penetration = smallestPenetration;
                    retcol.m_position = points[i];
                }
                else if (norY_2 <= THRESH_0_POSITIVE && norY_1 <= THRESH_0_POSITIVE && penetrationY[i] > 0) {
                    smallestPenetration = actualPenetrationY;
                    ret = true;

                    retcol.m_normal = yaxis;
                    retcol.m_penetration = smallestPenetration;
                    retcol.m_position = points[i];
                }
            }
        }
    }

    return ret;
}

bool dphysics::CollisionDetector::_BoxBoxEdgeDetect(Collision *collisions, BoxPrimitive *body1, BoxPrimitive *body2) {
    bool ret = false;

    ysVector point1 = ysMath::Add(ysMath::MatMult(body1->Orientation, ysMath::LoadVector(body1->HalfWidth, body1->HalfHeight)), body1->Position);
    ysVector point2 = ysMath::Add(ysMath::MatMult(body1->Orientation, ysMath::LoadVector(-body1->HalfWidth, body1->HalfHeight)), body1->Position);
    ysVector point3 = ysMath::Add(ysMath::MatMult(body1->Orientation, ysMath::LoadVector(body1->HalfWidth, -body1->HalfHeight)), body1->Position);
    ysVector point4 = ysMath::Add(ysMath::MatMult(body1->Orientation, ysMath::LoadVector(-body1->HalfWidth, -body1->HalfHeight)), body1->Position);

    ysVector edgeNormal_1 = ysMath::MatMult(body1->Orientation, ysMath::LoadVector(1.0f, 0.0f));
    ysVector edgeNormal_2 = ysMath::Negate(edgeNormal_1);
    ysVector edgeNormal_3 = ysMath::MatMult(body1->Orientation, ysMath::LoadVector(0.0f, 1.0f));
    ysVector edgeNormal_4 = ysMath::Negate(edgeNormal_3);

    ysVector normal1_1 = edgeNormal_1;
    ysVector normal2_1 = edgeNormal_2;
    ysVector normal3_1 = edgeNormal_1;
    ysVector normal4_1 = edgeNormal_2;

    ysVector normal1_2 = edgeNormal_3;
    ysVector normal2_2 = edgeNormal_3;
    ysVector normal3_2 = edgeNormal_4;
    ysVector normal4_2 = edgeNormal_4;

    ysVector points[] = { point1, point2, point3, point4 };
    int edges[][2] = { {0, 2}, {1, 3}, {0, 1}, {2, 3} };
    ysVector edgeNormals[] = { edgeNormal_1, edgeNormal_2, edgeNormal_3, edgeNormal_4 };
    ysVector normals_1[] = { normal1_1, normal2_1, normal3_1, normal4_1 };
    ysVector normals_2[] = { normal1_2, normal2_2, normal3_2, normal4_2 };

    float penetrationX[4];
    float penetrationY[4];

    ysVector xaxis = ysMath::Constants::XAxis;
    ysVector yaxis = ysMath::Constants::YAxis;
    xaxis = ysMath::MatMult(body2->Orientation, xaxis);
    yaxis = ysMath::MatMult(body2->Orientation, yaxis);

    ysVector del = ysMath::Sub(body2->Position, body1->Position);
    ysVector delDir = ysMath::Normalize(del);

    int numCollisions = 0;

    for (int i = 0; i < 4; i++) {
        ysVector relPoint = ysMath::Sub(points[i], body2->Position);

        ysVector projx = ysMath::Dot(relPoint, xaxis);
        ysVector projy = ysMath::Dot(relPoint, yaxis);

        penetrationX[i] = ysMath::GetScalar(projx);
        penetrationY[i] = ysMath::GetScalar(projy);
    }

    float smallestPenetration = FLT_MAX;
    for (int i = 0; i < 4; i++) {
        float norX_1 = ysMath::GetX(normals_1[i]);
        float norY_1 = ysMath::GetY(normals_1[i]);
        float norX_2 = ysMath::GetX(normals_2[i]);
        float norY_2 = ysMath::GetY(normals_2[i]);

        int *edge = edges[i];

        if ((abs(penetrationX[edge[0]]) <= body2->HalfWidth && abs(penetrationX[edge[1]]) <= body2->HalfWidth) &&
            (std::min(penetrationY[edge[0]], penetrationY[edge[1]]) <= body2->HalfHeight) &&
            (std::max(penetrationY[edge[0]], penetrationY[edge[1]]) >= -body2->HalfHeight)) {

            float actualPenetrationX0 = body2->HalfWidth - abs(penetrationX[edge[0]]);
            float actualPenetrationX1 = body2->HalfWidth - abs(penetrationX[edge[1]]);
            float actualPenetrationX = (actualPenetrationX0 < actualPenetrationX1) ? actualPenetrationX0 : actualPenetrationX1;

            if (actualPenetrationX < smallestPenetration) {
                numCollisions = 0;
                if (abs(penetrationY[edge[0]]) <= body2->HalfHeight) {
                    collisions[numCollisions].m_penetration = actualPenetrationX0;
                    collisions[numCollisions].m_position = points[edge[0]];
                    collisions[numCollisions].m_normal = ysMath::Negate(edgeNormals[i]);
                    numCollisions++;
                }

                if (abs(penetrationY[edge[1]]) <= body2->HalfHeight) {
                    collisions[numCollisions].m_penetration = actualPenetrationX1;
                    collisions[numCollisions].m_position = points[edge[1]];
                    collisions[numCollisions].m_normal = ysMath::Negate(edgeNormals[i]);
                    numCollisions++;
                }
                smallestPenetration = actualPenetrationX;
            }

            ret = true;
        }

        if ((abs(penetrationY[edge[0]]) <= body2->HalfHeight && abs(penetrationY[edge[1]]) <= body2->HalfHeight) &&
            (std::min(penetrationX[edge[0]], penetrationX[edge[1]]) <= body2->HalfWidth) &&
            (std::max(penetrationX[edge[0]], penetrationX[edge[1]]) >= -body2->HalfWidth)) {

            float actualPenetrationY0 = body2->HalfHeight - abs(penetrationY[edge[0]]);
            float actualPenetrationY1 = body2->HalfHeight - abs(penetrationY[edge[1]]);
            float actualPenetrationY = (actualPenetrationY0 < actualPenetrationY1) ? actualPenetrationY0 : actualPenetrationY1;

            if (actualPenetrationY < smallestPenetration) {
                numCollisions = 0;
                if (abs(penetrationX[edge[0]]) <= body2->HalfWidth) {
                    collisions[numCollisions].m_penetration = actualPenetrationY0;
                    collisions[numCollisions].m_position = points[edge[0]];
                    collisions[numCollisions].m_normal = ysMath::Negate(edgeNormals[i]);
                    numCollisions++;
                }

                if (abs(penetrationX[edge[1]]) <= body2->HalfWidth) {
                    collisions[numCollisions].m_penetration = actualPenetrationY1;
                    collisions[numCollisions].m_position = points[edge[1]];
                    collisions[numCollisions].m_normal = ysMath::Negate(edgeNormals[i]);
                    numCollisions++;
                }
                smallestPenetration = actualPenetrationY;
            }

            ret = true;
        }
    }

    return ret;
}
