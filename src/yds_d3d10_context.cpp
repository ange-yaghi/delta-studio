#include "../include/yds_d3d10_context.h"

#include "../include/yds_d3d10_render_target.h"
#include "../include/yds_d3d10_device.h"

ysD3D10Context::ysD3D10Context() : ysRenderingContext(ysDevice::DeviceAPI::DirectX10, ysWindowSystemObject::Platform::Windows) {
    m_swapChain = nullptr;
}

ysD3D10Context::~ysD3D10Context() {
    /* void */
}
