#include "../include/yds_gpu_buffer.h"

ysGPUBuffer::ysGPUBuffer() : ysContextObject("GPU_BUFFER", DeviceAPI::Unknown) {
    m_bufferType = GPU_UNDEFINED_BUFFER;

    m_RAMMirror = nullptr;
    m_size = 0;
    m_currentStride = 0;

    m_mirrorToRAM = false;
}

ysGPUBuffer::ysGPUBuffer(DeviceAPI API) : ysContextObject("GPU_BUFFER", API) {
    m_bufferType = GPU_UNDEFINED_BUFFER;

    m_RAMMirror = nullptr;
    m_size = 0;
    m_currentStride = 0;

    m_mirrorToRAM = false;
}

ysGPUBuffer::~ysGPUBuffer() {
    /* void */
}
