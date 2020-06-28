#include "../include/yds_d3d11_context.h"

#include "../include/yds_d3d11_render_target.h"
#include "../include/yds_d3d11_device.h"

ysD3D11Context::ysD3D11Context() : ysRenderingContext(ysDevice::DeviceAPI::DirectX11, ysWindowSystemObject::Platform::Windows) {
    m_swapChain = nullptr;
}

ysD3D11Context::~ysD3D11Context() {
    /* void */
}
