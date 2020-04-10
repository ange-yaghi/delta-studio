#include <pch.h>

#include "../include/yds_transform.h"

#include "utilities.h"

#include <fstream>

TEST(TransformTest, BasicTransform) {
    ysTransform transform;
    transform.SetOrientation(ysMath::LoadQuaternion(ysMath::Constants::PI / 2, ysMath::Constants::ZAxis));
    transform.SetPosition(ysMath::LoadVector(1.0f, 1.0f, 0.0f, 0.0f));

    VecEq(transform.WorldToLocalSpace(transform.GetLocalPosition()), ysMath::Constants::Zero);
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

