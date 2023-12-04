#ifndef YDS_RENDER_TARGET_H
#define YDS_RENDER_TARGET_H

#include "yds_context_object.h"
#include "yds_depth_buffer.h"

class ysRenderingContext;
class ysRenderTarget : public ysContextObject {
    friend class ysRenderingContext;
    friend class ysDevice;

public:
    enum class Type {
        OnScreen,
        OffScreen,
        Subdivision,

        Undefined
    };

    enum class Format {
        R8G8B8A8_UNORM,
        R32G32B32_FLOAT,
        R32_DEPTH_COMPONENT,
        R32_FLOAT
    };

public:
    ysRenderTarget();
    ysRenderTarget(DeviceAPI API);
    virtual ~ysRenderTarget();

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    int GetPhysicalWidth() const { return m_physicalWidth; }
    int GetPhysicalHeight() const { return m_physicalHeight; }

    int GetPosX() const { return m_posX; }
    int GetPosY() const { return m_posY; }

    Format GetFormat() const { return m_format; }
    Type GetType() const { return m_type; }

    int GetSampleCount() const { return m_sampleCount; }
    bool HasDepthBuffer() const { return m_hasDepthBuffer; }

    bool HasColorData() const { return m_hasColorData; }

    inline ysRenderingContext *GetAssociatedContext() {
        return m_associatedContext;
    }

    inline ysRenderTarget *GetParent() const { return m_parent; }
    inline ysRenderTarget *GetRoot() {
        return (GetParent() != this) ? GetParent()->GetRoot() : this;
    }

    bool IsDepthTestEnabled() const { return m_depthTestEnabled; }
    void SetDepthTestEnabled(bool enable) { m_depthTestEnabled = enable; }

    inline int GetMsaa() const { return m_msaa; }

protected:
    int m_msaa = 1;

    int m_posX;
    int m_posY;

    int m_width;
    int m_height;

    int m_physicalWidth;
    int m_physicalHeight;

    Type m_type;
    Format m_format;

    int m_sampleCount;
    bool m_hasColorData;
    bool m_hasDepthBuffer;
    bool m_depthTestEnabled;

    ysRenderTarget *m_parent;
    ysRenderingContext *m_associatedContext;
    ysDepthBuffer *m_depthBuffer;
};

#endif /* YDS_RENDER_TARGET_H */
