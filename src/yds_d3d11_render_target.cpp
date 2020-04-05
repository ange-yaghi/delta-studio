#include "../include/yds_d3d11_render_target.h"

#include <assert.h>

ysD3D11RenderTarget::ysD3D11RenderTarget() : ysRenderTarget(DIRECTX11) {
	m_resourceView = nullptr;
	m_renderTargetView = nullptr;

	m_depthBuffer = nullptr;

	m_depthStencilView = nullptr;

	m_depthTestEnabledState = nullptr;
	m_depthTestDisabledState = nullptr;
}

ysD3D11RenderTarget::~ysD3D11RenderTarget() {
	assert(m_resourceView == nullptr);
	assert(m_renderTargetView == nullptr);
	assert(m_depthStencilView == nullptr);
	assert(m_depthTestEnabledState == nullptr);
	assert(m_depthTestDisabledState == nullptr);
}
