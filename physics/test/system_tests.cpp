#include <pch.h>

#include "../include/delta_physics.h"
#include "utilities.h"

TEST(DeltaPhysicsSystemTests, SanityCheck) {
    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);
}

TEST(DeltaPhysicsSystemTests, SystemCheck) {
    dphysics::RigidBodySystem rb;
    
    dphysics::RigidBody A;
    A.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    A.SetInverseMass(1 / 1.0f);
    A.Transform.SetPosition(ysMath::LoadVector(-10.0f, 0.0f, 0.0f));
    A.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    dphysics::CollisionObject *col;
    A.CollisionGeometry.NewCircleObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsCircle()->Position = ysMath::Constants::Zero;
    col->GetAsCircle()->Radius = 1.0f;

    dphysics::RigidBody B;
    B.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    B.SetInverseMass(1 / 1.0f);
    B.Transform.SetPosition(ysMath::LoadVector(10.0f, 0.0f, 0.0f));
    B.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    B.CollisionGeometry.NewCircleObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsCircle()->Position = ysMath::Constants::Zero;
    col->GetAsCircle()->Radius = 1.0f;

    A.SetVelocity(ysMath::LoadVector(1.0f, 0.0f, 0.0f));
    B.SetVelocity(ysMath::LoadVector(-1.0f, 0.0f, 0.0f));

    rb.RegisterRigidBody(&A);
    rb.RegisterRigidBody(&B);

    for (int i = 0; i < 120; ++i) {
        A.SetVelocity(ysMath::LoadVector(1.0f, 0.0f, 0.0f));
        B.SetVelocity(ysMath::LoadVector(-1.0f, 0.0f, 0.0f));

        rb.Update(1 / 120.0f);
    }

    VecEq(A.Transform.GetWorldPosition(), ysMath::LoadVector(-9.0f, 0.0f, 0.0f));
    VecEq(B.Transform.GetWorldPosition(), ysMath::LoadVector(9.0f, 0.0f, 0.0f));
}

TEST(DeltaPhysicsSystemTests, CollisionCheck) {
    dphysics::RigidBodySystem rb;

    dphysics::RigidBody A;
    A.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    A.SetInverseMass(1 / 1.0f);
    A.Transform.SetPosition(ysMath::LoadVector(-10.0f, 0.0f, 0.0f));
    A.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    dphysics::CollisionObject *col;
    A.CollisionGeometry.NewCircleObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsCircle()->Position = ysMath::Constants::Zero;
    col->GetAsCircle()->Radius = 1.0f;

    dphysics::RigidBody B;
    B.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    B.SetInverseMass(1 / 1.0f);
    B.Transform.SetPosition(ysMath::LoadVector(10.0f, 0.0f, 0.0f));
    B.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    B.CollisionGeometry.NewCircleObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsCircle()->Position = ysMath::Constants::Zero;
    col->GetAsCircle()->Radius = 1.0f;

    A.SetVelocity(ysMath::LoadVector(1.0f, 0.0f, 0.0f));
    B.SetVelocity(ysMath::LoadVector(-1.0f, 0.0f, 0.0f));

    rb.RegisterRigidBody(&A);
    rb.RegisterRigidBody(&B);

    for (int i = 0; i < 1200; ++i) {
        A.SetVelocity(ysMath::LoadVector(1.0f, 0.0f, 0.0f));
        B.SetVelocity(ysMath::LoadVector(-1.0f, 0.0f, 0.0f));

        rb.Update(1 / 120.0f);
    }

    VecEq(A.Transform.GetWorldPosition(), ysMath::LoadVector(-1.0f, 0.0f, 0.0f));
    VecEq(B.Transform.GetWorldPosition(), ysMath::LoadVector(1.0f, 0.0f, 0.0f));
}

TEST(DeltaPhysicsSystemTests, VelocityResolutionCheck) {
    dphysics::RigidBodySystem rb;

    dphysics::RigidBody A;
    A.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    A.SetInverseMass(1 / 1.0f);
    A.Transform.SetPosition(ysMath::LoadVector(-2.0f, 0.0f, 0.0f));
    A.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    dphysics::CollisionObject *col;
    A.CollisionGeometry.NewCircleObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsCircle()->Position = ysMath::Constants::Zero;
    col->GetAsCircle()->Radius = 1.0f;

    dphysics::RigidBody B;
    B.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    B.SetInverseMass(1 / 1.0f);
    B.Transform.SetPosition(ysMath::LoadVector(2.0f, 0.0f, 0.0f));
    B.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    B.CollisionGeometry.NewCircleObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsCircle()->Position = ysMath::Constants::Zero;
    col->GetAsCircle()->Radius = 1.0f;

    A.SetVelocity(ysMath::LoadVector(10.0f, 0.0f, 0.0f));
    B.SetVelocity(ysMath::LoadVector(-10.0f, 0.0f, 0.0f));

    rb.RegisterRigidBody(&A);
    rb.RegisterRigidBody(&B);

    rb.OpenReplayFile("SystemTest_replay.txt");

    for (int i = 0; i < 100; ++i) {
        rb.Update(1 / 120.0f);
    }

    rb.CloseReplayFile();
}

TEST(DeltaPhysicsSystemTests, VelocityResolutionCheckOffAngle) {
    dphysics::RigidBodySystem rb;

    dphysics::RigidBody A;
    A.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    A.SetInverseMass(1 / 1.0f);
    A.SetInverseInertiaTensor(ysMath::LoadIdentity());
    A.Transform.SetPosition(ysMath::LoadVector(-2.0f, 0.5f, 0.0f));
    A.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    dphysics::CollisionObject *col;
    A.CollisionGeometry.NewCircleObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsCircle()->Position = ysMath::Constants::Zero;
    col->GetAsCircle()->Radius = 1.0f;

    dphysics::RigidBody B;
    B.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    B.SetInverseMass(1 / 1.0f);
    B.SetInverseInertiaTensor(ysMath::LoadIdentity());
    B.Transform.SetPosition(ysMath::LoadVector(2.0f, 0.0f, 0.0f));
    B.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    B.CollisionGeometry.NewCircleObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsCircle()->Position = ysMath::Constants::Zero;
    col->GetAsCircle()->Radius = 1.0f;

    A.SetVelocity(ysMath::LoadVector(10.0f, 0.0f, 0.0f));
    B.SetVelocity(ysMath::LoadVector(-10.0f, 0.0f, 0.0f));

    rb.RegisterRigidBody(&A);
    rb.RegisterRigidBody(&B);

    rb.OpenReplayFile("SystemTest_replay.txt");

    for (int i = 0; i < 100; ++i) {
        rb.Update(1 / 120.0f);
    }

    rb.CloseReplayFile();
}
TEST(DeltaPhysicsSystemTests, VelocityResolutionRotation) {
    dphysics::RigidBodySystem rb;

    dphysics::RigidBody A;
    A.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    A.SetInverseMass(1 / 1.0f);
    A.SetInverseInertiaTensor(ysMath::LoadIdentity());
    A.Transform.SetPosition(ysMath::LoadVector(-2.0f, 0.5f, 0.0f));
    A.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    dphysics::CollisionObject *col;
    A.CollisionGeometry.NewBoxObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsBox()->Position = ysMath::Constants::Zero;
    col->GetAsBox()->HalfHeight = 0.5f;
    col->GetAsBox()->HalfWidth = 0.5f;
    col->GetAsBox()->Orientation = ysMath::Constants::QuatIdentity;

    dphysics::RigidBody B;
    B.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    B.SetInverseMass(1 / 1.0f);
    B.SetInverseInertiaTensor(ysMath::LoadIdentity());
    B.Transform.SetPosition(ysMath::LoadVector(2.0f, 0.0f, 0.0f));
    B.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    B.CollisionGeometry.NewBoxObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsBox()->Position = ysMath::Constants::Zero;
    col->GetAsBox()->HalfHeight = 0.5f;
    col->GetAsBox()->HalfWidth = 0.5f;
    col->GetAsBox()->Orientation = ysMath::Constants::QuatIdentity;

    A.SetVelocity(ysMath::LoadVector(10.0f, 0.0f, 0.0f));
    B.SetVelocity(ysMath::LoadVector(-10.0f, 0.0f, 0.0f));

    rb.RegisterRigidBody(&A);
    rb.RegisterRigidBody(&B);

    rb.OpenReplayFile("SystemTest_replay.txt");

    for (int i = 0; i < 100; ++i) {
        rb.Update(1 / 120.0f);
    }

    rb.CloseReplayFile();
}

TEST(DeltaPhysicsSystemTests, VelocityResolutionRotationDifferentMasses) {
    dphysics::RigidBodySystem rb;

    dphysics::RigidBody A;
    A.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    A.SetInverseMass(1 / 1.0f);
    A.SetInverseInertiaTensor(A.GetRectangleTensor(0.5f, 0.5f));
    A.Transform.SetPosition(ysMath::LoadVector(-2.0f, 0.5f, 0.0f));
    A.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    dphysics::CollisionObject *col;
    A.CollisionGeometry.NewBoxObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsBox()->Position = ysMath::Constants::Zero;
    col->GetAsBox()->HalfHeight = 0.5f;
    col->GetAsBox()->HalfWidth = 0.5f;
    col->GetAsBox()->Orientation = ysMath::Constants::QuatIdentity;

    dphysics::RigidBody B;
    B.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    B.SetInverseMass(1 / 100.0f);
    B.SetInverseInertiaTensor(B.GetRectangleTensor(0.5f, 0.5f));
    B.Transform.SetPosition(ysMath::LoadVector(2.0f, 0.0f, 0.0f));
    B.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    B.CollisionGeometry.NewBoxObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsBox()->Position = ysMath::Constants::Zero;
    col->GetAsBox()->HalfHeight = 0.5f;
    col->GetAsBox()->HalfWidth = 0.5f;
    col->GetAsBox()->Orientation = ysMath::Constants::QuatIdentity;

    A.SetVelocity(ysMath::LoadVector(10.0f, 0.0f, 0.0f));
    B.SetVelocity(ysMath::LoadVector(-10.0f, 0.0f, 0.0f));

    rb.RegisterRigidBody(&A);
    rb.RegisterRigidBody(&B);

    rb.OpenReplayFile("SystemTest_replay.txt");

    for (int i = 0; i < 100; ++i) {
        rb.Update(1 / 120.0f);
    }

    rb.CloseReplayFile();
}

TEST(DeltaPhysicsSystemTests, VelocityResolutionHighFriction) {
    dphysics::RigidBodySystem rb;

    rb.InitializeFrictionTable(2, 0.5f, 0.2f);
    rb.SetFriction(0, 1, 1.5f, 1.0f);

    dphysics::RigidBody A;
    A.SetMaterial(0);
    A.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    A.SetInverseMass(1 / 1.0f);
    A.SetInverseInertiaTensor(A.GetRectangleTensor(0.5f, 0.5f));
    A.Transform.SetPosition(ysMath::LoadVector(-2.0f, 0.5f, 0.0f));
    A.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    dphysics::CollisionObject *col;
    A.CollisionGeometry.NewBoxObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsBox()->Position = ysMath::Constants::Zero;
    col->GetAsBox()->HalfHeight = 0.5f;
    col->GetAsBox()->HalfWidth = 0.5f;
    col->GetAsBox()->Orientation = ysMath::Constants::QuatIdentity;

    dphysics::RigidBody B;
    B.SetMaterial(1);
    B.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    B.SetInverseMass(0.0f);
    B.Transform.SetPosition(ysMath::LoadVector(0.0f, -2.0f, 0.0f));
    B.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    B.CollisionGeometry.NewBoxObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsBox()->Position = ysMath::Constants::Zero;
    col->GetAsBox()->HalfHeight = 0.5f;
    col->GetAsBox()->HalfWidth = 15.0f;
    col->GetAsBox()->Orientation = ysMath::Constants::QuatIdentity;

    A.SetVelocity(ysMath::LoadVector(-10.0f, -10.0f, 0.0f));
    B.SetVelocity(ysMath::LoadVector(0.0f, 0.0f, 0.0f));

    rb.RegisterRigidBody(&A);
    rb.RegisterRigidBody(&B);

    rb.OpenReplayFile("SystemTest_replay.txt");

    for (int i = 0; i < 100; ++i) {
        rb.Update(1 / 120.0f);
    }

    rb.CloseReplayFile();
}

TEST(DeltaPhysicsSystemTests, VelocityResolutionLowFriction) {
    dphysics::RigidBodySystem rb;

    rb.InitializeFrictionTable(2, 0.5f, 0.2f);
    rb.SetFriction(0, 1, 0.1f, 0.01f);

    dphysics::RigidBody A;
    A.SetMaterial(0);
    A.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    A.SetInverseMass(1 / 1.0f);
    A.SetInverseInertiaTensor(A.GetRectangleTensor(0.5f, 0.5f));
    A.Transform.SetPosition(ysMath::LoadVector(-2.0f, 0.5f, 0.0f));
    A.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    dphysics::CollisionObject *col;
    A.CollisionGeometry.NewBoxObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsBox()->Position = ysMath::Constants::Zero;
    col->GetAsBox()->HalfHeight = 0.5f;
    col->GetAsBox()->HalfWidth = 0.5f;
    col->GetAsBox()->Orientation = ysMath::Constants::QuatIdentity;

    dphysics::RigidBody B;
    B.SetMaterial(1);
    B.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    B.SetInverseMass(0.0f);
    B.Transform.SetPosition(ysMath::LoadVector(0.0f, -2.0f, 0.0f));
    B.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    B.CollisionGeometry.NewBoxObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsBox()->Position = ysMath::Constants::Zero;
    col->GetAsBox()->HalfHeight = 0.5f;
    col->GetAsBox()->HalfWidth = 15.0f;
    col->GetAsBox()->Orientation = ysMath::Constants::QuatIdentity;

    A.SetVelocity(ysMath::LoadVector(-10.0f, -10.0f, 0.0f));
    B.SetVelocity(ysMath::LoadVector(0.0f, 0.0f, 0.0f));

    rb.RegisterRigidBody(&A);
    rb.RegisterRigidBody(&B);

    rb.OpenReplayFile("SystemTest_replay.txt");

    for (int i = 0; i < 100; ++i) {
        rb.Update(1 / 120.0f);
    }

    rb.CloseReplayFile();
}
