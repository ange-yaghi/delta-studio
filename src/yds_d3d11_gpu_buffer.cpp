#define NOMINMAX
#include "yds_d3d11_gpu_buffer.h"

ysD3D11GPUBuffer::ysD3D11GPUBuffer() : ysGPUBuffer(DeviceAPI::DirectX11) {
    m_buffer = nullptr;
}

ysD3D11GPUBuffer::~ysD3D11GPUBuffer() {
    /* void */
}

void ysD3D11GPUBuffer::SetDebugName(const std::string &debugName) {
    if (m_buffer != nullptr && debugName.size() < std::numeric_limits<UINT>::max()) {
        const UINT len = static_cast<UINT>(debugName.size());
        m_buffer->SetPrivateData(WKPDID_D3DDebugObjectName, len, debugName.c_str());
    }
}
