#include "../include/yds_d3d11_context.h"

#include "../include/yds_d3d11_device.h"
#include "../include/yds_d3d11_render_target.h"

ysD3D11Context::ysD3D11Context()
    : ysRenderingContext(ysDevice::DeviceAPI::DirectX11,
                         ysWindowSystemObject::Platform::Windows) {
    m_swapChain = nullptr;
    m_swapChain1 = nullptr;

#if YDS_D3D11_USE_FLIP_MODEL
    m_msaaRenderTarget = nullptr;
#endif
}

ysD3D11Context::~ysD3D11Context() {}
