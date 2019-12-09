#include "../include/render_skeleton.h"

dbasic::RenderSkeleton::RenderSkeleton() {
    /* void */
}

dbasic::RenderSkeleton::~RenderSkeleton() {
    /* void */
}

dbasic::RenderNode *dbasic::RenderSkeleton::NewNode() {
    RenderNode *newNode = m_renderNodes.NewGeneric<RenderNode, 16>();
    newNode->SetSkeleton(this);

    return newNode;
}

dbasic::RenderNode *dbasic::RenderSkeleton::GetNode(int index) {
    return m_renderNodes.Get(index);
}

dbasic::RenderNode *dbasic::RenderSkeleton::FindNode(const char *boneName) {
    int nNodes = m_renderNodes.GetNumObjects();

    for (int i = 0; i < nNodes; i++) {
        if (strcmp(m_renderNodes.Get(i)->GetName(), boneName) == 0) {
            return m_renderNodes.Get(i);
        }
    }

    return NULL;
}

void dbasic::RenderSkeleton::Update() {
    int nNodes = m_renderNodes.GetNumObjects();

    for (int i = 0; i < nNodes; i++) {
        RenderNode *node = m_renderNodes.Get(i);
        node->RigidBody.UpdateDerivedData();
    }
}
