#include "../include/keyframe.h"

#include "../include/animation_export_data.h"

dbasic::Keyframe::Keyframe() : ysObject("Keyframe") {
    m_frame = -1;
    m_keyType = ObjectKeyframeDataExport::KEY_FLAG_UNDEFINED;

    m_positionKey = ysMath::Constants::Zero;
    m_rotationKey = ysMath::Constants::Zero;
}

dbasic::Keyframe::~Keyframe() {
    /* void */
}

void dbasic::Keyframe::LoadAssetKeyframe(ObjectKeyframeDataExport::KEY_DATA *data) {
    m_frame = data->m_frame;
    m_keyType = data->m_keyType;
    m_positionKey = ysMath::LoadVector(data->m_positionKey);
    m_rotationKey = ysMath::LoadVector(data->m_rotationKey);
}
