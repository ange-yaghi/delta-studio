#ifndef YDS_RENDER_TARGET_H
#define YDS_RENDER_TARGET_H

#include "yds_context_object.h"
#include "yds_depth_buffer.h"

class ysRenderingContext;
class ysRenderTarget : public ysContextObject {
    friend class ysRenderingContext;
    friend class ysDevice;

public:
    enum RENDER_TARGET_TYPE {
        RENDER_TARGET_ON_SCREEN,
        RENDER_TARGET_OFF_SCREEN,
        RENDER_TARGET_SUBDIVISION,

        RENDER_TARGET_UNDEFINED
    };

    enum RENDER_TARGET_FORMAT {
        RTF_R8G8B8A8_UNORM,
        RTF_R32G32B32_FLOAT
    };

public:
    ysRenderTarget();
    ysRenderTarget(DEVICE_API API);
    virtual ~ysRenderTarget();

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    int GetPosX() const { return m_posX; }
    int GetPosY() const { return m_posY; }

    RENDER_TARGET_FORMAT GetFormat() const { return m_format; }
    RENDER_TARGET_TYPE GetType() const { return m_type; }

    int GetSampleCount() const { return m_sampleCount; }
    bool HasDepthBuffer() const { return m_hasDepthBuffer; }

    ysRenderingContext *GetAssociatedContext() { return m_associatedContext; }
    ysRenderTarget *GetParent() { return m_parent; }

protected:
    int m_posX;
    int m_posY;

    int m_width;
    int m_height;

    RENDER_TARGET_TYPE m_type;
    RENDER_TARGET_FORMAT m_format;

    int m_sampleCount;
    bool m_hasDepthBuffer;

    ysRenderTarget *m_parent;
    ysRenderingContext *m_associatedContext;
    ysDepthBuffer *m_depthBuffer;
};

#endif /* YDS_RENDER_TARGET_H */
