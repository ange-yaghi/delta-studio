#include "../include/yds_object_animation_data.h"

ysObjectAnimationData::ysObjectAnimationData() {
    Clear();
}

ysObjectAnimationData::~ysObjectAnimationData() {
    /* void */
}

void ysObjectAnimationData::Clear() {
    m_objectName[0] = '\0';
}
