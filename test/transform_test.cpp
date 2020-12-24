#include <pch.h>

#include "../include/yds_transform.h"

#include "utilities.h"

#include <fstream>

TEST(TransformTest, BasicTransform) {
    ysTransform transform;
    transform.SetOrientation(ysMath::LoadQuaternion(ysMath::Constants::PI / 2, ysMath::Constants::ZAxis));
    transform.SetPosition(ysMath::LoadVector(1.0f, 1.0f, 0.0f, 0.0f));

    VecEq(transform.WorldToLocalSpace(transform.GetPositionParentSpace()), ysMath::Constants::Zero);
    VecEq(transform.WorldToLocalSpace(ysMath::Constants::Zero), ysMath::LoadVector(-1.0f, 1.0f, 0.0f, 0.0f));
}

TEST(TransformTest, ParentTest) {
    ysTransform parent;
    parent.SetOrientation(ysMath::LoadQuaternion(ysMath::Constants::PI / 2, ysMath::Constants::ZAxis));
    parent.SetPosition(ysMath::LoadVector(5.0f, 0.0f, 0.0f, 0.0f));

    ysTransform child;
    child.SetOrientation(ysMath::LoadQuaternion(ysMath::Constants::PI / 2, ysMath::Constants::ZAxis));
    child.SetPosition(ysMath::LoadVector(1.0f, 1.0f, 0.0f, 0.0f));
    child.SetParent(&parent);

    VecEq(child.GetWorldPosition(), ysMath::LoadVector(5.0f - 1.0f, 1.0f, 0.0f, 0.0f));

    ysVector originLocal = child.WorldToLocalSpace(ysMath::Constants::Zero);
    ysVector originWorld = child.LocalToWorldSpace(originLocal);

    VecEq(originLocal, child.WorldToLocalSpace(originWorld));
    VecEq(ysMath::Constants::Zero, child.LocalToWorldSpace(originLocal));
}

TEST(TransformTest, ParentTestDeepTranslationOnly) {
    ysTransform t0, t1, t2;
    t0.SetPosition(ysMath::LoadVector(1.0f, 2.0f, -3.0f, 1.0f));
    t0.SetOrientation(ysMath::Constants::QuatIdentity);

    t1.SetParent(&t0);
    t1.SetPosition(ysMath::LoadVector(10.0f, -62.0f, 35.0f, 1.0f));
    t1.SetOrientation(ysMath::Constants::QuatIdentity);

    t2.SetParent(&t1);
    t2.SetPosition(ysMath::LoadVector(-13.0f, 25.0f, 33.0f, 1.0f));
    t2.SetOrientation(ysMath::Constants::QuatIdentity);

    ysMatrix m0 = ysMath::LoadMatrix(t0.GetOrientationParentSpace(), t0.GetPositionParentSpace());
    ysMatrix m1 = ysMath::LoadMatrix(t1.GetOrientationParentSpace(), t1.GetPositionParentSpace());
    ysMatrix m2 = ysMath::LoadMatrix(t2.GetOrientationParentSpace(), t2.GetPositionParentSpace());

    ysMatrix world0 = m0;
    ysMatrix world1 = ysMath::MatMult(world0, m1);
    ysMatrix world2 = ysMath::MatMult(world1, m2);

    ysVector probe = ysMath::LoadVector(1.0f, 0.5f, -0.7f, 1.0f);

    ysVector testValue0 = t0.LocalToWorldSpace(probe);
    ysVector ref0 = ysMath::MatMult(world0, probe);

    ysVector testValue1 = t1.LocalToWorldSpace(probe);
    ysVector ref1 = ysMath::MatMult(world1, probe);

    ysVector testValue2 = t2.LocalToWorldSpace(probe);
    ysVector ref2 = ysMath::MatMult(world2, probe);

    VecEq(testValue0, ref0);
    VecEq(testValue1, ref1);
    VecEq(testValue2, ref2);
}

TEST(TransformTest, ParentTestDeepRotationOnly) {
    ysTransform t0, t1, t2;
    t0.SetPosition(ysMath::LoadVector(1.0f, 2.0f, -3.0f, 1.0f));
    t0.SetOrientation(ysMath::Constants::Zero3);

    t1.SetParent(&t0);
    t1.SetPosition(ysMath::LoadVector(10.0f, -62.0f, 35.0f, 1.0f));
    t1.SetOrientation(ysMath::Constants::Zero3);

    t2.SetParent(&t1);
    t2.SetPosition(ysMath::LoadVector(-13.0f, 25.0f, 33.0f, 1.0f));
    t2.SetOrientation(ysMath::Constants::Zero3);

    ysMatrix m0 = ysMath::LoadMatrix(t0.GetOrientationParentSpace(), t0.GetPositionParentSpace());
    ysMatrix m1 = ysMath::LoadMatrix(t1.GetOrientationParentSpace(), t1.GetPositionParentSpace());
    ysMatrix m2 = ysMath::LoadMatrix(t2.GetOrientationParentSpace(), t2.GetPositionParentSpace());

    ysMatrix world0 = m0;
    ysMatrix world1 = ysMath::MatMult(world0, m1);
    ysMatrix world2 = ysMath::MatMult(world1, m2);

    ysVector probe = ysMath::LoadVector(1.0f, 0.5f, -0.7f, 1.0f);

    ysVector testValue0 = t0.LocalToWorldSpace(probe);
    ysVector ref0 = ysMath::MatMult(world0, probe);

    ysVector testValue1 = t1.LocalToWorldSpace(probe);
    ysVector ref1 = ysMath::MatMult(world1, probe);

    ysVector testValue2 = t2.LocalToWorldSpace(probe);
    ysVector ref2 = ysMath::MatMult(world2, probe);

    VecEq(testValue0, ref0);
    VecEq(testValue1, ref1);
    VecEq(testValue2, ref2);
}

TEST(TransformTest, ParentTestDeep) {
    ysTransform t0, t1, t2;
    t0.SetPosition(ysMath::LoadVector(1.0f, 2.0f, -3.0f, 1.0f));
    t0.SetOrientation(ysMath::LoadVector(1.0f, 5.0f, 7.0f, -5.0f));

    t1.SetParent(&t0);
    t1.SetPosition(ysMath::LoadVector(10.0f, -62.0f, 35.0f, 1.0f));
    t1.SetOrientation(ysMath::LoadVector(71.0f, 65.0f, -77.0f, 56.0f));

    t2.SetParent(&t1);
    t2.SetPosition(ysMath::LoadVector(-13.0f, 25.0f, 33.0f, 1.0f));
    t2.SetOrientation(ysMath::LoadVector(51.0f, -45.0f, 75.0f, 35.0f));

    ysMatrix m0 = ysMath::LoadMatrix(t0.GetOrientationParentSpace(), t0.GetPositionParentSpace());
    ysMatrix m1 = ysMath::LoadMatrix(t1.GetOrientationParentSpace(), t1.GetPositionParentSpace());
    ysMatrix m2 = ysMath::LoadMatrix(t2.GetOrientationParentSpace(), t2.GetPositionParentSpace());

    ysMatrix world0 = m0;
    ysMatrix world1 = ysMath::MatMult(world0, m1);
    ysMatrix world2 = ysMath::MatMult(world1, m2);

    ysVector probe = ysMath::LoadVector(1.0f, 0.5f, -0.7f, 1.0f);
    
    ysVector testValue0 = t0.LocalToWorldSpace(probe);
    ysVector ref0 = ysMath::MatMult(world0, probe);

    ysVector testValue1 = t1.LocalToWorldSpace(probe);
    ysVector ref1 = ysMath::MatMult(world1, probe);

    ysVector testValue2 = t2.LocalToWorldSpace(probe);
    ysVector ref2 = ysMath::MatMult(world2, probe);

    VecEq(testValue0, ref0);
    VecEq(testValue1, ref1);
    VecEq(testValue2, ref2);
}

TEST(TransformTest, FullRotation) {
    ysVector probe = ysMath::LoadVector(0.0f, 1.0f, 0.0f);
    for (int i = 0; i <= 360; ++i) {
        float angle = (i / 360.0f) * ysMath::Constants::PI / 180.0f;
        ysQuaternion quat = ysMath::LoadQuaternion(angle, ysMath::Constants::ZAxis);
        ysMatrix mat = ysMath::RotationTransform(ysMath::Constants::ZAxis, angle);

        ysVector ref = ysMath::MatMult(mat, probe);
        ysVector test = ysMath::QuatTransform(quat, probe);

        VecEq(ref, test);
    }
}

TEST(TransformTest, FullRotationMatConversion) {
    ysVector probe = ysMath::LoadVector(0.0f, 1.0f, 0.0f, 1.0f);
    for (int i = 0; i <= 360; ++i) {
        float angle = (i / 360.0f) * ysMath::Constants::PI / 180.0f;
        ysQuaternion quat = ysMath::LoadQuaternion(angle, ysMath::Constants::ZAxis);
        ysMatrix mat = ysMath::RotationTransform(ysMath::Constants::ZAxis, angle);

        ysVector ref = ysMath::MatMult(mat, probe);
        ysVector test = ysMath::MatMult(ysMath::LoadMatrix(quat), probe);

        VecEq(ref, test);
    }
}

TEST(TransformTest, WorldSpaceConversion) {
    ysTransform t0, t1, t2;
    t0.SetPosition(ysMath::LoadVector(1.0f, 2.0f, -3.0f, 1.0f));
    t0.SetOrientation(ysMath::LoadVector(1.0f, 5.0f, 7.0f, -5.0f));

    t1.SetParent(&t0);
    t1.SetPosition(ysMath::LoadVector(10.0f, -62.0f, 35.0f, 1.0f));
    t1.SetOrientation(ysMath::LoadVector(71.0f, 65.0f, -77.0f, 56.0f));

    t2.SetParent(&t1);
    t2.SetPosition(ysMath::LoadVector(-13.0f, 25.0f, 33.0f, 1.0f));
    t2.SetOrientation(ysMath::LoadVector(51.0f, -45.0f, 75.0f, 35.0f));

    ysMatrix m0 = ysMath::LoadMatrix(t0.GetOrientationParentSpace(), t0.GetPositionParentSpace());
    ysMatrix m1 = ysMath::LoadMatrix(t1.GetOrientationParentSpace(), t1.GetPositionParentSpace());
    ysMatrix m2 = ysMath::LoadMatrix(t2.GetOrientationParentSpace(), t2.GetPositionParentSpace());

    ysMatrix world0 = m0;
    ysMatrix world1 = ysMath::MatMult(world0, m1);
    ysMatrix world2 = ysMath::MatMult(world1, m2);

    ysVector probe = ysMath::Constants::Zero3;

    ysVector testValue0 = t0.GetWorldPosition();
    ysVector ref0 = ysMath::MatMult(world0, probe);

    ysVector testValue1 = t1.GetWorldPosition();
    ysVector ref1 = ysMath::MatMult(world1, probe);

    ysVector testValue2 = t2.GetWorldPosition();
    ysVector ref2 = ysMath::MatMult(world2, probe);

    VecEq(testValue0, ref0);
    VecEq(testValue1, ref1);
    VecEq(testValue2, ref2);
}
