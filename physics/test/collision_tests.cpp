#include <pch.h>

#include "../include/delta_physics.h"
#include "utilities.h"

TEST(DeltaPhysicsTest, SanityCheck) {
    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);
}

TEST(CollisionTests, CircleCircleSanityCheck) {
    dphysics::CirclePrimitive c1;
    dphysics::CirclePrimitive c2;

    c1.Position = ysMath::LoadVector(1.0f, 0.0f, 0.0f, 0.0f);
    c1.Radius = 1.0f;

    c2.Position = ysMath::LoadVector(1.5f, 0.0f, 0.0f, 0.0f);
    c2.Radius = 1.0f;

    dphysics::CollisionDetector detector;
    dphysics::Collision collision;
    bool colliding = detector.CircleCircleCollision(collision, nullptr, nullptr, &c1, &c2);

    EXPECT_TRUE(colliding);
    EXPECT_NEAR(collision.m_penetration, 1.5f, 1E-4);

    VecEq(collision.m_position, ysMath::LoadVector(2.0f, 0.0f, 0.0f, 0.0f));
}

TEST(CollisionTests, CircleCircleNotColliding) {
    dphysics::CirclePrimitive c1;
    dphysics::CirclePrimitive c2;

    c1.Position = ysMath::LoadVector(1.0f, 0.0f, 0.0f, 0.0f);
    c1.Radius = 1.0f;

    c2.Position = ysMath::LoadVector(5.5f, 0.0f, 0.0f, 0.0f);
    c2.Radius = 1.0f;

    dphysics::CollisionDetector detector;
    dphysics::Collision collision;
    bool colliding = detector.CircleCircleCollision(collision, nullptr, nullptr, &c1, &c2);

    EXPECT_FALSE(colliding);
}

TEST(CollisionTests, BoxBoxSanityCheck) {
    dphysics::BoxPrimitive b1;
    dphysics::BoxPrimitive b2;

    b1.Position = ysMath::LoadVector(0.0f, 0.0f, 0.0f, 0.0f);
    b1.HalfHeight = 0.5f;
    b1.HalfWidth = 0.5f;
    b1.Orientation = ysMath::LoadIdentity();

    b2.Position = ysMath::LoadVector(1.0f - 0.001f, 0.0f, 0.0f, 0.0f);
    b2.HalfHeight = 0.5f;
    b2.HalfWidth = 0.5f;
    b2.Orientation = ysMath::LoadIdentity();

    dphysics::CollisionDetector detector;
    dphysics::Collision collision;
    bool colliding = detector.BoxBoxCollision(collision, nullptr, nullptr, &b1, &b2);

    EXPECT_TRUE(colliding);
    EXPECT_GE(collision.m_penetration, 0);
    EXPECT_LE(collision.m_penetration, 0.001f);
}

TEST(CollisionTests, BoxBoxOffCenter) {
    dphysics::BoxPrimitive b1;
    dphysics::BoxPrimitive b2;

    b1.Position = ysMath::LoadVector(0.0f, 0.0f, 0.0f, 0.0f);
    b1.HalfHeight = 0.5f;
    b1.HalfWidth = 0.5f;
    b1.Orientation = ysMath::LoadIdentity();

    b2.Position = ysMath::LoadVector(1.0f - 0.001f, 0.25f, 0.0f, 0.0f);
    b2.HalfHeight = 0.5f;
    b2.HalfWidth = 0.5f;
    b2.Orientation = ysMath::LoadIdentity();

    dphysics::CollisionDetector detector;
    dphysics::Collision collision;
    bool colliding = detector.BoxBoxCollision(collision, nullptr, nullptr, &b1, &b2);

    EXPECT_TRUE(colliding);
    EXPECT_GE(collision.m_penetration, 0);
    EXPECT_LE(collision.m_penetration, 0.001f);
}

TEST(CollisionTests, BoxBoxOffCenterDifferentSizes) {
    dphysics::BoxPrimitive b1;
    dphysics::BoxPrimitive b2;

    constexpr float Penetration = 0.001f;

    b1.Position = ysMath::LoadVector(0.0f, 0.0f, 0.0f, 0.0f);
    b1.HalfHeight = 0.5f;
    b1.HalfWidth = 0.5f;
    b1.Orientation = ysMath::LoadIdentity();

    b2.Position = ysMath::LoadVector(0.75f - Penetration, 0.25f, 0.0f, 0.0f);
    b2.HalfHeight = 0.25f;
    b2.HalfWidth = 0.25f;
    b2.Orientation = ysMath::LoadIdentity();

    dphysics::CollisionDetector detector;
    dphysics::Collision collision;
    bool colliding = detector.BoxBoxCollision(collision, nullptr, nullptr, &b1, &b2);

    EXPECT_TRUE(colliding);
    EXPECT_GE(collision.m_penetration, 0);
    EXPECT_LE(collision.m_penetration, Penetration);
}

TEST(CollisionTests, BoxBoxOffCenterDifferentSizesDeep) {
    dphysics::BoxPrimitive b1;
    dphysics::BoxPrimitive b2;

    constexpr float Penetration = 0.01f;

    b1.Position = ysMath::LoadVector(0.0f, 0.0f, 0.0f, 0.0f);
    b1.HalfHeight = 0.5f;
    b1.HalfWidth = 0.5f;
    b1.Orientation = ysMath::LoadIdentity();

    b2.Position = ysMath::LoadVector(0.75f - Penetration, 0.5f, 0.0f, 0.0f);
    b2.HalfHeight = 0.25f;
    b2.HalfWidth = 0.25f;
    b2.Orientation = ysMath::LoadIdentity();

    dphysics::RigidBody a;
    dphysics::RigidBody b;

    dphysics::CollisionDetector detector;
    dphysics::Collision collision;
    bool colliding = detector.BoxBoxCollision(collision, &a, &b, &b1, &b2);

    EXPECT_TRUE(colliding);
    EXPECT_GE(collision.m_penetration, 0);
    EXPECT_LE(collision.m_penetration, Penetration);
    EXPECT_EQ(collision.m_body1, &b);
    EXPECT_EQ(collision.m_body2, &a);
}

TEST(CollisionTests, BoxBoxOffCenterDifferentSizesDeep2) {
    dphysics::BoxPrimitive b1;
    dphysics::BoxPrimitive b2;

    b1.Position = ysMath::LoadVector(0.0f, 0.0f, 0.0f, 0.0f);
    b1.HalfHeight = 0.5f;
    b1.HalfWidth = 0.5f;
    b1.Orientation = ysMath::LoadIdentity();

    b2.Position = ysMath::LoadVector(1.682f, 1.08866f, 0.0f, 0.0f);
    b2.HalfHeight = 1.25f;
    b2.HalfWidth = 1.25f;
    b2.Orientation = ysMath::LoadIdentity();

    dphysics::RigidBody a;
    dphysics::RigidBody b;

    dphysics::CollisionDetector detector;
    dphysics::Collision collision;
    bool colliding = detector.BoxBoxCollision(collision, &b, &a, &b2, &b1);

    EXPECT_TRUE(colliding);
    EXPECT_GE(collision.m_penetration, 0);
    EXPECT_EQ(collision.m_body1, &a);
    EXPECT_EQ(collision.m_body2, &b);
    VecEq(collision.m_normal, ysMath::LoadVector(-1.0f, 0.0f, 0.0f));
}

TEST(CollisionTests, BoxBoxBigSmall) {
    dphysics::BoxPrimitive b1;
    dphysics::BoxPrimitive b2;

    b1.Position = ysMath::LoadVector(0.0f, 0.0f, 0.0f, 0.0f);
    b1.HalfHeight = 10.0f;
    b1.HalfWidth = 10.0f;
    b1.Orientation = ysMath::LoadIdentity();

    b2.Position = ysMath::LoadVector(11.4f, 0.0f, 0.0f, 0.0f);
    b2.HalfHeight = 1.5f;
    b2.HalfWidth = 1.5f;
    b2.Orientation = ysMath::LoadIdentity();

    dphysics::RigidBody a;
    dphysics::RigidBody b;

    dphysics::CollisionDetector detector;
    dphysics::Collision collision;
    bool colliding = detector.BoxBoxCollision(collision, &b, &a, &b2, &b1);

    EXPECT_TRUE(colliding);
    EXPECT_NEAR(collision.m_penetration, 0.1f, 1E-4);
}

TEST(CollisionTests, RayCircleCollision) {
    dphysics::RayPrimitive b1;
    dphysics::CirclePrimitive b2;

    b1.Position = ysMath::LoadVector(0.0f, 0.0f, 0.0f, 0.0f);
    b1.Direction = ysMath::LoadVector(1.0f, 0.0f, 0.0f, 0.0f);

    b2.Position = ysMath::LoadVector(10.0f, 0.0f, 0.0f, 0.0f);
    b2.Radius = 1.0f;

    dphysics::RigidBody a;
    dphysics::RigidBody b;

    dphysics::CollisionDetector detector;
    dphysics::Collision collision;
    bool colliding = detector.RayCircleCollision(collision, &b, &a, &b1, &b2);

    EXPECT_TRUE(colliding);
    EXPECT_NEAR(collision.m_penetration, 9.0f, 1E-4);

    b2.Position = ysMath::LoadVector(10.0f, 1.0f, 0.0f, 0.0f); 
    colliding = detector.RayCircleCollision(collision, &b, &a, &b1, &b2);

    EXPECT_TRUE(colliding);
    EXPECT_NEAR(collision.m_penetration, 10.0f, 1E-4);

    b2.Position = ysMath::LoadVector(-10.0f, 1.0f, 0.0f, 0.0f);
    colliding = detector.RayCircleCollision(collision, &b, &a, &b1, &b2);

    EXPECT_FALSE(colliding);
}

TEST(CollisionTests, BoxCircleEdgeCase) {
    dphysics::BoxPrimitive b1;
    dphysics::CirclePrimitive b2;

    b1.Position = ysMath::LoadVector(0.1f, 0.0f, 0.0f, 1.0f);
    b1.HalfHeight = 1.0f;
    b1.HalfWidth = 1.0f;
    b1.Orientation = ysMath::LoadIdentity();

    b2.Position = ysMath::LoadVector(0.0f, 0.0f, 0.0f, 1.0f);
    b2.Radius = 1.0f;

    dphysics::RigidBody a;
    dphysics::RigidBody b;

    dphysics::CollisionDetector detector;
    dphysics::Collision collision;
    bool colliding = detector.CircleBoxCollision(collision, &b, &a, &b2, &b1);

    EXPECT_TRUE(colliding);
}
