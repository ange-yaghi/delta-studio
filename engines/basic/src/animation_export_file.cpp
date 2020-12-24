#include "../include/animation_export_file.h"

dbasic::AnimationExportFile::AnimationExportFile() : ysObject("ysAnimationExportFile") {
    m_openMode = Mode::Closed;
    m_fileVersion = -1;
}

dbasic::AnimationExportFile::~AnimationExportFile() {
    /* void */
}

ysError dbasic::AnimationExportFile::Open(const char *fname, Mode mode) {
    YDS_ERROR_DECLARE("Open");

    if (mode != Mode::Write &&
        mode != Mode::Read) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    if (mode == Mode::Write)    m_file.open(fname, std::ios::binary | std::ios::out);
    if (mode == Mode::Read)        m_file.open(fname, std::ios::binary | std::ios::in);

    if (!m_file.is_open()) return YDS_ERROR_RETURN(ysError::CouldNotOpenFile);

    if (mode == Mode::Read) {
        // Read header
        ExportFileHeader header;
        m_file.read((char *)&header, sizeof(ExportFileHeader));

        if (!m_file) {
            m_file.close();
            return YDS_ERROR_RETURN(ysError::InvalidFileType);
        }

        if (header.FileMagicNumber != MAGIC_NUMBER) {
            m_file.close();
            return YDS_ERROR_RETURN(ysError::InvalidFileType);
        }

        if (header.FileVersion < 0 || header.FileVersion > FILE_VERSION) {
            m_file.close();
            return YDS_ERROR_RETURN(ysError::UnsupportedFileVersion);
        }

        m_fileVersion = header.FileVersion;
    }
    else if (mode == Mode::Write) {
        m_fileVersion = FILE_VERSION;

        ExportFileHeader header;
        header.FileMagicNumber = MAGIC_NUMBER;
        header.FileVersion = m_fileVersion;

        m_file.write((char *)&header, sizeof(ExportFileHeader));
    }

    m_openMode = mode;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::AnimationExportFile::WriteCustomData(void *data, int size) {
    YDS_ERROR_DECLARE("WriteCustomData");

    if (!m_file.is_open()) return YDS_ERROR_RETURN(ysError::NoFile);
    if (data == NULL) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (m_openMode != Mode::Write) return YDS_ERROR_RETURN(ysError::InvalidOperation);

    m_file.write((char *)data, size);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::AnimationExportFile::WriteObjectAnimationData(AnimationExportData *objectAnimationData) {
    YDS_ERROR_DECLARE("WriteObjectAnimationData");

    if (!m_file.is_open()) return YDS_ERROR_RETURN(ysError::NoFile);
    if (objectAnimationData == NULL) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (m_openMode != Mode::Write) return YDS_ERROR_RETURN(ysError::InvalidOperation);

    // Write Keys First
    KeyframeSectionHeader keyHeader;
    keyHeader.KeyframeCount = objectAnimationData->GetKeyCount();

    m_file.write((char *)&keyHeader, sizeof(KeyframeSectionHeader));

    for (int i = 0; i < keyHeader.KeyframeCount; i++) {
        ObjectKeyframeDataExport *keyData = objectAnimationData->GetKey(i);

        KeyHeader keyHeader;
        strcpy_s(keyHeader.ObjectName, 64, keyData->GetObjectName());
        keyHeader.KeyCount = keyData->GetKeyCount();

        m_file.write((char *)&keyHeader, sizeof(KeyHeader));
        m_file.write((char *)keyData->GetKeyBuffer(), sizeof(ObjectKeyframeDataExport::KEY_DATA) * (std::streamsize)keyData->GetKeyCount());
    }

    // Write Poses
    int poseCount = objectAnimationData->GetPoseCount();

    PoseSectionHeader poseSectionHeader;
    poseSectionHeader.PoseCount = poseCount;

    m_file.write((char *)&poseSectionHeader, sizeof(PoseSectionHeader));

    for (int i = 0; i < poseCount; i++) {
        PoseExport *pose = objectAnimationData->GetPose(i);

        PoseOutputHeader poseHeader;
        strcpy_s(poseHeader.PoseName, 64, pose->GetName());
        poseHeader.Frame = pose->GetFrame();

        m_file.write((char *)&poseHeader, sizeof(PoseOutputHeader));
    }

    // Write motions
    int motionCount = objectAnimationData->GetMotionCount();

    MotionSectionHeader motionSectionHeader;
    motionSectionHeader.MotionCount = motionCount;

    m_file.write((char *)&motionSectionHeader, sizeof(MotionSectionHeader));

    for (int i = 0; i < motionCount; i++) {
        MotionExport *motion = objectAnimationData->GetMotion(i);

        MotionOutputHeader motionHeader;
        strcpy_s(motionHeader.MotionName, 64, motion->GetName());
        motionHeader.EndFrame = motion->GetEndFrame();
        motionHeader.StartFrame = motion->GetStartFrame();

        m_file.write((char *)&motionHeader, sizeof(MotionOutputHeader));
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::AnimationExportFile::ReadObjectAnimationData(AnimationExportData *objectAnimationData) {
    YDS_ERROR_DECLARE("ReadObjectAnimationData");

    if (!m_file.is_open()) return YDS_ERROR_RETURN(ysError::NoFile);
    if (objectAnimationData == NULL) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (m_openMode != Mode::Read) return YDS_ERROR_RETURN(ysError::InvalidOperation);

    // Read Keys First
    KeyframeSectionHeader keyHeader;

    m_file.read((char *)&keyHeader, sizeof(KeyframeSectionHeader));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    for (int i = 0; i < keyHeader.KeyframeCount; i++) {
        KeyHeader keyHeader;
        m_file.read((char *)&keyHeader, sizeof(KeyHeader));
        if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

        ObjectKeyframeDataExport *newKeyframeData = objectAnimationData->AddObjectKeyData(keyHeader.ObjectName);

        newKeyframeData->PreallocateKeys(keyHeader.KeyCount);

        if (keyHeader.KeyCount > 0) {
            m_file.read((char *)newKeyframeData->GetKeyBuffer(), sizeof(ObjectKeyframeDataExport::KEY_DATA) * keyHeader.KeyCount);
        }

        if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);
    }

    // Read Poses

    PoseSectionHeader poseSectionHeader;
    m_file.read((char *)&poseSectionHeader, sizeof(PoseSectionHeader));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    int poseCount = poseSectionHeader.PoseCount;

    for (int i = 0; i < poseCount; i++) {
        PoseOutputHeader poseHeader;
        m_file.read((char *)&poseHeader, sizeof(PoseOutputHeader));
        if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

        PoseExport *newPose = objectAnimationData->AddPose(poseHeader.PoseName, poseHeader.Frame);
    }

    // Read motions

    MotionSectionHeader motionSectionHeader;

    m_file.read((char *)&motionSectionHeader, sizeof(MotionSectionHeader));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    int motionCount = motionSectionHeader.MotionCount;

    for (int i = 0; i < motionCount; i++) {
        MotionOutputHeader motionHeader;
        m_file.read((char *)&motionHeader, sizeof(MotionOutputHeader));
        if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

        objectAnimationData->AddMotion(motionHeader.MotionName, motionHeader.StartFrame, motionHeader.EndFrame);
    }

    return YDS_ERROR_RETURN(ysError::None);
}


void dbasic::AnimationExportFile::Close() {
    m_file.close();
}
