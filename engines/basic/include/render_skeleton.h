#ifndef DELTA_BASIC_RENDER_SKELETON_H
#define DELTA_BASIC_RENDER_SKELETON_H

#include "delta_core.h"

#include "rigid_body.h"
#include "render_node.h"

namespace dbasic {

    class RenderSkeleton : public ysObject {
    public:
        RenderSkeleton();
        ~RenderSkeleton();

        RenderNode *NewNode();
        RenderNode *GetNode(int index);
        RenderNode *GetNode(const char *nodeName);

        int GetNodeCount() const { return m_renderNodes.GetNumObjects(); }
        RenderNode *FindNode(const char *nodeName);

        void Update();

    protected:
        // Container for all nodes
        ysDynamicArray<RenderNode, 4> m_renderNodes;
    };
    
} /* namespace dbasic */

#endif /* DELTA_BASIC_RENDER_SKELETON_H */
