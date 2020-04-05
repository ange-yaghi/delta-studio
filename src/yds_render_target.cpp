#include "../include/yds_render_target.h"

ysRenderTarget::ysRenderTarget() : ysContextObject("RENDER_TARGET", API_UNKNOWN) {
    m_associatedContext = nullptr;
    m_depthBuffer = nullptr;
    m_parent = nullptr;
}

ysRenderTarget::ysRenderTarget(DEVICE_API API) : ysContextObject("RENDER_TARGET", API) {
    m_associatedContext = nullptr;
    m_depthBuffer = nullptr;
    m_parent = nullptr;
}

ysRenderTarget::~ysRenderTarget() {
    /* void */
}
