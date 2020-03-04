#include "../include/yds_d3d10_gpu_buffer.h"

ysD3D10GPUBuffer::ysD3D10GPUBuffer() : ysGPUBuffer(DIRECTX10) {
    m_buffer = NULL;
}

ysD3D10GPUBuffer::~ysD3D10GPUBuffer() {
    /* void */
}
