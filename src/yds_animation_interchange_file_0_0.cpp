#include "../include/yds_animation_interchange_file_0_0.h"

#include "../include/yds_animation_action.h"

ysAnimationInterchangeFile0_0::ysAnimationInterchangeFile0_0() {
    m_majorVersion = 0;
    m_minorVersion = 0;

    m_toolId = -1;
    m_compilationStatus = false;
    m_actionCount = 0;
}

ysAnimationInterchangeFile0_0::~ysAnimationInterchangeFile0_0() {
    /* void */
}

ysError ysAnimationInterchangeFile0_0::Open(const char *fname) {
    YDS_ERROR_DECLARE("Open");

    m_file.open(fname, std::ios::binary | std::ios::in | std::ios::out);
    if (!m_file.is_open()) return YDS_ERROR_RETURN_MSG(ysError::YDS_COULD_NOT_OPEN_FILE, fname);

    IdHeader idHeader;
    m_file.read((char *)&idHeader, sizeof(IdHeader));

    if (idHeader.MagicNumber != MAGIC_NUMBER) {
        m_file.close();
        return YDS_ERROR_RETURN(ysError::YDS_INVALID_FILE_TYPE);
    }

    if (idHeader.MinorVersion != MINOR_VERSION || idHeader.MajorVersion != MAJOR_VERSION) {
        m_file.close();
        return YDS_ERROR_RETURN(ysError::YDS_UNSUPPORTED_FILE_VERSION);
    }

    m_compilationStatus = idHeader.CompilationStatus == 0x0;
    m_toolId = idHeader.EditorId;

    FileHeader fileHeader;
    m_file.read((char *)&fileHeader, sizeof(FileHeader));

    m_actionCount = fileHeader.ActionCount;

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysAnimationInterchangeFile0_0::Close() {
    YDS_ERROR_DECLARE("Close");

    if (m_file.is_open()) m_file.close();

    m_majorVersion = -1;
    m_minorVersion = -1;

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysAnimationCurve::CurveType ysAnimationInterchangeFile0_0::InterpretCurveType(unsigned int curveType) {
    switch (curveType) {
    case 0x1:
        return ysAnimationCurve::CurveType::LocationVec;
    case 0x2:
        return ysAnimationCurve::CurveType::RotationQuat;
    case 0x3:
        return ysAnimationCurve::CurveType::LocationX;
    case 0x4:
        return ysAnimationCurve::CurveType::LocationY;
    case 0x5:
        return ysAnimationCurve::CurveType::LocationZ;
    case 0x6:
        return ysAnimationCurve::CurveType::RotationQuatW;
    case 0x7:
        return ysAnimationCurve::CurveType::RotationQuatX;
    case 0x8:
        return ysAnimationCurve::CurveType::RotationQuatY;
    case 0x9:
        return ysAnimationCurve::CurveType::RotationQuatZ;
    case 0x0:
    default:
        return ysAnimationCurve::CurveType::Undefined;
    }
}

ysError ysAnimationInterchangeFile0_0::ReadAction(ysAnimationAction *action) {
    YDS_ERROR_DECLARE("ReadAction");

    ActionHeader actionHeader;
    m_file.read((char *)&actionHeader, sizeof(ActionHeader));

    action->SetName(actionHeader.Name);
    
    int curveCount = actionHeader.CurveCount;
    for (int i = 0; i < curveCount; ++i) {
        CurveHeader curveHeader;
        m_file.read((char *)&curveHeader, sizeof(CurveHeader));

        ysAnimationCurve *newCurve = action->NewCurve(curveHeader.TargetBone);
        newCurve->SetCurveType(InterpretCurveType(curveHeader.CurveType));
        
        int keyframeCount = curveHeader.KeyframeCount;
        for (int i = 0; i < keyframeCount; ++i) {
            Keyframe keyframe;
            m_file.read((char *)&keyframe, sizeof(Keyframe));

            newCurve->AddSamplePoint(keyframe.Timestamp, keyframe.Value);
        }
    }

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}
