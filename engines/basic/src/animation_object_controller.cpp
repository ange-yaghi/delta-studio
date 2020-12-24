#include "../include/animation_object_controller.h"

#include "../include/delta_physics.h"

dbasic::AnimationObjectController::AnimationObjectController() : ysObject("AnimationObjectController") {
    m_target = nullptr;
    m_framerate = 30.0f;
    m_frame = 0;
    m_timeOffset = 0.0f;
}

dbasic::AnimationObjectController::~AnimationObjectController() {
    /* void */
}

void dbasic::AnimationObjectController::Update() {
    // Update rotation
    ysQuaternion currentRotation = m_target->GetOrientationParentSpace();
    ysQuaternion targetRotation;

    int low = -1;
    int high = -1;

    int nKeys = m_timeline.GetNumObjects();
    for (int i = 0; i < nKeys; i++) {
        if (m_timeline[i].GetKeyType() & ObjectKeyframeDataExport::KEY_FLAG_ROTATION_KEY) {
            if (m_timeline[i].GetFrame() <= m_frame) {
                low = i;
            }
            else {
                high = i;
                break;
            }
        }
    }

    if (low == -1) {
        targetRotation = m_timeline[high].GetRotationKey();
    }
    else if (high == -1) {
        targetRotation = m_timeline[low].GetRotationKey();
    }
    else {
        float interpDistance = m_timeOffset - m_timeline[low].GetFrame() / m_framerate;
        float totalDistance = (m_timeline[high].GetFrame() - m_timeline[low].GetFrame()) / m_framerate;

        float interp = interpDistance / totalDistance;

        ysVector vInterpLow = ysMath::LoadScalar(1.0f - interp);
        ysVector vInterpHigh = ysMath::LoadScalar(interp);

        ysQuaternion lowRot = m_timeline[low].GetRotationKey();
        ysQuaternion highRot = m_timeline[high].GetRotationKey();

        targetRotation = ysMath::Add(ysMath::Mul(vInterpLow, lowRot), ysMath::Mul(vInterpHigh, highRot));
    }

    m_target->SetOrientation(targetRotation);
}
