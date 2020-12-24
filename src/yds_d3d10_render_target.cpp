#include "../include/yds_d3d10_render_target.h"

ysD3D10RenderTarget::ysD3D10RenderTarget() : ysRenderTarget(DeviceAPI::DirectX10) {
    m_resourceView = NULL;
    m_renderTargetView = NULL;

    m_depthBuffer = NULL;
}

ysD3D10RenderTarget::~ysD3D10RenderTarget() {
    /* void */
}
