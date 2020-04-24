#include <pch.h>

#include "../include/yds_animation_action.h"
#include "../include/yds_animation_action_binding.h"
#include "../include/yds_animation_mixer.h"
#include "../include/yds_animation_curve.h"
#include "../include/yds_animation_target.h"
#include "../include/yds_animation_interchange_file.h"

#include "utilities.h"

#include <fstream>

constexpr float Epsilon = 1E-5f;

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
    mixer.QueueSegment(&binding, settings);
    
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

TEST(AnimationTest, LoopInterruption) {
    ysAnimationChannel mixer;

    ysAnimationAction action;
    action.SetLength(10.0f);

    ysAnimationActionBinding binding;
    binding.SetAction(&action);

    EXPECT_NEAR(mixer.ProbeTotalAmplitude(), 0.0f, Epsilon);

    ysAnimationChannel::ActionSettings settings;
    settings.Speed = 3.0f;
    settings.FadeIn = 0.5f;

    mixer.AddSegment(&binding, settings);
    mixer.QueueSegment(&binding, settings);

    mixer.Sample(); mixer.Advance(0.5f);
    EXPECT_NEAR(mixer.ProbeTotalAmplitude(), 0.0f, Epsilon);

    for (int i = 0; i < 1000; ++i) {
        if (i == 78) {
            int breakHere = 0;
        }

        mixer.Advance(0.001f); 
        mixer.Sample();
        if (std::isnan(mixer.ProbeTotalAmplitude())) {
            int breakHere = 0;
        }

        if (std::abs(mixer.ProbeTotalAmplitude() - 1.0f) > Epsilon) {
            int breakHere = 0;
            break;
        }

        EXPECT_NEAR(mixer.ProbeTotalAmplitude(), 1.0f, Epsilon);

        if (rand() % 100 == 0) {
            mixer.AddSegment(&binding, settings);
        }
        else if (rand() % 100 == 1) {
            mixer.QueueSegment(&binding, settings);
        }
    }
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
    curve.AddLinearSamplePoint(1.0f, 0.1f);
    curve.AddLinearSamplePoint(2.0f, 1.0f);
    curve.AddLinearSamplePoint(3.0f, 0.5f);
    curve.AddLinearSamplePoint(4.0f, 0.25f);

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
    curve0->AddLinearSamplePoint(1.0f, 0.1f);
    curve0->AddLinearSamplePoint(2.0f, 1.0f);
    curve0->AddLinearSamplePoint(3.0f, 0.5f);
    curve0->AddLinearSamplePoint(4.0f, 0.25f);

    ysAnimationCurve *curve1 = action.NewCurve("Bone1");
    curve1->SetCurveType(ysAnimationCurve::CurveType::LocationY);
    curve1->AddLinearSamplePoint(1.0f, 0.5f);
    curve1->AddLinearSamplePoint(2.0f, 0.5f);
    curve1->AddLinearSamplePoint(3.0f, 0.5f);
    curve1->AddLinearSamplePoint(4.0f, 0.25f);

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
    curve0->AddLinearSamplePoint(1.0f, 0.5f);
    curve0->AddLinearSamplePoint(2.0f, 1.0f);
    curve0->AddLinearSamplePoint(3.0f, 0.5f);
    curve0->AddLinearSamplePoint(4.0f, 0.25f);

    ysAnimationAction action1;
    action1.SetLength(4.0f);
    action1.SetName("Action1");

    ysAnimationCurve *curve1 = action1.NewCurve("Bone0");
    curve1->SetCurveType(ysAnimationCurve::CurveType::LocationX);
    curve1->AddLinearSamplePoint(1.0f, 3.0f);
    curve1->AddLinearSamplePoint(2.0f, 2.0f);
    curve1->AddLinearSamplePoint(3.0f, 3.0f);
    curve1->AddLinearSamplePoint(4.0f, 4.0f);

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
    ysAnimationInterchangeFile interchangeFile;
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

TEST(AnimationTest, BezierTest) {
    ysAnimationInterchangeFile interchangeFile;
    interchangeFile.Open("../../../test/animation_files/bezier_test.dimo");

    int actionCount = interchangeFile.GetActionCount();
    EXPECT_EQ(actionCount, 1);

    ysAnimationAction actions[1];
    for (int i = 0; i < actionCount; ++i) {
        interchangeFile.ReadAction(&actions[i]);
    }

    EXPECT_EQ(actions[0].GetName(), "TestAction");

    ysAnimationCurve *curve = actions[0].GetCurve("Bone", ysAnimationCurve::CurveType::LocationY);
    EXPECT_NE(curve, nullptr);

    EXPECT_NEAR(curve->Sample(0.0f), 0.0f, 1E-4);
    EXPECT_NEAR(curve->Sample(10.0f), 0.527372f, 1E-4);
    EXPECT_NEAR(curve->Sample(20.0f), 1.14506f, 1E-4);
    EXPECT_NEAR(curve->Sample(36.0f), 0.414749f, 1E-4);
    EXPECT_NEAR(curve->Sample(46.0f), 0.079635f, 1E-4);
    EXPECT_NEAR(curve->Sample(70.0f), 0.0f, 1E-4);
}
