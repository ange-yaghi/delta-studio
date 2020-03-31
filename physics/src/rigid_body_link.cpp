#include "../include/rigid_body_link.h"

dphysics::RigidBodyLink::RigidBodyLink() : ysObject("RigidBodyLink") {
    m_body1 = NULL;
    m_body2 = NULL;
}

dphysics::RigidBodyLink::~RigidBodyLink() {
    /* void */
}
