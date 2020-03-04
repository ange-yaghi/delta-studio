#include "../include/yds_gpu_buffer.h"

ysGPUBuffer::ysGPUBuffer() : ysContextObject("GPU_BUFFER", API_UNKNOWN) {
	m_bufferType = GPU_UNDEFINED_BUFFER;

	m_RAMMirror = NULL;
	m_size = 0;

	m_mirrorToRAM = false;
}

ysGPUBuffer::ysGPUBuffer(DEVICE_API API) : ysContextObject("GPU_BUFFER", API) {
	m_bufferType = GPU_UNDEFINED_BUFFER;

	m_RAMMirror = NULL;
	m_size = 0;

	m_mirrorToRAM = false;
}

ysGPUBuffer::~ysGPUBuffer() {
    /* void */
}
