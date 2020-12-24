#include "../include/animation_export_data.h"

dbasic::AnimationExportData::AnimationExportData() : ysObject("AnimationExportData") {
    Clear();
}

dbasic::AnimationExportData::~AnimationExportData() {
    /* void */
}

void dbasic::AnimationExportData::Clear() {

    m_referenceFrame = 0;

    m_keyframes.Clear();
    m_poses.Clear();
    m_motions.Clear();

}

dbasic::ObjectKeyframeDataExport *dbasic::AnimationExportData::AddObjectKeyData(char *object) {
    ObjectKeyframeDataExport *keyframeData = &m_keyframes.New();

    strcpy_s(keyframeData->m_objectName, 64, object);

    return keyframeData;
}

void dbasic::AnimationExportData::AddMotion(char *name, int startFrame, int endFrame) {
    MotionExport *newMotion = &m_motions.New();

    newMotion->m_startFrame = startFrame + m_referenceFrame;
    newMotion->m_endFrame = endFrame + m_referenceFrame;
    strcpy_s(newMotion->m_name, 64, name);
}

dbasic::PoseExport *dbasic::AnimationExportData::AddPose(char *name, int frame) {
    PoseExport *newPose = &m_poses.New();

    newPose->m_frame = frame + m_referenceFrame;
    strcpy_s(newPose->m_name, 64, name);

    return newPose;
}

dbasic::MotionExport *dbasic::AnimationExportData::GetMotion(char *name) {
    int motionCount = GetMotionCount();

    for (int i = 0; i < motionCount; i++) {
        if (strcmp(name, m_motions[i].GetName()) == 0) {
            return &m_motions[i];
        }
    }

    return NULL;
}

ysError dbasic::AnimationExportData::LoadToolObjectData(ysObjectAnimationData *objectAnimationData) {
    YDS_ERROR_DECLARE("LoadToolObjectData");

    if (objectAnimationData == NULL) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    int positionKeyCount = objectAnimationData->m_positionKeys.NumKeys;
    int rotationKeyCount = objectAnimationData->m_rotationKeys.NumKeys;

    ObjectKeyframeDataExport *newKeyData = AddObjectKeyData(objectAnimationData->m_objectName);

    ObjectKeyframeDataExport::KEY_DATA *newKey = NULL;

    for (int i = 0; i < positionKeyCount; i++) {

        newKey = newKeyData->AddKey(ObjectKeyframeDataExport::KEY_FLAG_POSITION_KEY, objectAnimationData->m_positionKeys.Keys[i].Frame);
        newKey->m_positionKey = objectAnimationData->m_positionKeys.Keys[i].Position;

    }

    for (int i = 0; i < rotationKeyCount; i++) {

        newKey = newKeyData->AddKey(ObjectKeyframeDataExport::KEY_FLAG_ROTATION_KEY, objectAnimationData->m_positionKeys.Keys[i].Frame);
        newKey->m_rotationKey = objectAnimationData->m_rotationKeys.Keys[i].RotationQuaternion;

    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::AnimationExportData::LoadTimeTagData(ysTimeTagData *timeTagData) {
    YDS_ERROR_DECLARE("LoadTimeTagData");

    if (timeTagData == NULL) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    int tagCount = timeTagData->m_timeTagCount;

    for (int i = 0; i < tagCount; i++) {
        char name[128];
        bool start;

        int frame = timeTagData->m_timeTags[i].Frame;

        if (ParseMotionTag(timeTagData->m_timeTags[i].Name, name, start)) {
            MotionExport *existing = GetMotion(name);

            if (existing != NULL) {
                if (start) existing->m_startFrame = frame;
                else existing->m_endFrame = frame;
            }
            else {
                if (start) AddMotion(name, frame, -1);
                else AddMotion(name, -1, frame);
            }
        }
        else if (ParsePoseTag(timeTagData->m_timeTags[i].Name, name)) {
            AddPose(name, frame);
        }
    }

    return YDS_ERROR_RETURN(ysError::None);
}

bool dbasic::AnimationExportData::ParseMotionTag(const char *tag, char *name, bool &start) {
    char buffer[1024];
    int index = 0;

    if (!ReadName(tag, buffer, index)) return false;

    if (strcmp(buffer, "MOTION") != 0) return false;

    if (!ReadSeparator(tag, index)) return false;
    if (!ReadName(tag, name, index)) return false;
    if (!ReadSeparator(tag, index)) return false;
    if (!ReadName(tag, buffer, index)) return false;

    if (strcmp(buffer, "START") == 0) start = true;
    else if (strcmp(buffer, "END") == 0) start = false;
    else return false;

    return true;
}

bool dbasic::AnimationExportData::ParsePoseTag(const char *tag, char *name) {
    char buffer[1024];
    int index = 0;

    if (!ReadName(tag, buffer, index)) return false;

    if (strcmp(buffer, "POSE") != 0) return false;

    if (!ReadSeparator(tag, index)) return false;
    if (!ReadName(tag, name, index)) return false;

    return true;
}

bool dbasic::AnimationExportData::ReadSeparator(const char *tag, int &index) {
    if (tag[index] == ':') index++;
    else return false;

    if (tag[index] == ':') index++;
    else return false;

    return true;
}

bool dbasic::AnimationExportData::ReadName(const char *tag, char *name, int &index) {
    int length = 0;

    while (tag[index] != ':' && tag[index] != '\0') {
        name[length] = tag[index];
        index++;
        length++;
    }

    name[length] = '\0';

    return true;
}

// Pose Export Structure
dbasic::PoseExport::PoseExport() : ysObject("PoseExport") {
    m_name[0] = '\0';
    m_frame = -1;
}

dbasic::PoseExport::~PoseExport() {
    /* void */
}

// Keyframe Export Structure
dbasic::ObjectKeyframeDataExport::ObjectKeyframeDataExport() {
    m_objectName[0] = '\0';
}

dbasic::ObjectKeyframeDataExport::~ObjectKeyframeDataExport() {
    /* void */
}

dbasic::ObjectKeyframeDataExport::KEY_DATA *dbasic::ObjectKeyframeDataExport::AddKey(KEY_FLAG keyType, int frame) {
    int n_keys = m_keyData.GetNumObjects();

    KEY_DATA *data = NULL;

    for (int i = 0; i < n_keys; i++) {
        if (frame < m_keyData[i].m_frame) {
            data = &m_keyData.Insert(i);
            data->m_keyType = KEY_FLAG_UNDEFINED;
            break;
        }
        else if (frame == m_keyData[i].m_frame) {
            data = &m_keyData[i];
            break;
        }
    }

    if (data == NULL) {
        // Insert a new key at the end
        data = &m_keyData.New();
        data->m_keyType = KEY_FLAG_UNDEFINED;
    }

    data->m_keyType |= keyType;
    data->m_frame = frame;

    return data;
}

// Motion Export Structure
dbasic::MotionExport::MotionExport() {
    Clear();
}

dbasic::MotionExport::~MotionExport() {
    /* void */
}

void dbasic::MotionExport::Clear() {
    m_name[0] = '\0';

    m_startFrame = -1;
    m_endFrame = -1;
}
