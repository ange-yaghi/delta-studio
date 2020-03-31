#include "../include/render_node.h"

dbasic::RenderNode::RenderNode() : ysObject("RenderNode") {
    m_parent = nullptr;
    m_renderSkeleton = nullptr;
    m_modelAsset = nullptr;
}

dbasic::RenderNode::~RenderNode() {
    /* void */
}

void dbasic::RenderNode::SetParent(RenderNode *node) {
    m_parent = node;

    if (m_parent != nullptr) {
        m_parent->RigidBody.AddChild(&RigidBody);
    }
}

dbasic::RenderNode *dbasic::RenderNode::GetParent() {
    return m_parent;
}
