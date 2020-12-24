#include "../include/yds_d3d11_gpu_buffer.h"

ysD3D11GPUBuffer::ysD3D11GPUBuffer() : ysGPUBuffer(DeviceAPI::DirectX11) {
    m_buffer = nullptr;
}

ysD3D11GPUBuffer::~ysD3D11GPUBuffer() {
    /* void */
}

void ysD3D11GPUBuffer::SetDebugName(const std::string &debugName) {
    ysGPUBuffer::SetDebugName(debugName);

    if (m_buffer != nullptr) {
        m_buffer->SetPrivateData(WKPDID_D3DDebugObjectName, debugName.size(), debugName.c_str());
    }
}
