#include "../include/yds_opengl_device.h"
#include "../include/yds_opengl_context.h"
#include "../include/yds_opengl_render_target.h"
#include "../include/yds_opengl_gpu_buffer.h"
#include "../include/yds_opengl_shader.h"
#include "../include/yds_opengl_input_layout.h"
#include "../include/yds_opengl_shader_program.h"
#include "../include/yds_opengl_texture.h"

#include "../include/yds_opengl_windows_context.h"

#include "OpenGL.h"
#include <SDL.h>
#include <SDL_image.h>

#include "../include/yds_file.h"

ysOpenGLDevice::ysOpenGLDevice() : ysDevice(DeviceAPI::OpenGL4_0) {
    m_deviceCreated = false;
    m_realContext = nullptr;
    m_activeContext = nullptr;
}

ysOpenGLDevice::~ysOpenGLDevice() {
    /* void */
}

ysError ysOpenGLDevice::InitializeDevice() {
    YDS_ERROR_DECLARE("InitializeDevice");

    m_deviceCreated = true;

    InitializeTextureSlots(64);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::DestroyDevice() {
    YDS_ERROR_DECLARE("DestroyDevice");

    m_deviceCreated = false;

    return YDS_ERROR_RETURN(ysError::None);
}

bool ysOpenGLDevice::CheckSupport() {
    // TEMP
    return true;
}

ysError ysOpenGLDevice::CreateRenderingContext(ysRenderingContext **context, ysWindow *window) {
    YDS_ERROR_DECLARE("CreateRenderingContext");

    if (context == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *context = nullptr;

    if (window->GetPlatform() == ysWindowSystemObject::Platform::Windows) {
        ysOpenGLWindowsContext *newContext = m_renderingContexts.NewGeneric<ysOpenGLWindowsContext>();
        YDS_NESTED_ERROR_CALL(newContext->CreateRenderingContext(this, window, 4, 3));

        // TEMP
        glFrontFace(GL_CCW);

        SetFaceCulling(true);
        SetFaceCullingMode(CullMode::Back);

        *context = static_cast<ysRenderingContext *>(newContext);

        return YDS_ERROR_RETURN(ysError::None);
    }
    else {
        return YDS_ERROR_RETURN_MSG(ysError::IncompatiblePlatforms, "Only Windows platforms are currently supported.");
    }
}

ysError ysOpenGLDevice::UpdateRenderingContext(ysRenderingContext *context) {
    YDS_ERROR_DECLARE("UpdateRenderingContext");

    ysRenderTarget *target = context->GetAttachedRenderTarget();
    ysOpenGLVirtualContext *openglContext = static_cast<ysOpenGLVirtualContext *>(context);

    const int width = context->GetWindow()->GetGameWidth();
    const int height = context->GetWindow()->GetGameHeight();
    const int pwidth = context->GetWindow()->GetScreenWidth();
    const int pheight = context->GetWindow()->GetScreenHeight();

    if (target != nullptr) {
        YDS_NESTED_ERROR_CALL(ResizeRenderTarget(target, width, height, pwidth, pheight));
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::DestroyRenderingContext(ysRenderingContext *&context) {
    YDS_ERROR_DECLARE("DestroyRenderingTarget");

    if (context == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    if (context == m_activeContext) {
        SetRenderingContext(nullptr);
    }

    ysOpenGLVirtualContext *openglContext = static_cast<ysOpenGLVirtualContext *>(context);

    if (openglContext->IsRealContext()) {
        ysOpenGLVirtualContext *transferContext = GetTransferContext();
        if (!transferContext) {
            openglContext->DestroyContext();
            UpdateContext();
        }
        else {
            openglContext->TransferContext(transferContext);
            openglContext->DestroyContext();
            UpdateContext();
        }
    }

    YDS_NESTED_ERROR_CALL(ysDevice::DestroyRenderingContext(context));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::SetContextMode(ysRenderingContext *context, ysRenderingContext::ContextMode mode) {
    YDS_ERROR_DECLARE("SetContextMode");

    if (context == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (!CheckCompatibility(context)) return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);

    ysOpenGLVirtualContext *openglContext = static_cast<ysOpenGLVirtualContext *>(context);
    openglContext->SetContextMode(mode);

    return YDS_ERROR_RETURN(ysError::None);
}

void ysOpenGLDevice::SetRenderingContext(ysRenderingContext *context) {
    if (context != nullptr) {
        if (context != m_activeContext) {
            ysOpenGLVirtualContext *openGLContext = static_cast<ysOpenGLVirtualContext *>(context);
            openGLContext->SetContext(m_realContext);
        }
    }
    else {
        if (m_realContext != nullptr) m_realContext->SetContext(nullptr);
    }

    m_activeContext = context;

    // TEMP

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // END TEMP
}

ysError ysOpenGLDevice::CreateOnScreenRenderTarget(ysRenderTarget **newTarget, ysRenderingContext *context, bool depthBuffer) {
    YDS_ERROR_DECLARE("CreateOnScreenRenderTarget");

    if (newTarget == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newTarget = nullptr;

    if (context == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (context->GetAttachedRenderTarget() != nullptr) return YDS_ERROR_RETURN(ysError::ContextAlreadyHasRenderTarget);

    ysOpenGLRenderTarget *newRenderTarget = m_renderTargets.NewGeneric<ysOpenGLRenderTarget>();
    ysOpenGLVirtualContext *openglContext = static_cast<ysOpenGLVirtualContext *>(context);

    openglContext->m_attachedRenderTarget = newRenderTarget;

    newRenderTarget->m_type = ysRenderTarget::Type::OnScreen;
    newRenderTarget->m_posX = 0;
    newRenderTarget->m_posY = 0;
    newRenderTarget->m_width = context->GetWindow()->GetGameWidth();
    newRenderTarget->m_height = context->GetWindow()->GetGameHeight();
    newRenderTarget->m_physicalWidth = context->GetWindow()->GetScreenWidth();
    newRenderTarget->m_physicalHeight = context->GetWindow()->GetScreenHeight();
    newRenderTarget->m_format = ysRenderTarget::Format::R8G8B8A8_UNORM;
    newRenderTarget->m_hasDepthBuffer = depthBuffer;
    newRenderTarget->m_associatedContext = context;

    *newTarget = newRenderTarget;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::CreateOffScreenRenderTarget(ysRenderTarget **newTarget, int width, int height, 
    ysRenderTarget::Format format, bool colorData, bool depthBuffer) 
{
    YDS_ERROR_DECLARE("CreateOffScreenRenderTarget");

    if (newTarget == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ysOpenGLRenderTarget *newRenderTarget = m_renderTargets.NewGeneric<ysOpenGLRenderTarget>();

    ysError result = CreateOpenGLOffScreenRenderTarget(newRenderTarget, width, height, format, colorData, depthBuffer);
    if (result != ysError::None) {
        m_renderTargets.Delete(newRenderTarget->GetIndex());
        return result;
    }

    *newTarget = newRenderTarget;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::CreateSubRenderTarget(ysRenderTarget **newTarget, ysRenderTarget *parent, int x, int y, int width, int height) {
    YDS_ERROR_DECLARE("CreateSubRenderTarget");

    if (newTarget == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (parent->GetType() == ysRenderTarget::Type::Subdivision) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ysOpenGLRenderTarget *newRenderTarget = m_renderTargets.NewGeneric<ysOpenGLRenderTarget>();

    newRenderTarget->m_type = ysRenderTarget::Type::Subdivision;
    newRenderTarget->m_posX = x;
    newRenderTarget->m_posY = y;
    newRenderTarget->m_width = width;
    newRenderTarget->m_height = height;
    newRenderTarget->m_physicalWidth = width;
    newRenderTarget->m_physicalHeight = height;
    newRenderTarget->m_format = ysRenderTarget::Format::R8G8B8A8_UNORM;
    newRenderTarget->m_hasDepthBuffer = parent->HasDepthBuffer();
    newRenderTarget->m_associatedContext = parent->GetAssociatedContext();
    newRenderTarget->m_parent = parent;

    *newTarget = static_cast<ysRenderTarget *>(newRenderTarget);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::ResizeRenderTarget(ysRenderTarget *target, int width, int height, int pwidth, int pheight) {
    YDS_ERROR_DECLARE("ResizeRenderTarget");

    if (target == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    YDS_NESTED_ERROR_CALL(ysDevice::ResizeRenderTarget(target, width, height, pwidth, pheight));

    ysRenderTarget *prevTargets[MaxRenderTargets];
    for (int i = 0; i < MaxRenderTargets; ++i) {
        prevTargets[i] = m_activeRenderTarget[i];
    }

    for (int i = 0; i < MaxRenderTargets; ++i) {
        if (target == prevTargets[i]) {
            SetRenderTarget(nullptr, i);
        }
    }

    if (target->GetType() == ysRenderTarget::Type::OnScreen) {
        // Nothing needs to be done
    }
    else if (target->GetType() == ysRenderTarget::Type::OffScreen) {
        YDS_NESTED_ERROR_CALL(DestroyOpenGLRenderTarget(target));
        YDS_NESTED_ERROR_CALL(CreateOpenGLOffScreenRenderTarget(
            target, width, height, target->GetFormat(), target->HasColorData(), target->HasDepthBuffer()));
    }
    else if (target->GetType() == ysRenderTarget::Type::Subdivision) {
        // Nothing needs to be done
    }

    for (int i = 0; i < MaxRenderTargets; ++i) {
        if (target == prevTargets[i]) {
            SetRenderTarget(target, i);
        }
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::SetRenderTarget(ysRenderTarget *target, int slot) {
    YDS_ERROR_DECLARE("SetRenderTarget");

    if (target != nullptr) {
        ysOpenGLRenderTarget *openglTarget = static_cast<ysOpenGLRenderTarget *>(target);
        ysOpenGLRenderTarget *realTarget = (target->GetType() == ysRenderTarget::Type::Subdivision) ?
            static_cast<ysOpenGLRenderTarget *>(target->GetParent()) : openglTarget;

        for (int i = 0; i < MaxRenderTargets; ++i) {
            if (realTarget != m_activeRenderTarget[i]) {
                if (target->GetAssociatedContext() != nullptr) {
                    SetRenderingContext(target->GetAssociatedContext());
                }
            }
        }

        if (realTarget->GetType() == ysRenderTarget::Type::OnScreen) {
            m_realContext->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        }
        else if (realTarget->GetType() == ysRenderTarget::Type::OffScreen) {
            if (slot == 0) {
                m_realContext->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, realTarget->GetFramebuffer());

                if (realTarget->HasColorData()) {
                    m_realContext->glFramebufferTexture2D(
                        GL_FRAMEBUFFER,
                        GL_COLOR_ATTACHMENT0,
                        GL_TEXTURE_2D,
                        realTarget->m_textureHandle,
                        0);
                }
            }
            else {
                if (realTarget->HasColorData()) {
                    m_realContext->glFramebufferTexture2D(
                        GL_FRAMEBUFFER,
                        GL_COLOR_ATTACHMENT0 + slot,
                        GL_TEXTURE_2D,
                        realTarget->m_textureHandle,
                        0);
                }
            }
        }

        if (slot == 0) {
            if (target->HasDepthBuffer() && target->IsDepthTestEnabled()) glEnable(GL_DEPTH_TEST);
            else glDisable(GL_DEPTH_TEST);

            const int refw = realTarget->GetPhysicalWidth();
            const int refh = realTarget->GetPhysicalHeight();

            const int pwidth = target->GetPhysicalWidth();
            const int pheight = target->GetPhysicalHeight();

            glViewport(target->GetPosX(), refh - target->GetPosY() - pheight, pwidth, pheight);
        }
    }

    YDS_NESTED_ERROR_CALL(ysDevice::SetRenderTarget(target, slot));

    GLenum buffers[MaxRenderTargets];
    int bufferCount = 0;
    for (int i = 0; i < MaxRenderTargets; ++i) {
        if (m_activeRenderTarget[i] != nullptr && m_activeRenderTarget[i]->HasColorData()) {
            buffers[bufferCount] = GL_COLOR_ATTACHMENT0 + bufferCount;
            ++bufferCount;
        }
    }

    if (bufferCount > 0) {
        m_realContext->glDrawBuffers(bufferCount, buffers);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::SetDepthTestEnabled(ysRenderTarget *target, bool enable) {
    YDS_ERROR_DECLARE("SetDepthTestEnabled");

    const bool previousState = target->IsDepthTestEnabled();
    YDS_NESTED_ERROR_CALL(ysDevice::SetDepthTestEnabled(target, enable));

    if (target == GetActiveRenderTarget() && previousState != enable) {
        SetRenderTarget(target);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::ReadRenderTarget(ysRenderTarget *src, uint8_t *target) {
    return ysError();
}

ysError ysOpenGLDevice::DestroyRenderTarget(ysRenderTarget *&target) {
    YDS_ERROR_DECLARE("DestroyRenderTarget");

    if (target == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    for (int i = 0; i < MaxRenderTargets; ++i) {
        if (target == m_activeRenderTarget[i]) {
            YDS_NESTED_ERROR_CALL(SetRenderTarget(nullptr, i));
        }
    }

    ysOpenGLRenderTarget *openglTarget = static_cast<ysOpenGLRenderTarget *>(target);
    DestroyOpenGLRenderTarget(openglTarget);

    YDS_NESTED_ERROR_CALL(ysDevice::DestroyRenderTarget(target));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::ClearBuffers(const float *clearColor) {
    YDS_ERROR_DECLARE("ClearBuffers");

    if (!m_deviceCreated) return YDS_ERROR_RETURN(ysError::NoDevice);

    if (m_activeRenderTarget != nullptr) {
        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
        glClearDepth(1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        return YDS_ERROR_RETURN(ysError::None);
    }

    return YDS_ERROR_RETURN(ysError::NoRenderTarget);
}

ysError ysOpenGLDevice::Present() {
    YDS_ERROR_DECLARE("Present");

    if (m_activeContext == nullptr) return YDS_ERROR_RETURN(ysError::NoContext);
    if (m_activeRenderTarget[0]->GetType() == ysRenderTarget::Type::Subdivision) return YDS_ERROR_RETURN(ysError::InvalidOperation);

    ysOpenGLVirtualContext *currentContext = static_cast<ysOpenGLVirtualContext *>(m_activeContext);
    currentContext->Present();

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::SetFaceCulling(bool faceCulling) {
    YDS_ERROR_DECLARE("SetFaceCulling");

    if (faceCulling) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::SetFaceCullingMode(CullMode cullMode) {
    YDS_ERROR_DECLARE("SetFaceCullingMode");

    switch (cullMode) {
    case CullMode::Front:
        glCullFace(GL_FRONT);
        break;
    case CullMode::Back:
        glCullFace(GL_BACK);
        break;
    case CullMode::None:
        glCullFace(GL_NONE);
        break;
    default:
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

// Vertex Buffers
ysError ysOpenGLDevice::CreateVertexBuffer(ysGPUBuffer **newBuffer, int size, char *data, bool mirrorToRam) {
    YDS_ERROR_DECLARE("CreateVertexBuffer");

    if (newBuffer == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newBuffer = nullptr;

    ysOpenGLGPUBuffer *newOpenGLBuffer = m_gpuBuffers.NewGeneric<ysOpenGLGPUBuffer>();

    m_realContext->glGenVertexArrays(1, &newOpenGLBuffer->m_vertexArrayHandle);
    m_realContext->glBindVertexArray(newOpenGLBuffer->m_vertexArrayHandle);

    m_realContext->glGenBuffers(1, &newOpenGLBuffer->m_bufferHandle);
    m_realContext->glBindBuffer(GL_ARRAY_BUFFER, newOpenGLBuffer->m_bufferHandle);
    m_realContext->glBufferData(GL_ARRAY_BUFFER, size, (void *)data, GL_STATIC_DRAW);

    newOpenGLBuffer->m_size = size;
    newOpenGLBuffer->m_mirrorToRAM = mirrorToRam;
    newOpenGLBuffer->m_bufferType = ysGPUBuffer::GPU_DATA_BUFFER;

    if (mirrorToRam) {
        newOpenGLBuffer->m_RAMMirror = new char[size];
        if (data != nullptr) memcpy(newOpenGLBuffer->m_RAMMirror, data, size);
    }

    *newBuffer = static_cast<ysGPUBuffer *>(newOpenGLBuffer);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::CreateIndexBuffer(ysGPUBuffer **newBuffer, int size, char *data, bool mirrorToRam) {
    YDS_ERROR_DECLARE("CreateIndexBuffer");

    if (newBuffer == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newBuffer = nullptr;

    ysOpenGLGPUBuffer *newOpenGLBuffer = m_gpuBuffers.NewGeneric<ysOpenGLGPUBuffer>();

    newOpenGLBuffer->m_size = size;
    newOpenGLBuffer->m_mirrorToRAM = mirrorToRam;
    newOpenGLBuffer->m_bufferType = ysGPUBuffer::GPU_INDEX_BUFFER;

    m_realContext->glGenBuffers(1, &newOpenGLBuffer->m_bufferHandle);
    m_realContext->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newOpenGLBuffer->m_bufferHandle);
    m_realContext->glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, (void *)data, GL_STATIC_DRAW);

    if (mirrorToRam) {
        newOpenGLBuffer->m_RAMMirror = new char[size];
        if (data != nullptr) memcpy(newOpenGLBuffer->m_RAMMirror, data, size);

    }

    *newBuffer = static_cast<ysGPUBuffer *>(newOpenGLBuffer);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::CreateConstantBuffer(ysGPUBuffer **newBuffer, int size, char *data, bool mirrorToRam) {
    YDS_ERROR_DECLARE("CreateConstantBuffer");

    if (newBuffer == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newBuffer = nullptr;

    ysOpenGLGPUBuffer *newOpenGLBuffer = m_gpuBuffers.NewGeneric<ysOpenGLGPUBuffer>();

    newOpenGLBuffer->m_size = size;
    newOpenGLBuffer->m_mirrorToRAM = mirrorToRam;
    newOpenGLBuffer->m_bufferType = ysGPUBuffer::GPU_CONSTANT_BUFFER;

    m_realContext->glGenBuffers(1, &newOpenGLBuffer->m_bufferHandle);
    m_realContext->glBindBuffer(GL_UNIFORM_BUFFER, newOpenGLBuffer->m_bufferHandle);
    m_realContext->glBufferData(GL_UNIFORM_BUFFER, size, (void *)data, GL_DYNAMIC_DRAW);

    if (mirrorToRam) {
        newOpenGLBuffer->m_RAMMirror = new char[size];
        if (data != nullptr) memcpy(newOpenGLBuffer->m_RAMMirror, data, size);
    }

    *newBuffer = static_cast<ysGPUBuffer *>(newOpenGLBuffer);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::DestroyGPUBuffer(ysGPUBuffer *&buffer) {
    YDS_ERROR_DECLARE("DestroyGPUBuffer");

    if (!CheckCompatibility(buffer))    return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    if (buffer == NULL)                    return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ysOpenGLGPUBuffer *openglBuffer = static_cast<ysOpenGLGPUBuffer *>(buffer);
    m_realContext->glDeleteBuffers(1, &openglBuffer->m_bufferHandle);

    if (openglBuffer->m_bufferType == ysGPUBuffer::GPU_DATA_BUFFER) {
        m_realContext->glDeleteVertexArrays(1, &openglBuffer->m_vertexArrayHandle);
    }

    YDS_NESTED_ERROR_CALL(ysDevice::DestroyGPUBuffer(buffer));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::UseVertexBuffer(ysGPUBuffer *buffer, int stride, int offset) {
    YDS_ERROR_DECLARE("UseVertexBuffer");

    if (!CheckCompatibility(buffer)) return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);

    if (buffer == nullptr) {
        // Release all bindings
        m_realContext->glBindVertexArray(0);
        m_realContext->glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else {
        ysOpenGLGPUBuffer *openglBuffer = static_cast<ysOpenGLGPUBuffer *>(buffer);
        ysOpenGLGPUBuffer *openglCurrentIndexBuffer = static_cast<ysOpenGLGPUBuffer *>(m_activeIndexBuffer);

        if (openglBuffer->m_bufferType == ysGPUBuffer::GPU_DATA_BUFFER && m_activeVertexBuffer != buffer) {
            m_realContext->glBindVertexArray(openglBuffer->m_vertexArrayHandle);
            m_realContext->glBindBuffer(GL_ARRAY_BUFFER, openglBuffer->m_bufferHandle);

            // The input layout must be redone
            ResubmitInputLayout();
            if (openglCurrentIndexBuffer != nullptr) m_realContext->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openglCurrentIndexBuffer->m_bufferHandle);
        }
    }

    YDS_NESTED_ERROR_CALL(ysDevice::UseVertexBuffer(buffer, stride, offset));
    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::UseIndexBuffer(ysGPUBuffer *buffer, int offset) {
    YDS_ERROR_DECLARE("UseIndexBuffer");

    if (!CheckCompatibility(buffer)) return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);

    if (buffer == nullptr) {
        // Release all bindings
        m_realContext->glBindVertexArray(0);
        m_realContext->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    else {
        ysOpenGLGPUBuffer *openglBuffer = static_cast<ysOpenGLGPUBuffer *>(buffer);
        ysOpenGLGPUBuffer *openglCurrentIndexBuffer = static_cast<ysOpenGLGPUBuffer *>(m_activeIndexBuffer);

        if (openglBuffer->m_bufferType == ysGPUBuffer::GPU_INDEX_BUFFER && m_activeIndexBuffer != buffer) {
            m_realContext->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openglBuffer->m_bufferHandle);
        }

        YDS_NESTED_ERROR_CALL(ysDevice::UseIndexBuffer(buffer, offset));
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::UseConstantBuffer(ysGPUBuffer *buffer, int slot) {
    YDS_ERROR_DECLARE("UseConstantBuffer");

    if (buffer == nullptr) {
        m_realContext->glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    else {
        ysOpenGLGPUBuffer *openglBuffer = static_cast<ysOpenGLGPUBuffer *>(buffer);
        m_realContext->glBindBufferRange(GL_UNIFORM_BUFFER, slot, openglBuffer->m_bufferHandle, 0, buffer->GetSize());
    }

    YDS_NESTED_ERROR_CALL(ysDevice::UseConstantBuffer(buffer, slot));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::EditBufferDataRange(ysGPUBuffer *buffer, char *data, int size, int offset) {
    YDS_ERROR_DECLARE("EditBufferData");

    if (!CheckCompatibility(buffer))            return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    if (buffer == nullptr || data == nullptr)    return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ysGPUBuffer *previous = GetActiveBuffer(buffer->GetType());

    ysOpenGLGPUBuffer *openglBuffer = static_cast<ysOpenGLGPUBuffer *>(buffer);
    int target = openglBuffer->GetTarget();

    m_realContext->glBindBuffer(target, openglBuffer->m_bufferHandle);
    m_realContext->glBufferSubData(target, offset, size, data);

    // Restore Previous State
    if (previous != buffer) {
        openglBuffer = static_cast<ysOpenGLGPUBuffer *>(previous);
        m_realContext->glBindBuffer(target, (openglBuffer) ? openglBuffer->m_bufferHandle : 0);
    }

    YDS_NESTED_ERROR_CALL(ysDevice::EditBufferDataRange(buffer, data, size, offset));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::EditBufferData(ysGPUBuffer *buffer, char *data) {
    YDS_ERROR_DECLARE("EditBufferData");

    if (!CheckCompatibility(buffer))            return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    if (buffer == nullptr || data == nullptr)    return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ysGPUBuffer *previous = GetActiveBuffer(buffer->GetType());

    ysOpenGLGPUBuffer *openglBuffer = static_cast<ysOpenGLGPUBuffer *>(buffer);
    int target = openglBuffer->GetTarget();
    
    m_realContext->glBindBuffer(target, openglBuffer->m_bufferHandle);
    m_realContext->glBufferSubData(target, 0, openglBuffer->GetSize(), data);

    // Restore Previous State
    if (previous != buffer) {
        openglBuffer = static_cast<ysOpenGLGPUBuffer *>(previous);
        m_realContext->glBindBuffer(target, (openglBuffer) ? openglBuffer->m_bufferHandle : 0);
    }

    YDS_NESTED_ERROR_CALL(ysDevice::EditBufferData(buffer, data));

    return YDS_ERROR_RETURN(ysError::None);
}

// Shaders
ysError ysOpenGLDevice::CreateVertexShader(ysShader **newShader, const char *shaderFilename, const char *shaderName) {
    YDS_ERROR_DECLARE("CreateVertexShader");

    if (newShader == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newShader = nullptr;

    if (shaderFilename == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (shaderName == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ysFile file;
    int fileLength;
    char *fileBuffer;
    unsigned int handle;
    GLenum result;

    YDS_NESTED_ERROR_CALL(file.OpenFile(shaderFilename, ysFile::FILE_BINARY | ysFile::FILE_READ));
    fileLength = file.GetFileLength();
    fileBuffer = new char[(unsigned long long)fileLength + 1];
    file.ReadFileToBuffer(fileBuffer);
    fileBuffer[fileLength] = 0;

    handle = m_realContext->glCreateShader(GL_VERTEX_SHADER);
    if (handle == 0) return YDS_ERROR_RETURN(ysError::CouldNotCreateShader);

    m_realContext->glShaderSource(handle, 1, &fileBuffer, NULL); result = glGetError();
    m_realContext->glCompileShader(handle);

    delete[] fileBuffer;

    // Check Compilation
    int shaderCompiled;
    m_realContext->glGetShaderiv(handle, GL_COMPILE_STATUS, &shaderCompiled);

    if (!shaderCompiled) {
        char errorBuffer[2048 + 1];
        m_realContext->glGetShaderInfoLog(handle, 2048, NULL, errorBuffer);

        return YDS_ERROR_RETURN_MSG(ysError::VertexShaderCompilationError, errorBuffer);
    }

    ysOpenGLShader *newOpenGLShader = m_shaders.NewGeneric<ysOpenGLShader>();
    strcpy_s(newOpenGLShader->m_shaderName, 64, shaderName);
    strcpy_s(newOpenGLShader->m_filename, 256, shaderFilename);
    newOpenGLShader->m_shaderType = ysShader::ShaderType::Vertex;
    newOpenGLShader->m_handle = handle;

    *newShader = static_cast<ysShader *>(newOpenGLShader);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::CreatePixelShader(ysShader **newShader, const char *shaderFilename, const char *shaderName) {
    YDS_ERROR_DECLARE("CreatePixelShader");

    if (newShader == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newShader = nullptr;

    if (shaderFilename == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (shaderName == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    GLenum result = glGetError();

    ysFile file;
    int fileLength;
    char *fileBuffer;

    unsigned int shaderHandle;

    shaderHandle = m_realContext->glCreateShader(GL_FRAGMENT_SHADER);
    if (shaderHandle == 0) return YDS_ERROR_RETURN(ysError::CouldNotCreateShader);

    YDS_NESTED_ERROR_CALL(file.OpenFile(shaderFilename, ysFile::FILE_BINARY | ysFile::FILE_READ));
    fileLength = file.GetFileLength();
    fileBuffer = new char[(unsigned long long)fileLength + 1];
    file.ReadFileToBuffer(fileBuffer);
    fileBuffer[fileLength] = 0;

    m_realContext->glShaderSource(shaderHandle, 1, &fileBuffer, NULL);
    m_realContext->glCompileShader(shaderHandle);

    delete[] fileBuffer;

    // Check Compilation
    int shaderCompiled;

    m_realContext->glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &shaderCompiled);

    if (!shaderCompiled) {
        char errorBuffer[2048 + 1];
        m_realContext->glGetShaderInfoLog(shaderHandle, 2048, NULL, errorBuffer);
        return YDS_ERROR_RETURN_MSG(ysError::FragmentShaderCompilationError, errorBuffer);
    }

    ysOpenGLShader *newOpenGLShader = m_shaders.NewGeneric<ysOpenGLShader>();
    strcpy_s(newOpenGLShader->m_shaderName, 64, shaderName);
    strcpy_s(newOpenGLShader->m_filename, 256, shaderFilename);
    newOpenGLShader->m_shaderType = ysShader::ShaderType::Pixel;
    newOpenGLShader->m_handle = shaderHandle;

    *newShader = static_cast<ysShader *>(newOpenGLShader);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::DestroyShader(ysShader *&shader) {
    YDS_ERROR_DECLARE("DestroyShader");

    if (shader == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ysOpenGLShader *openglShader = static_cast<ysOpenGLShader *>(shader);
    m_realContext->glDeleteShader(openglShader->m_handle);

    YDS_NESTED_ERROR_CALL(ysDevice::DestroyShader(shader));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::DestroyShaderProgram(ysShaderProgram *&program, bool destroyShaders) {
    YDS_ERROR_DECLARE("DestroyShaderProgram");

    if (program == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ysOpenGLShaderProgram *openglProgram = static_cast<ysOpenGLShaderProgram *>(program);

    for (int i = 0; i < (int)ysShader::ShaderType::NumShaderTypes; i++) {
        ysOpenGLShader *openglShader = static_cast<ysOpenGLShader *>(openglProgram->m_shaderSlots[i]);
        m_realContext->glDetachShader(openglProgram->m_handle, openglShader->m_handle);
    }

    m_realContext->glDeleteProgram(openglProgram->m_handle);

    YDS_NESTED_ERROR_CALL(ysDevice::DestroyShaderProgram(program, destroyShaders));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::AttachShader(ysShaderProgram *program, ysShader *shader) {
    YDS_ERROR_DECLARE("AttachShader");

    YDS_NESTED_ERROR_CALL(ysDevice::AttachShader(program, shader));

    ysOpenGLShaderProgram *openglProgram = static_cast<ysOpenGLShaderProgram *>(program);
    ysOpenGLShader *openglShader = static_cast<ysOpenGLShader *>(shader);
    m_realContext->glAttachShader(openglProgram->m_handle, openglShader->m_handle);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::LinkProgram(ysShaderProgram *program) {
    YDS_ERROR_DECLARE("LinkProgram");

    YDS_NESTED_ERROR_CALL(ysDevice::LinkProgram(program));

    ysOpenGLShaderProgram *openglProgram = static_cast<ysOpenGLShaderProgram *>(program);
    m_realContext->glLinkProgram(openglProgram->m_handle);

    GLint status;
    m_realContext->glGetProgramiv(openglProgram->m_handle, GL_LINK_STATUS, &status);

    if (status == GL_FALSE) {
        return YDS_ERROR_RETURN(ysError::ProgramLinkError);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::UseShaderProgram(ysShaderProgram *program) {
    YDS_ERROR_DECLARE("UseShaderProgram");

    ysShaderProgram *previousShaderProgram = m_activeShaderProgram;
    YDS_NESTED_ERROR_CALL(ysDevice::UseShaderProgram(program));

    if (previousShaderProgram == program) return YDS_ERROR_RETURN(ysError::None);

    if (program != nullptr) {
        ysOpenGLShaderProgram *openglProgram = static_cast<ysOpenGLShaderProgram *>(program);
        m_realContext->glUseProgram(openglProgram->m_handle);
    }
    else {
        m_realContext->glUseProgram(NULL);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::CreateShaderProgram(ysShaderProgram **program) {
    YDS_ERROR_DECLARE("CreateShaderProgram");

    if (program == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ysOpenGLShaderProgram *newProgram = m_shaderPrograms.NewGeneric<ysOpenGLShaderProgram>();
    newProgram->m_handle = m_realContext->glCreateProgram();

    *program = static_cast<ysShaderProgram *>(newProgram);

    return YDS_ERROR_RETURN(ysError::None);
}

// Input Layouts
ysError ysOpenGLDevice::CreateInputLayout(ysInputLayout **newInputLayout, ysShader *shader, const ysRenderGeometryFormat *format) {
    YDS_ERROR_DECLARE("CreateInputLayout");

    ysOpenGLInputLayout *newLayout = m_inputLayouts.NewGeneric<ysOpenGLInputLayout>();
    newLayout->m_size = 0;
    int nChannels = format->GetChannelCount();

    for (int i = 0; i < nChannels; i++) {
        const ysRenderGeometryChannel *channel = format->GetChannel(i);

        ysOpenGLLayoutChannel *newChannel = newLayout->m_channels.New();
        newChannel->m_length = channel->GetLength();
        newChannel->m_type = GetFormatGLType(channel->GetFormat());
        newChannel->m_size = channel->GetSize();
        newChannel->m_offset = channel->GetOffset();

        newLayout->m_size += newChannel->m_size;
    }

    *newInputLayout = static_cast<ysInputLayout *>(newLayout);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::UseInputLayout(ysInputLayout *layout) {
    YDS_ERROR_DECLARE("UseInputLayout");

    if (layout == m_activeInputLayout) return YDS_ERROR_RETURN(ysError::None);
    if (layout == nullptr) return YDS_ERROR_RETURN(ysError::None);

    YDS_NESTED_ERROR_CALL(ysDevice::UseInputLayout(layout));

    if (m_activeVertexBuffer == nullptr) return YDS_ERROR_RETURN(ysError::None);

    ysOpenGLInputLayout *openglLayout = static_cast<ysOpenGLInputLayout *>(layout);
    const int nChannels = openglLayout->m_channels.GetNumObjects();

    for (int i = 0; i < nChannels; i++) {
        ysOpenGLLayoutChannel *channel = openglLayout->m_channels.Get(i);
        m_realContext->glVertexAttribPointer(i, channel->m_length, channel->m_type, GL_FALSE, openglLayout->m_size, (void *)channel->m_offset);
        m_realContext->glEnableVertexAttribArray(i);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::DestroyInputLayout(ysInputLayout *&layout) {
    YDS_ERROR_DECLARE("DestroyInputLayout");

    YDS_NESTED_ERROR_CALL(ysDevice::DestroyInputLayout(layout));

    return YDS_ERROR_RETURN(ysError::None);
}

unsigned int ysOpenGLDevice::GetPixel(SDL_Surface *surface, int x, int y) {
    const int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
    case 1:
        return *p;
        break;
    case 2:
        return *(Uint16 *)p;
        break;
    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;
    case 4:
        return *(Uint32 *)p;
        break; 
    default:
        return 0; // Avoid warnings
    }
}

ysError ysOpenGLDevice::CreateTexture(ysTexture **texture, const char *fname) {
    YDS_ERROR_DECLARE("CreateTexture");

    if (texture == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *texture = nullptr;

    if (fname == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    bool useAlpha = true;

    // Use SDL to load the image
    SDL_Surface *pTexSurface = IMG_Load(fname); 

    if (pTexSurface == nullptr) {
        const char *err = IMG_GetError();
        return YDS_ERROR_RETURN_MSG(ysError::CouldNotOpenFile, fname);
    }

    ysOpenGLTexture *newTexture = m_textures.NewGeneric<ysOpenGLTexture>();
    strcpy_s(newTexture->m_filename, 257, fname);

    glGenTextures(1, &newTexture->m_handle);
    glBindTexture(GL_TEXTURE_2D, newTexture->m_handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    newTexture->m_width = pTexSurface->w;
    newTexture->m_height = pTexSurface->h;

    int nBytes = newTexture->m_width * newTexture->m_height * (useAlpha ? 4 : 3);
    int size = 0;

    Uint8 *imageData = new Uint8[nBytes];

    int pos = 0;
    int y;
    int x;

    int index = 0;

    for (y = (newTexture->m_height - 1); y > -1; y--) {
        for (x = 0; x < newTexture->m_width; x++) {
            Uint8 r = 0, g = 0, b = 0, a = 0;

            Uint32 colour = GetPixel(pTexSurface, x, y);

            if (!useAlpha) {
                SDL_GetRGB(colour, pTexSurface->format, &r, &g, &b);
            }
            else {
                SDL_GetRGBA(colour, pTexSurface->format, &r, &g, &b, &a);
            }

            imageData[index] = r; ++index;
            imageData[index] = g; ++index;
            imageData[index] = b; ++index;

            if (useAlpha) {
                imageData[index] = a; index++;
            }
        }
    }

    const int texType = (useAlpha) ? GL_RGBA : GL_RGB;

    if (!useAlpha) glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    else glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glTexImage2D(GL_TEXTURE_2D, 0, texType, newTexture->m_width, newTexture->m_height, 0, texType, GL_UNSIGNED_BYTE, imageData);
    m_realContext->glGenerateMipmap(GL_TEXTURE_2D); // TEMP

    SDL_FreeSurface(pTexSurface);
    delete[] imageData;

    *texture = static_cast<ysTexture *>(newTexture);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::CreateTexture(ysTexture **texture, int width, int height, const unsigned char *buffer) {
    YDS_ERROR_DECLARE("CreateTexture");

    if (texture == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *texture = nullptr;

    ysOpenGLTexture *newTexture = m_textures.NewGeneric<ysOpenGLTexture>();
    strcpy_s(newTexture->m_filename, 257, "");

    glGenTextures(1, &newTexture->m_handle);
    glBindTexture(GL_TEXTURE_2D, newTexture->m_handle);

    newTexture->m_width = width;
    newTexture->m_height = height;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newTexture->m_width, newTexture->m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    *texture = static_cast<ysTexture *>(newTexture);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::UpdateTexture(ysTexture *texture, const unsigned char *buffer) {
    YDS_ERROR_DECLARE("UpdateTexture");
    return YDS_ERROR_RETURN(ysError::NotImplemented);
}

ysError ysOpenGLDevice::CreateAlphaTexture(ysTexture **texture, int width, int height, const unsigned char *buffer) {
    YDS_ERROR_DECLARE("CreateTexture");

    if (texture == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *texture = nullptr;

    ysOpenGLTexture *newTexture = m_textures.NewGeneric<ysOpenGLTexture>();
    strcpy_s(newTexture->m_filename, 257, "");

    glGenTextures(1, &newTexture->m_handle);
    glBindTexture(GL_TEXTURE_2D, newTexture->m_handle);

    newTexture->m_width = width;
    newTexture->m_height = height;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, newTexture->m_width, newTexture->m_height, 0, GL_RED, GL_UNSIGNED_BYTE, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    *texture = static_cast<ysTexture *>(newTexture);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::UseTexture(ysTexture *texture, int slot) {
    YDS_ERROR_DECLARE("UseTexture");
    YDS_NESTED_ERROR_CALL(ysDevice::UseTexture(texture, slot));

    if (texture != nullptr) {
        ysOpenGLTexture *openglTexture = static_cast<ysOpenGLTexture *>(texture);

        m_realContext->glActiveTexture(slot + GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, openglTexture->m_handle);
    }
    else {
        m_realContext->glActiveTexture(slot + GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, NULL);

    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::UseRenderTargetAsTexture(ysRenderTarget *renderTarget, int slot) {
    YDS_ERROR_DECLARE("UseRenderTargetAsTexture");
    YDS_NESTED_ERROR_CALL(ysDevice::UseRenderTargetAsTexture(renderTarget, slot));

    if (renderTarget != nullptr) {
        ysOpenGLRenderTarget *openglRenderTarget = static_cast<ysOpenGLRenderTarget *>(renderTarget);

        m_realContext->glActiveTexture(slot + GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, openglRenderTarget->GetTexture());
    }
    else {
        m_realContext->glActiveTexture(slot + GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::DestroyTexture(ysTexture *&texture) {
    YDS_ERROR_DECLARE("DestroyTexture");

    if (texture == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ysOpenGLTexture *openglTexture = static_cast<ysOpenGLTexture *>(texture);

    for (int i = 0; i < m_maxTextureSlots; i++) {
        if (m_activeTextures[i].Texture == texture) {
            UseTexture(nullptr, i);
        }
    }

    glDeleteTextures(1, &openglTexture->m_handle);

    YDS_NESTED_ERROR_CALL(ysDevice::DestroyTexture(texture));

    return YDS_ERROR_RETURN(ysError::None);
}

void ysOpenGLDevice::ResubmitInputLayout() {
    if (m_activeInputLayout == nullptr) return;

    ysOpenGLInputLayout *openglLayout = static_cast<ysOpenGLInputLayout *>(m_activeInputLayout);
    int nChannels = openglLayout->m_channels.GetNumObjects();

    for (int i = 0; i < nChannels; i++) {
        ysOpenGLLayoutChannel *channel = openglLayout->m_channels.Get(i);

        if (channel->m_type == GL_FLOAT) {
            m_realContext->glVertexAttribPointer(i, channel->m_length, channel->m_type, GL_FALSE, openglLayout->m_size, (void *)channel->m_offset);
        }
        else {
            m_realContext->glVertexAttribIPointer(i, channel->m_length, channel->m_type, openglLayout->m_size, (void *)channel->m_offset);
        }

        m_realContext->glEnableVertexAttribArray(i);
    }
}

const ysOpenGLVirtualContext *ysOpenGLDevice::UpdateContext() {
    for (int i = 0; i < m_renderingContexts.GetNumObjects(); i++) {
        ysOpenGLVirtualContext *openglContext = static_cast<ysOpenGLVirtualContext *>(m_renderingContexts.Get(i));
        if (openglContext->IsRealContext()) {
            return m_realContext = openglContext;
        }

    }

    return nullptr;
}

ysOpenGLVirtualContext *ysOpenGLDevice::GetTransferContext() {
    for (int i = 0; i < m_renderingContexts.GetNumObjects(); i++) {
        ysOpenGLVirtualContext *openglContext = static_cast<ysOpenGLVirtualContext *>(m_renderingContexts.Get(i));
        if (!openglContext->IsRealContext()) return openglContext;
    }

    return nullptr;
}

int ysOpenGLDevice::GetFormatGLType(ysRenderGeometryChannel::ChannelFormat format) {
    switch (format) {
    case ysRenderGeometryChannel::ChannelFormat::R32G32B32_FLOAT:
    case ysRenderGeometryChannel::ChannelFormat::R32G32_FLOAT:
    case ysRenderGeometryChannel::ChannelFormat::R32G32B32A32_FLOAT:
        return GL_FLOAT;
    case ysRenderGeometryChannel::ChannelFormat::R32G32B32A32_UINT:
        return GL_UNSIGNED_INT;
    case ysRenderGeometryChannel::ChannelFormat::R32G32B32_UINT:
        return GL_UNSIGNED_INT;
    default:
        // No real option here
        return GL_4_BYTES;
    }
}

int ysOpenGLDevice::GetFramebufferName(int slot) {
    return GL_COLOR_ATTACHMENT0 + slot;
}

// TEMP
void ysOpenGLDevice::Draw(int numFaces, int indexOffset, int vertexOffset) {
    if (m_activeVertexBuffer != nullptr) {
        m_realContext->glDrawElementsBaseVertex(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_SHORT, (void *)(indexOffset * 2), vertexOffset);
    }
}

ysError ysOpenGLDevice::CreateOpenGLOffScreenRenderTarget(ysRenderTarget *target, int width, int height, 
    ysRenderTarget::Format format, bool colorData, bool depthBuffer) 
{
    YDS_ERROR_DECLARE("CreateOpenGLOffScreenRenderTarget");

    // Generate the empty texture
    unsigned int newTexture;
    glGenTextures(1, &newTexture);
    glBindTexture(GL_TEXTURE_2D, newTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (!colorData) {
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    GLint glFormat;
    GLint glType;
    GLint pixelFormat;

    switch (format) {
    case ysRenderTarget::Format::R8G8B8A8_UNORM:
        glFormat = GL_RGBA8;
        glType = GL_UNSIGNED_BYTE;
        break;
    case ysRenderTarget::Format::R32G32B32_FLOAT:
        glFormat = GL_RGBA32F_ARB;
        glType = GL_FLOAT;
        break;
    case ysRenderTarget::Format::R32_DEPTH_COMPONENT:
        glFormat = GL_DEPTH_COMPONENT;
        glType = GL_FLOAT;
        break;
    case ysRenderTarget::Format::R32_FLOAT:
        glFormat = GL_R32F;
        glType = GL_FLAT;
        break;
    default:
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    }

    pixelFormat = colorData
        ? GL_RGBA
        : GL_DEPTH_COMPONENT;

    glTexImage2D(GL_TEXTURE_2D, 0, glFormat, width, height, 0, pixelFormat, glType, nullptr);

    // Create a framebuffer
    unsigned int framebuffer;
    m_realContext->glGenFramebuffers(1, &framebuffer);
    m_realContext->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    if (colorData) m_realContext->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, newTexture, 0);

    // Create a depth buffer
    unsigned int depthBufferHandle = 0;
    if (depthBuffer) {
        m_realContext->glGenRenderbuffers(1, &depthBufferHandle);
        m_realContext->glBindRenderbuffer(GL_RENDERBUFFER, depthBufferHandle);
        m_realContext->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
        m_realContext->glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferHandle);

        if (!colorData) m_realContext->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, newTexture, 0);
    }

    ysOpenGLRenderTarget *newRenderTarget = static_cast<ysOpenGLRenderTarget *>(target);

    newRenderTarget->m_type = ysRenderTarget::Type::OffScreen;
    newRenderTarget->m_posX = 0;
    newRenderTarget->m_posY = 0;
    newRenderTarget->m_width = width;
    newRenderTarget->m_height = height;
    newRenderTarget->m_physicalWidth = width;
    newRenderTarget->m_physicalHeight = height;
    newRenderTarget->m_format = format;
    newRenderTarget->m_hasDepthBuffer = depthBuffer;
    newRenderTarget->m_hasColorData = colorData;
    newRenderTarget->m_associatedContext = nullptr;

    newRenderTarget->m_textureHandle = newTexture;
    newRenderTarget->m_depthBufferHandle = depthBufferHandle;
    newRenderTarget->m_framebufferHandle = framebuffer;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLDevice::DestroyOpenGLRenderTarget(ysRenderTarget *target) {
    YDS_ERROR_DECLARE("DestroyOpenGLRenderTarget");

    ysOpenGLRenderTarget *openglTarget = static_cast<ysOpenGLRenderTarget *>(target);

    if (target->GetType() == ysRenderTarget::Type::OffScreen) {
        if (target->HasDepthBuffer()) {
            unsigned int depthBuffer[] = { openglTarget->GetDepthBuffer() };
            m_realContext->glDeleteRenderbuffers(1, depthBuffer);
        }

        unsigned int frameBuffer[] = { openglTarget->GetFramebuffer() };
        m_realContext->glDeleteFramebuffers(1, frameBuffer);

        unsigned int texture = openglTarget->GetTexture();
        glDeleteTextures(1, &texture);
    }

    return YDS_ERROR_RETURN(ysError::None);
}
