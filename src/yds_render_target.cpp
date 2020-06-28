#include "../include/yds_render_target.h"

ysRenderTarget::ysRenderTarget() : ysContextObject("RENDER_TARGET", DeviceAPI::Unknown) {
    m_associatedContext = nullptr;
    m_depthBuffer = nullptr;
    m_parent = nullptr;
    m_depthTestEnabled = false;
}

ysRenderTarget::ysRenderTarget(DeviceAPI API) : ysContextObject("RENDER_TARGET", API) {
    m_associatedContext = nullptr;
    m_depthBuffer = nullptr;
    m_parent = nullptr;
    m_depthTestEnabled = false;
}

ysRenderTarget::~ysRenderTarget() {
    /* void */
}
