#include "../include/yds_render_target.h"

ysRenderTarget::ysRenderTarget() : ysContextObject("RENDER_TARGET", API_UNKNOWN) {
    m_associatedContext = NULL;
    m_depthBuffer = NULL;
    m_parent = NULL;
}

ysRenderTarget::ysRenderTarget(DEVICE_API API) : ysContextObject("RENDER_TARGET", API) {
    m_associatedContext = NULL;
    m_depthBuffer = NULL;
    m_parent = NULL;
}

ysRenderTarget::~ysRenderTarget() {
    /* void */
}
