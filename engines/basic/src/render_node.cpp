#include "../include/render_node.h"

dbasic::RenderNode::RenderNode() : ysObject("RenderNode") {
    m_name[0] = '\0';

    m_parent = nullptr;
    m_renderSkeleton = nullptr;
    m_modelAsset = nullptr;
    m_assetID = -1;

    m_locationTarget.ClearLocation(ysMath::Constants::Zero);
    m_rotationTarget.ClearRotation(ysMath::Constants::QuatIdentity);

    m_restLocation = ysMath::Constants::Zero;
    m_restOrientation = ysMath::Constants::QuatIdentity;
    m_lastValidOrientation = ysMath::Constants::QuatIdentity;
}

dbasic::RenderNode::~RenderNode() {
    /* void */
}

void dbasic::RenderNode::SetParent(RenderNode *node) {
    m_parent = node;
    if (node != nullptr) Transform.SetParent(&node->Transform);
}

dbasic::RenderNode *dbasic::RenderNode::GetParent() {
    return m_parent;
}
