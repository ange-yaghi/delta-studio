#include "../include/yds_d3d11_gpu_buffer.h"

ysD3D11GPUBuffer::ysD3D11GPUBuffer() : ysGPUBuffer(DeviceAPI::DirectX11) {
	m_buffer = NULL;
}

ysD3D11GPUBuffer::~ysD3D11GPUBuffer() {
    /* void */
}
