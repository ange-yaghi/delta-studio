#include <pch.h>

#include "../include/yds_animation_action.h"
#include "../include/yds_animation_action_binding.h"
#include "../include/yds_animation_mixer.h"
#include "../include/yds_animation_curve.h"
#include "../include/yds_animation_target.h"
#include "../include/yds_animation_interchange_file_0_0.h"

#include "utilities.h"

#include <fstream>

constexpr float Epsilon = 1E-5;

TEST(AnimationTest, SanityCheck) {
    ysAnimationChannel mixer;

    ysAnimationAction action;
    action.SetLength(1.0f);

    ysAnimationActionBinding binding;
    binding.SetAction(&action);

    EXPECT_NEAR(mixer.ProbeTotalAmplitude(), 0.0f, Epsilon);

    ysAnimationChannel::ActionSettings settings;
    settings.FadeIn = 0.5f;

    mixer.AddSegment(&binding, settings);
    mixer.Sample();
    EXPECT_NEAR(mixer.ProbeTotalAmplitude(), 0.0f, Epsilon);

    mixer.Advance(0.25f); mixer.Sample();
    EXPECT_NEAR(mixer.ProbeTotalAmplitude(), 0.5f, Epsilon);

    mixer.Advance(0.25f); mixer.Sample();
    EXPECT_NEAR(mixer.ProbeTotalAmplitude(), 1.0f, Epsilon);
}

TEST(AnimationTest, TransitionTest) {
    ysAnimationChannel mixer;

    ysAnimationAction action;
    action.SetLength(1.0f);

    ysAnimationActionBinding binding;
    binding.SetAction(&action);

    EXPECT_NEAR(mixer.ProbeTotalAmplitude(), 0.0f, Epsilon);

    ysAnimationChannel::ActionSettings settings;
    settings.FadeIn = 0.5f;

    mixer.AddSegment(&binding, settings);
    mixer.AddSegmentAtEnd(&binding, settings);
    
    mixer.Sample();
    EXPECT_NEAR(mixer.ProbeTotalAmplitude(), 0.0f, Epsilon);
    
    mixer.Advance(0.5f); mixer.Sample();
    EXPECT_NEAR(mixer.ProbeTotalAmplitude(), 1.0f, Epsilon);
    
    mixer.Advance(0.5f); mixer.Sample();
    EXPECT_NEAR(mixer.ProbeTotalAmplitude(), 1.0f, Epsilon);

    mixer.Advance(0.5f); mixer.Sample();
    EXPECT_NEAR(mixer.ProbeTotalAmplitude(), 1.0f, Epsilon);

    mixer.Advance(0.5f); mixer.Sample();
    EXPECT_NEAR(mixer.ProbeTotalAmplitude(), 1.0f, Epsilon);

    mixer.Advance(0.5f); mixer.Sample();
    EXPECT_NEAR(mixer.ProbeTotalAmplitude(), 1.0f, Epsilon);
}

TEST(AnimationTest, ImmediateTransitionTest) {
    ysAnimationChannel mixer;

    ysAnimationAction action;
    action.SetLength(1.0f);

    ysAnimationActionBinding binding;
    binding.SetAction(&action);

    EXPECT_NEAR(mixer.ProbeTotalAmplitude(), 0.0f, Epsilon);

    ysAnimationChannel::ActionSettings settings;
    settings.FadeIn = 0.0f;
    mixer.AddSegment(&binding, settings);

    mixer.Sample();
    EXPECT_NEAR(mixer.ProbeTotalAmplitude(), 1.0f, Epsilon);
}

TEST(AnimationTest, CurveTest) {
    ysAnimationCurve curve;
    curve.AddSamplePoint(1.0f, 0.1f);
    curve.AddSamplePoint(2.0f, 1.0f);
    curve.AddSamplePoint(3.0f, 0.5f);
    curve.AddSamplePoint(4.0f, 0.25f);

    float s0 = curve.Sample(0.0f);
    EXPECT_NEAR(s0, 0.1f, Epsilon);

    float s1 = curve.Sample(1.5f);
    EXPECT_NEAR(s1, 1.1f / 2, Epsilon);

    float s2 = curve.Sample(3.0f);
    EXPECT_NEAR(s2, 0.5f, Epsilon);

    float s3 = curve.Sample(5.0f);
    EXPECT_NEAR(s3, 0.25f, Epsilon);

    float s4 = curve.Sample(4.0f);
    EXPECT_NEAR(s4, 0.25f, Epsilon);
}

TEST(AnimationTest, BindingTest) {
    ysAnimationAction action;
    action.SetLength(4.0f);
    action.SetName("TestAction");

    ysAnimationCurve *curve0 = action.NewCurve("Bone0");
    curve0->SetCurveType(ysAnimationCurve::CurveType::LocationX);
    curve0->AddSamplePoint(1.0f, 0.1f);
    curve0->AddSamplePoint(2.0f, 1.0f);
    curve0->AddSamplePoint(3.0f, 0.5f);
    curve0->AddSamplePoint(4.0f, 0.25f);

    ysAnimationCurve *curve1 = action.NewCurve("Bone1");
    curve1->SetCurveType(ysAnimationCurve::CurveType::LocationY);
    curve1->AddSamplePoint(1.0f, 0.5f);
    curve1->AddSamplePoint(2.0f, 0.5f);
    curve1->AddSamplePoint(3.0f, 0.5f);
    curve1->AddSamplePoint(4.0f, 0.25f);

    TransformTarget bone0Loc, bone1Loc;

    bone0Loc.ClearLocation(ysMath::LoadVector(1.0f, 2.0f, 3.0f, 0.0f));
    bone1Loc.ClearLocation(ysMath::LoadVector(4.0f, 5.0f, 6.0f, 0.0f));

    ysAnimationActionBinding binding;
    binding.SetAction(&action);
    binding.AddTarget("Bone0", &bone0Loc, nullptr);
    binding.AddTarget("Bone1", &bone1Loc, nullptr);

    ysAnimationChannel::ActionSettings settings;
    settings.FadeIn = 0.0f;

    ysAnimationChannel mixer;
    mixer.AddSegment(&binding, settings);
    mixer.Sample();
    bone0Loc.ClearFlags();
    bone1Loc.ClearFlags();
    
    mixer.Advance(0.1f); mixer.Sample();
    VecEq(bone0Loc.GetLocationResult(), ysMath::LoadVector(0.1f, 2.0f, 3.0f, 0.0f));
    VecEq(bone1Loc.GetLocationResult(), ysMath::LoadVector(4.0f, 0.5f, 6.0f, 0.0f));
}

TEST(AnimationTest, ActionMixing) {
    ysAnimationAction action0;
    action0.SetLength(4.0f);
    action0.SetName("Action0");

    ysAnimationCurve *curve0 = action0.NewCurve("Bone0");
    curve0->SetCurveType(ysAnimationCurve::CurveType::LocationX);
    curve0->AddSamplePoint(1.0f, 0.5f);
    curve0->AddSamplePoint(2.0f, 1.0f);
    curve0->AddSamplePoint(3.0f, 0.5f);
    curve0->AddSamplePoint(4.0f, 0.25f);

    ysAnimationAction action1;
    action1.SetLength(4.0f);
    action1.SetName("Action1");

    ysAnimationCurve *curve1 = action1.NewCurve("Bone0");
    curve1->SetCurveType(ysAnimationCurve::CurveType::LocationX);
    curve1->AddSamplePoint(1.0f, 3.0f);
    curve1->AddSamplePoint(2.0f, 2.0f);
    curve1->AddSamplePoint(3.0f, 3.0f);
    curve1->AddSamplePoint(4.0f, 4.0f);

    TransformTarget bone0Loc;

    bone0Loc.ClearLocation(ysMath::LoadVector(1.0f, 2.0f, 3.0f, 0.0f));

    ysAnimationActionBinding binding0;
    binding0.SetAction(&action0);
    binding0.AddTarget("Bone0", &bone0Loc, nullptr);

    ysAnimationActionBinding binding1;
    binding1.SetAction(&action1);
    binding1.AddTarget("Bone0", &bone0Loc, nullptr);

    ysAnimationChannel::ActionSettings settings;
    settings.FadeIn = 0.0f;

    ysAnimationChannel mixer;
    mixer.AddSegment(&binding0, settings);

    mixer.Sample(); // t = 0
    VecEq(bone0Loc.GetLocationResult(), 
        ysMath::LoadVector(curve0->Sample(0.0f), 2.0f, 3.0f, 0.0f));
    bone0Loc.ClearFlags();

    settings.FadeIn = 0.5f;
    mixer.Advance(1.5f);
    mixer.AddSegment(&binding1, settings);

    mixer.Sample(); // t = 1.5
    VecEq(bone0Loc.GetLocationResult(), 
        ysMath::LoadVector(curve0->Sample(1.5f), 2.0f, 3.0f, 0.0f));
    bone0Loc.ClearFlags();

    mixer.Advance(0.25f); mixer.Sample(); // t = 1.75
    VecEq(bone0Loc.GetLocationResult(), 
        ysMath::LoadVector((curve0->Sample(1.75f) + curve1->Sample(0.25f)) / 2, 2.0f, 3.0f, 0.0f));
    bone0Loc.ClearFlags();

    mixer.Advance(0.25f); mixer.Sample(); // t = 2.0f
    VecEq(bone0Loc.GetLocationResult(),
        ysMath::LoadVector(curve1->Sample(1.0f), 2.0f, 3.0f, 0.0f));
    bone0Loc.ClearFlags();
}

TEST(AnimationTest, AnimationInterchangeFile) {
    ysAnimationInterchangeFile0_0 interchangeFile;
    interchangeFile.Open("../../../test/animation_files/armature_test.dimo");

    int actionCount = interchangeFile.GetActionCount();
    EXPECT_EQ(actionCount, 2);

    ysAnimationAction actions[2];
    for (int i = 0; i < actionCount; ++i) {
        interchangeFile.ReadAction(&actions[i]);
    }

    EXPECT_EQ(actions[0].GetName(), "Rise");
    EXPECT_EQ(actions[1].GetName(), "Twist");
}
