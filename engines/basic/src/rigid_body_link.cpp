#include "../include/rigid_body_link.h"

dbasic::RigidBodyLink::RigidBodyLink() : ysObject("RigidBodyLink") {
    m_body1 = NULL;
    m_body2 = NULL;
}

dbasic::RigidBodyLink::~RigidBodyLink() {
    /* void */
}
