#include "../include/rigid_body_link.h"

dphysics::RigidBodyLink::RigidBodyLink() : ysObject("RigidBodyLink") {
    m_body1 = nullptr;
    m_body2 = nullptr;
}

dphysics::RigidBodyLink::~RigidBodyLink() {
    /* void */
}
