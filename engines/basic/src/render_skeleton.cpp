#include "../include/render_skeleton.h"

dbasic::RenderSkeleton::RenderSkeleton() {
    /* void */
}

dbasic::RenderSkeleton::~RenderSkeleton() {
    /* void */
}

dbasic::RenderNode *dbasic::RenderSkeleton::GetRoot() {
    return GetNode(0);
}

dbasic::RenderNode *dbasic::RenderSkeleton::NewNode() {
    RenderNode *newNode = m_renderNodes.NewGeneric<RenderNode, 16>();
    newNode->SetSkeleton(this);

    return newNode;
}

dbasic::RenderNode *dbasic::RenderSkeleton::GetNode(int index) {
    return m_renderNodes.Get(index);
}

dbasic::RenderNode *dbasic::RenderSkeleton::GetNode(const char *nodeName) {
    return FindNode(nodeName);
}

void dbasic::RenderSkeleton::BindAction(
    ysAnimationAction *action,
    ysAnimationActionBinding *binding) 
{
    binding->SetAction(action);

    const int nNodes = m_renderNodes.GetNumObjects();
    for (int i = 0; i < nNodes; i++) {
        RenderNode *node = m_renderNodes.Get(i);
        if (!node->IsBone()) continue;

        if (action->IsAnimated(node->GetName())) {
            binding->AddTarget(
                node->GetName(),
                node->GetLocationTarget(),
                node->GetRotationTarget());
        }
    }
}

dbasic::RenderNode *dbasic::RenderSkeleton::FindNode(const char *boneName) {
    const int nNodes = m_renderNodes.GetNumObjects();
    for (int i = 0; i < nNodes; i++) {
        if (strcmp(m_renderNodes.Get(i)->GetName(), boneName) == 0) {
            return m_renderNodes.Get(i);
        }
    }

    return nullptr;
}

void dbasic::RenderSkeleton::Update() {
    const int nNodes = m_renderNodes.GetNumObjects();
    for (int i = 0; i < nNodes; i++) {
        RenderNode *node = m_renderNodes.Get(i);
    }
}

void dbasic::RenderSkeleton::UpdateAnimation(float dt) {
    const int nNodes = m_renderNodes.GetNumObjects();
    for (int i = 0; i < nNodes; ++i) {
        RenderNode *node = m_renderNodes.Get(i);
        node->GetLocationTarget()->ClearFlags();
        node->GetRotationTarget()->ClearFlags();
    }

    AnimationMixer.Sample();
    AnimationMixer.Advance(dt);
    
    for (int i = 0; i < nNodes; ++i) {
        RenderNode *node = m_renderNodes.Get(i);
        TransformTarget *locTarget = node->GetLocationTarget();
        TransformTarget *rotTarget = node->GetRotationTarget();

        if (locTarget->IsAnimated()) {
            ysVector movementParentSpace = locTarget->GetLocationResult();
            node->Transform.SetPosition(ysMath::Add(node->GetRestLocation(), movementParentSpace));
        }

        if (rotTarget->IsAnimated()) {
            ysQuaternion r = rotTarget->GetQuaternionResult();
            const float mag = ysMath::GetScalar(ysMath::Magnitude(r));
            if (mag <= 1e-5) {
                r = node->GetLastValidOrientation();
            }
            else {
                node->SetLastValidOrientation(r);
            }

            ysQuaternion newOrientation = ysMath::Normalize(
                ysMath::QuatMultiply(node->GetRestOrientation(), ysMath::Normalize(r)));
            node->Transform.SetOrientation(newOrientation);
        }
    }
}
