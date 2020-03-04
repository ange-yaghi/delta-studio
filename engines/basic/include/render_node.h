#ifndef DELTA_BASIC_RENDER_NODE_H
#define DELTA_BASIC_RENDER_NODE_H

#include "delta_core.h"

#include "rigid_body.h"

namespace dbasic {

    class RenderSkeleton;
    class ModelAsset;

    class RenderNode : public ysObject {
    public:
        RenderNode();
        ~RenderNode();

        // Rigid body component
        RigidBody RigidBody;

        void SetParent(RenderNode *node);
        RenderNode *GetParent();

        const char *GetName() const { return m_name; }
        void SetName(const char *name) { strcpy_s(m_name, 64, name); }

        RenderSkeleton *GetSkeleton() { return m_renderSkeleton; }
        void SetSkeleton(RenderSkeleton *skeleton) { m_renderSkeleton = skeleton; }

        ModelAsset *GetModelAsset() { return m_modelAsset; }
        void SetModelAsset(ModelAsset *modelAsset) { m_modelAsset = modelAsset; }

        void SetAssetID(int assetID) { m_assetID = assetID; }
        int GetAssetID() const { return m_assetID; }

    protected:
        // Node name
        char m_name[64];

        // Convenient reference to the asset ID this node is derived from
        int m_assetID;

        // Parent node
        RenderNode *m_parent;

        // Parent render skeleton
        RenderSkeleton *m_renderSkeleton;

        // Model asset
        ModelAsset *m_modelAsset;
    };

} /* namespace dbasic */

#endif /* DELTA_BASIC_RENDER_NODE_H */
