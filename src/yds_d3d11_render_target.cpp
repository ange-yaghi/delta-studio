#include "../include/yds_d3d11_render_target.h"

ysD3D11RenderTarget::ysD3D11RenderTarget() : ysRenderTarget(DIRECTX11) {
	m_resourceView = NULL;
	m_renderTargetView = NULL;

	m_depthBuffer = NULL;
}

ysD3D11RenderTarget::~ysD3D11RenderTarget() {
    /* void */
}
