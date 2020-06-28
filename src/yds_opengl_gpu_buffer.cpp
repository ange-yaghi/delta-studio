#include "../include/yds_opengl_gpu_buffer.h"

#include <OpenGL.h>

ysOpenGLGPUBuffer::ysOpenGLGPUBuffer() : ysGPUBuffer(DeviceAPI::OpenGL4_0) {
    m_bufferHandle = 0;
    m_vertexArrayHandle = 0;
}

ysOpenGLGPUBuffer::~ysOpenGLGPUBuffer() {
    /* void */
}

int ysOpenGLGPUBuffer::GetTarget() {
    int target = 0;
    switch (m_bufferType) {
    case ysGPUBuffer::GPU_CONSTANT_BUFFER:
        target = GL_UNIFORM_BUFFER;
        break;
    case ysGPUBuffer::GPU_DATA_BUFFER:
        target = GL_ARRAY_BUFFER;
        break;
    case ysGPUBuffer::GPU_INDEX_BUFFER:
        target = GL_ELEMENT_ARRAY_BUFFER;
        break;
    };

    return target;
}
