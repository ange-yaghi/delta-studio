#include "../include/pose.h"

dbasic::Pose::Pose() : ysObject("Pose") {
    m_name[0] = '\0';
    m_frame = -1;
}

dbasic::Pose::~Pose() {
    /* void */
}
