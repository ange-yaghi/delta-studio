#include "yds_animation_interchange_file_reader_0_1.h"

#include "yds_animation_action.h"
#include "yds_error_system.h"

ysAnimationInterchangeFileReader_0_1::ysAnimationInterchangeFileReader_0_1()
    : m_actionCount(0)
{ }

unsigned int ysAnimationInterchangeFileReader_0_1::GetActionCount() const
{
    return m_actionCount;
}

ysError ysAnimationInterchangeFileReader_0_1::ReadHeader(std::fstream &f) {
    YDS_ERROR_DECLARE("ReadHeader");

    FileHeader fileHeader;
    f.read((char *)&fileHeader, sizeof(FileHeader));

    m_actionCount = fileHeader.ActionCount;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysAnimationInterchangeFileReader_0_1::ReadAction(std::fstream &f, ysAnimationAction *action) {
    YDS_ERROR_DECLARE("ReadAction");

    ActionHeader actionHeader;
    f.read((char *)&actionHeader, sizeof(ActionHeader));

    action->SetName(actionHeader.Name);

    int curveCount = actionHeader.CurveCount;
    for (int i = 0; i < curveCount; ++i) {
        CurveHeader curveHeader;
        f.read((char *)&curveHeader, sizeof(CurveHeader));

        ysAnimationCurve *newCurve = action->NewCurve(curveHeader.TargetBone);
        newCurve->SetCurveType(InterpretCurveType(curveHeader.CurveType));

        int keyframeCount = curveHeader.KeyframeCount;
        for (int i = 0; i < keyframeCount; ++i) {
            Keyframe keyframe;
            f.read((char *)&keyframe, sizeof(Keyframe));

            ysAnimationCurve::CurveHandle handle;
            handle.mode =
                ysAnimationCurve::CurveHandle::InterpolationMode::Linear;
            handle.s = keyframe.Timestamp;
            handle.v = keyframe.Value;

            handle.mode = InterpretInterpolationMode(keyframe.InterpolationMode);

            handle.l_handle_x = keyframe.l_handle_x;
            handle.l_handle_y = keyframe.l_handle_y;

            handle.r_handle_x = keyframe.r_handle_x;
            handle.r_handle_y = keyframe.r_handle_y;

            newCurve->AddSamplePoint(handle);
        }
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysAnimationCurve::CurveType ysAnimationInterchangeFileReader_0_1::InterpretCurveType(unsigned int curveType) {
    switch (curveType) {
    case 0x1: return ysAnimationCurve::CurveType::LocationVec;
    case 0x2: return ysAnimationCurve::CurveType::RotationQuat;
    case 0x3: return ysAnimationCurve::CurveType::LocationX;
    case 0x4: return ysAnimationCurve::CurveType::LocationY;
    case 0x5: return ysAnimationCurve::CurveType::LocationZ;
    case 0x6: return ysAnimationCurve::CurveType::RotationQuatW;
    case 0x7: return ysAnimationCurve::CurveType::RotationQuatX;
    case 0x8: return ysAnimationCurve::CurveType::RotationQuatY;
    case 0x9: return ysAnimationCurve::CurveType::RotationQuatZ;
    case 0x0:
    default: return ysAnimationCurve::CurveType::Undefined;
    }
}

ysAnimationCurve::CurveHandle::InterpolationMode 
ysAnimationInterchangeFileReader_0_1::InterpretInterpolationMode(unsigned int interpolationMode)
{
    switch (interpolationMode) {
    case 0x0: return ysAnimationCurve::CurveHandle::InterpolationMode::Bezier;
    case 0x1: return ysAnimationCurve::CurveHandle::InterpolationMode::Linear;
    default: return ysAnimationCurve::CurveHandle::InterpolationMode::Linear;
    }
}
