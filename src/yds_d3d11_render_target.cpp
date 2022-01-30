#include "../include/yds_d3d11_render_target.h"

#include "../include/yds_d3d11_utilities.h"

#include <assert.h>

ysD3D11RenderTarget::ysD3D11RenderTarget() : ysRenderTarget(DeviceAPI::DirectX11) {
    m_resourceView = nullptr;
    m_renderTargetView = nullptr;

    m_depthBuffer = nullptr;

    m_depthStencilView = nullptr;

    m_depthTestEnabledState = nullptr;
    m_depthTestDisabledState = nullptr;
    m_texture = nullptr;
}

ysD3D11RenderTarget::~ysD3D11RenderTarget() {
    assert(m_resourceView == nullptr);
    assert(m_renderTargetView == nullptr);
    assert(m_depthStencilView == nullptr);
    assert(m_depthTestEnabledState == nullptr);
    assert(m_depthTestDisabledState == nullptr);
    assert(m_texture == nullptr);
}

void ysD3D11RenderTarget::SetDebugName(const std::string &debugName) {
    ysRenderTarget::SetDebugName(debugName);

    if (m_depthBuffer != nullptr) m_depthBuffer->SetDebugName(debugName + "_DEPTH_BUFFER");

    D3D11SetDebugName(m_resourceView, debugName + "_RESOURCE_VIEW");
    D3D11SetDebugName(m_renderTargetView, debugName + "_RENDER_TARGET_VIEW");
    D3D11SetDebugName(m_depthStencilView, debugName + "_DEPTH_STENCIL_VIEW");

    D3D11SetDebugName(m_depthTestEnabledState, debugName + "_DEPTH_TEST_ENABLED_STATE");
    D3D11SetDebugName(m_depthTestDisabledState, debugName + "_DEPTH_TEST_DISABLED_STATE");
}
