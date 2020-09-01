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
        EXPECT_TRUE(rb.CheckState()) << "Check failed on iteration: " << i;
    }

    rb.CloseReplayFile();
}

TEST(DeltaPhysicsSystemTests, Gravity) {
    dphysics::RigidBodySystem rb;

    dphysics::RigidBody A;
    A.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    A.SetInverseMass(0.0f);
    A.Transform.SetPosition(ysMath::LoadVector(-2.0f, 0.5f, 0.0f));
    A.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    dphysics::CollisionObject *col;
    A.CollisionGeometry.NewBoxObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsBox()->Position = ysMath::Constants::Zero;
    col->GetAsBox()->HalfHeight = 0.5f;
    col->GetAsBox()->HalfWidth = 0.5f;
    col->GetAsBox()->Orientation = ysMath::Constants::QuatIdentity;

    dphysics::RigidBody C;
    C.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    C.SetInverseMass(0.0f);
    C.Transform.SetPosition(ysMath::LoadVector(2.0f, -1.5f, 0.0f));
    C.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    C.CollisionGeometry.NewBoxObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsBox()->Position = ysMath::Constants::Zero;
    col->GetAsBox()->HalfHeight = 0.5f;
    col->GetAsBox()->HalfWidth = 0.5f;
    col->GetAsBox()->Orientation = ysMath::Constants::QuatIdentity;

    dphysics::RigidBody B;
    B.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    B.SetInverseMass(1.0f);
    B.SetInverseInertiaTensor(B.GetRectangleTensor(0.5f, 0.5f));
    B.Transform.SetPosition(ysMath::LoadVector(1.0f, 2.0f, 0.0f));
    B.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    B.CollisionGeometry.NewBoxObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsBox()->Position = ysMath::Constants::Zero;
    col->GetAsBox()->HalfHeight = 0.5f;
    col->GetAsBox()->HalfWidth = 5.5f;
    col->GetAsBox()->Orientation = ysMath::Constants::QuatIdentity;

    B.AddForceWorldSpace(ysMath::LoadVector(0.0f, -2.0f, 0.0f), B.Transform.GetWorldPosition());

    rb.RegisterRigidBody(&A);
    rb.RegisterRigidBody(&B);
    rb.RegisterRigidBody(&C);

    rb.OpenReplayFile("SystemTest_replay.txt");

    for (int i = 0; i < 1000; ++i) {
        B.ClearAccumulators();
        B.AddForceWorldSpace(ysMath::LoadVector(0.0f, -2.0f, 0.0f), B.Transform.GetWorldPosition());

        rb.Update(1 / 30.0f);
        EXPECT_TRUE(rb.CheckState()) << "Check failed on iteration: " << i;
    }

    rb.CloseReplayFile();
}

TEST(DeltaPhysicsSystemTests, Vases) {
    dphysics::RigidBodySystem rb;

    dphysics::RigidBody A;
    A.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    A.SetInverseMass(0.0f);
    A.Transform.SetPosition(ysMath::LoadVector(0.0f, 0.0f, 0.0f));
    A.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    dphysics::CollisionObject *col;
    A.CollisionGeometry.NewBoxObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsBox()->Position = ysMath::Constants::Zero;
    col->GetAsBox()->HalfHeight = 0.1f;
    col->GetAsBox()->HalfWidth = 1.0f;
    col->GetAsBox()->Orientation = ysMath::Constants::QuatIdentity;

    dphysics::RigidBody B;
    B.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    B.SetInverseMass(1.0f);
    B.SetInverseInertiaTensor(B.GetRectangleTensor(2.24f, 0.963f));
    B.Transform.SetPosition(ysMath::LoadVector(0.0f, 4.0f, 0.0f));
    B.Transform.SetOrientation(ysMath::LoadQuaternion(0.01f, ysMath::Constants::ZAxis));

    B.CollisionGeometry.NewBoxObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsBox()->Position = ysMath::Constants::Zero;
    col->GetAsBox()->HalfHeight = 2.24f / 2;
    col->GetAsBox()->HalfWidth = 0.963f / 2;
    col->GetAsBox()->Orientation = ysMath::Constants::QuatIdentity;

    rb.RegisterRigidBody(&A);
    rb.RegisterRigidBody(&B);

    rb.OpenReplayFile("SystemTest_replay.txt");

    for (int i = 0; i < 1000; ++i) {
        if (i == 18) {
            int a = 0;
        }

        B.ClearAccumulators();
        B.AddForceWorldSpace(
            ysMath::LoadVector(0.0f, -15.0f / B.GetInverseMass(), 0.0f),
            B.Transform.GetWorldPosition());

        rb.Update(1 / 30.0f);
        EXPECT_TRUE(rb.CheckState()) << "Check failed on iteration: " << i;
    }

    rb.CloseReplayFile();
}

TEST(DeltaPhysicsSystemTests, VaseTipForce) {
    dphysics::RigidBodySystem rb;

    dphysics::RigidBody A;
    A.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    A.SetInverseMass(0.0f);
    A.Transform.SetPosition(ysMath::LoadVector(0.0f, 0.0f, 0.0f));
    A.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    dphysics::CollisionObject *col;
    A.CollisionGeometry.NewBoxObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsBox()->Position = ysMath::Constants::Zero;
    col->GetAsBox()->HalfHeight = 0.1f;
    col->GetAsBox()->HalfWidth = 1.0f;
    col->GetAsBox()->Orientation = ysMath::Constants::QuatIdentity;

    dphysics::RigidBody B;
    B.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    B.SetInverseMass(1.0f);
    B.SetInverseInertiaTensor(B.GetRectangleTensor(2.24f, 0.963f));
    B.Transform.SetPosition(ysMath::LoadVector(0.0f, 4.0f, 0.0f));
    B.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    B.CollisionGeometry.NewBoxObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsBox()->Position = ysMath::Constants::Zero;
    col->GetAsBox()->HalfHeight = 2.24f / 2;
    col->GetAsBox()->HalfWidth = 0.963f / 2;
    col->GetAsBox()->Orientation = ysMath::Constants::QuatIdentity;

    rb.RegisterRigidBody(&A);
    rb.RegisterRigidBody(&B);

    rb.OpenReplayFile("SystemTest_replay.txt");

    for (int i = 0; i < 1000; ++i) {
        if (i == 18) {
            int a = 0;
        }

        B.ClearAccumulators();
        B.AddForceWorldSpace(
            ysMath::LoadVector(0.0f, -15.0f / B.GetInverseMass(), 0.0f),
            B.Transform.GetWorldPosition());

        if (i > 50) {
            B.AddForceWorldSpace(
                ysMath::LoadVector(-10.0f, 0.0f, 0.0f),
                ysMath::LoadVector(0.0f, 0.963f, 0.0f));
        }

        rb.Update(1 / 30.0f);
        EXPECT_TRUE(rb.CheckState()) << "Check failed on iteration: " << i;
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
        if (i == 25) {
            int a = 0;
        }

        rb.Update(1 / 120.0f);
        EXPECT_TRUE(rb.CheckState()) << "Check failed on iteration: " << i;
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
    A.Transform.SetOrientation(ysMath::LoadQuaternion(0.0f, ysMath::Constants::ZAxis));

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
        EXPECT_TRUE(rb.CheckState()) << "Check failed on iteration: " << i;
    }

    rb.CloseReplayFile();
}

TEST(DeltaPhysicsSystemTests, SpinningWheel) {
    dphysics::RigidBodySystem rb;

    rb.InitializeFrictionTable(2, 0.5f, 0.2f);
    rb.SetFriction(0, 1, 1.4f, 0.9f);

    dphysics::RigidBody A;
    A.SetMaterial(0);
    A.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    A.SetInverseMass(1 / 1.0f);
    A.SetInverseInertiaTensor(A.GetRectangleTensor(0.5f, 0.5f));
    A.Transform.SetPosition(ysMath::LoadVector(-2.0f, 0.5f, 0.0f));
    A.Transform.SetOrientation(ysMath::LoadQuaternion(0.0f, ysMath::Constants::ZAxis));

    dphysics::CollisionObject *col;
    A.CollisionGeometry.NewCircleObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsCircle()->Position = ysMath::Constants::Zero;
    col->GetAsCircle()->Radius = 0.5f;

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

    B.SetVelocity(ysMath::LoadVector(0.0f, 0.0f, 0.0f));

    rb.RegisterRigidBody(&A);
    rb.RegisterRigidBody(&B);

    rb.OpenReplayFile("SystemTest_replay.txt");

    for (int i = 0; i < 1000; ++i) {
        A.ClearAccumulators();
        A.AddForceWorldSpace(ysMath::LoadVector(0.0f, -20.0f, 0.0f), A.Transform.GetWorldPosition());

        if (ysMath::GetX(A.GetAngularVelocity()) < 5.0f) {
            A.AddTorque(ysMath::LoadVector(0.0f, 0.0f, 10.0f));
        }

        rb.Update(1 / 120.0f);
        EXPECT_TRUE(rb.CheckState()) << "Check failed on iteration: " << i;
    }

    rb.CloseReplayFile();
}

TEST(DeltaPhysicsSystemTests, SpinningWheelLinkTest) {
    dphysics::RigidBodySystem rb;

    rb.InitializeFrictionTable(2, 0.5f, 0.2f);
    rb.SetFriction(0, 1, 1.4f, 0.9f);

    dphysics::RigidBody A;
    A.SetMaterial(0);
    A.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    A.SetInverseMass(1 / 1.0f);
    A.SetInverseInertiaTensor(A.GetRectangleTensor(0.5f, 0.5f));
    A.Transform.SetPosition(ysMath::LoadVector(-2.0f, 0.5f, 0.0f));
    A.Transform.SetOrientation(ysMath::LoadQuaternion(0.0f, ysMath::Constants::ZAxis));

    dphysics::CollisionObject *col;
    A.CollisionGeometry.NewCircleObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsCircle()->Position = ysMath::Constants::Zero;
    col->GetAsCircle()->Radius = 0.5f;

    dphysics::RigidBody C;
    C.SetMaterial(0);
    C.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    C.SetInverseMass(1 / 1.0f);
    C.SetInverseInertiaTensor(C.GetRectangleTensor(0.5f, 0.5f));
    C.Transform.SetPosition(ysMath::LoadVector(-2.0f, 5.0f, 0.0f));
    C.Transform.SetOrientation(ysMath::LoadQuaternion(0.0f, ysMath::Constants::ZAxis));

    C.CollisionGeometry.NewBoxObject(&col);
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

    B.SetVelocity(ysMath::LoadVector(0.0f, 0.0f, 0.0f));

    rb.RegisterRigidBody(&A);
    rb.RegisterRigidBody(&B);
    rb.RegisterRigidBody(&C);

    rb.OpenReplayFile("SystemTest_replay.txt");

    dphysics::HingeLink *hinge = rb.CreateLink<dphysics::HingeLink>(&A, &C);
    hinge->SetConnectionPoints(ysMath::Constants::Zero, ysMath::LoadVector(-2.0f, -2.0f, 0.0f));

    for (int i = 0; i < 1000; ++i) {
        A.ClearAccumulators();
        A.AddForceWorldSpace(ysMath::LoadVector(0.0f, -10.0f, 0.0f), A.Transform.GetWorldPosition());
        C.AddForceWorldSpace(ysMath::LoadVector(0.0f, -10.0f, 0.0f), C.Transform.GetWorldPosition());

        if (ysMath::GetX(A.GetAngularVelocity()) < 5.0f) {
            A.AddTorque(ysMath::LoadVector(0.0f, 0.0f, 10.0f));
        }

        rb.Update(1 / 120.0f);
        EXPECT_TRUE(rb.CheckState()) << "Check failed on iteration: " << i;
    }

    rb.CloseReplayFile();
}

TEST(DeltaPhysicsSystemTests, RectangleCircleCollision) {
    dphysics::RigidBodySystem rb;

    dphysics::RigidBody A;
    A.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    A.SetInverseMass(1.0f);
    A.Transform.SetPosition(ysMath::LoadVector(-2.0f, 0.0f, 0.0f));
    A.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    dphysics::CollisionObject *col;
    A.CollisionGeometry.NewCircleObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsCircle()->Position = ysMath::Constants::Zero;
    col->GetAsCircle()->Radius = 0.9f;

    dphysics::RigidBody B;
    B.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    B.SetInverseMass(1.0f);
    B.Transform.SetPosition(ysMath::LoadVector(2.0f, -1.5f, 0.0f));
    B.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    B.CollisionGeometry.NewBoxObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsBox()->Position = ysMath::Constants::Zero;
    col->GetAsBox()->HalfHeight = 1.5f;
    col->GetAsBox()->HalfWidth = 1.5f;
    col->GetAsBox()->Orientation = ysMath::Constants::QuatIdentity;

    B.SetVelocity(ysMath::LoadVector(0.0f, 0.0f, 0.0f));

    rb.RegisterRigidBody(&B);
    rb.RegisterRigidBody(&A);

    rb.OpenReplayFile("SystemTest_replay.txt");

    for (int i = 0; i < 1000; ++i) {
        B.SetVelocity(ysMath::LoadVector(-4.0f, 0.0f, 0.0f));

        rb.Update(1 / 60.0f);
        EXPECT_TRUE(rb.CheckState()) << "Check failed on iteration: " << i;
    }

    rb.CloseReplayFile();
}

TEST(DeltaPhysicsSystemTests, GroundJitterTest) {
    dphysics::RigidBodySystem rb;

    dphysics::RigidBody A;
    A.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    A.SetInverseMass(1.0f / 100.0f);
    A.SetAlwaysAwake(true);
    A.SetInverseInertiaTensor(A.GetRectangleTensor(1.0f * 4.0f, 1.48f * 4.0f));
    A.Transform.SetPosition(ysMath::LoadVector(0.0f, 5.0f, 0.0f));
    A.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    dphysics::CollisionObject *col;
    A.CollisionGeometry.NewBoxObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsBox()->Position = ysMath::Constants::Zero;
    col->GetAsBox()->HalfHeight = 1.48f;
    col->GetAsBox()->HalfWidth = 1.0f;

    dphysics::RigidBody B;
    B.SetHint(dphysics::RigidBody::RigidBodyHint::Dynamic);
    B.SetInverseMass(0.0f);
    B.Transform.SetPosition(ysMath::LoadVector(0.0f, 0.0f, 0.0f));
    B.Transform.SetOrientation(ysMath::Constants::QuatIdentity);

    B.CollisionGeometry.NewBoxObject(&col);
    col->SetMode(dphysics::CollisionObject::Mode::Fine);
    col->GetAsBox()->Position = ysMath::Constants::Zero;
    col->GetAsBox()->HalfHeight = 1.0f;
    col->GetAsBox()->HalfWidth = 1.0f;
    col->GetAsBox()->Orientation = ysMath::Constants::QuatIdentity;

    B.SetVelocity(ysMath::LoadVector(0.0f, 0.0f, 0.0f));

    rb.RegisterRigidBody(&B);
    rb.RegisterRigidBody(&A);

    rb.OpenReplayFile("SystemTest_replay.txt");

    for (int i = 0; i < 1000; ++i) {
        if (i == 44) {
            int a = 0;
        }

        A.ClearAccumulators();
        A.AddForceWorldSpace(ysMath::LoadVector(0.0f, -100.0f * 10.0f, 0.0f), A.Transform.GetWorldPosition());  

        rb.Update(1 / 60.0f);
        EXPECT_TRUE(rb.CheckState()) << "Check failed on iteration: " << i;
    }

    rb.CloseReplayFile();
}

TEST(DeltaPhysicsSystemTests, ForceWithFriction) {
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

    B.SetVelocity(ysMath::LoadVector(0.0f, 0.0f, 0.0f));

    rb.RegisterRigidBody(&A);
    rb.RegisterRigidBody(&B);

    rb.OpenReplayFile("SystemTest_replay.txt");

    for (int i = 0; i < 100; ++i) {
        A.AddForceWorldSpace(
            ysMath::LoadVector(-1.0f, -1.0f, 0.0f, 0.0f),
            ysMath::Add(A.Transform.GetWorldPosition(), ysMath::LoadVector(0.0f, 0.5f, 0.0f)));
        rb.Update(1 / 120.0f);
        EXPECT_TRUE(rb.CheckState()) << "Check failed on iteration: " << i;
    }

    rb.CloseReplayFile();
}
