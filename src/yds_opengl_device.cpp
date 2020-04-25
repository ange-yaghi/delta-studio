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

ysOpenGLDevice::ysOpenGLDevice() : ysDevice(OPENGL4_0) {
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

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::DestroyDevice() {
    YDS_ERROR_DECLARE("DestroyDevice");

    m_deviceCreated = false;

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

bool ysOpenGLDevice::CheckSupport() {
    // TEMP
    return true;
}

ysError ysOpenGLDevice::CreateRenderingContext(ysRenderingContext **context, ysWindow *window) {
    YDS_ERROR_DECLARE("CreateRenderingContext");

    if (context == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);
    *context = NULL;

    if (window->GetPlatform() == ysWindowSystemObject::Platform::WINDOWS) {
        ysOpenGLWindowsContext *newContext;
        newContext = m_renderingContexts.NewGeneric<ysOpenGLWindowsContext>();
        YDS_NESTED_ERROR_CALL(newContext->CreateRenderingContext(this, window, 4, 3));

        // TEMP
        glFrontFace(GL_CW);

        *context = static_cast<ysRenderingContext *>(newContext);

        return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
    }
    else {
        return YDS_ERROR_RETURN_MSG(ysError::YDS_INCOMPATIBLE_PLATFORMS, "Only Windows platforms are currently supported.");
    }
}

ysError ysOpenGLDevice::UpdateRenderingContext(ysRenderingContext *context) {
    YDS_ERROR_DECLARE("UpdateRenderingContext");

    ysRenderTarget *target = context->GetAttachedRenderTarget();
    ysOpenGLVirtualContext *openglContext = static_cast<ysOpenGLVirtualContext *>(context);

    if (target != NULL) {
        YDS_NESTED_ERROR_CALL(ResizeRenderTarget(target, context->GetWindow()->GetScreenWidth(), context->GetWindow()->GetScreenHeight()));
    }

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::DestroyRenderingContext(ysRenderingContext *&context) {
    YDS_ERROR_DECLARE("DestroyRenderingTarget");

    if (context == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);

    if (context == m_activeContext) {
        SetRenderingContext(NULL);
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

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::SetContextMode(ysRenderingContext *context, ysRenderingContext::ContextMode mode) {
    YDS_ERROR_DECLARE("SetContextMode");

    if (context == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);
    if (!CheckCompatibility(context)) return YDS_ERROR_RETURN(ysError::YDS_INCOMPATIBLE_PLATFORMS);

    ysOpenGLVirtualContext *openglContext = static_cast<ysOpenGLVirtualContext *>(context);
    openglContext->SetContextMode(mode);

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

void ysOpenGLDevice::SetRenderingContext(ysRenderingContext *context) {
    if (context) {
        if (context != m_activeContext) {
            ysOpenGLVirtualContext *openGLContext = static_cast<ysOpenGLVirtualContext *>(context);
            //wglMakeCurrent(openGLContext->m_deviceHandle, m_contextHandle);// openGLContext->m_contextHandle);
            openGLContext->SetContext(m_realContext);
        }
    }
    else {
        if (m_realContext) m_realContext->SetContext(NULL);
        //wglMakeCurrent(NULL, NULL);
    }

    m_activeContext = context;

    // TEMP

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // END TEMP
}

ysError ysOpenGLDevice::CreateOnScreenRenderTarget(ysRenderTarget **newTarget, ysRenderingContext *context, bool depthBuffer) {
    YDS_ERROR_DECLARE("CreateOnScreenRenderTarget");

    if (newTarget == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);
    *newTarget = NULL;

    if (context == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);
    if (context->GetAttachedRenderTarget() != NULL) return YDS_ERROR_RETURN(ysError::YDS_CONTEXT_ALREADY_HAS_RENDER_TARGET);

    ysOpenGLRenderTarget *newRenderTarget = m_renderTargets.NewGeneric<ysOpenGLRenderTarget>();
    ysOpenGLVirtualContext *openglContext = static_cast<ysOpenGLVirtualContext *>(context);

    openglContext->m_attachedRenderTarget = newRenderTarget;

    newRenderTarget->m_type = ysRenderTarget::Type::OnScreen;
    newRenderTarget->m_posX = 0;
    newRenderTarget->m_posY = 0;
    newRenderTarget->m_width = context->GetWindow()->GetScreenWidth();
    newRenderTarget->m_height = context->GetWindow()->GetScreenHeight();
    newRenderTarget->m_format = ysRenderTarget::Format::RTF_R8G8B8A8_UNORM;
    newRenderTarget->m_hasDepthBuffer = depthBuffer;
    newRenderTarget->m_associatedContext = context;

    *newTarget = newRenderTarget;

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::CreateOffScreenRenderTarget(ysRenderTarget **newTarget, int width, int height, ysRenderTarget::Format format, int sampleCount, bool depthBuffer) {
    YDS_ERROR_DECLARE("CreateOffScreenRenderTarget");

    if (newTarget == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);

    ysOpenGLRenderTarget *newRenderTarget = m_renderTargets.NewGeneric<ysOpenGLRenderTarget>();

    ysError result = CreateOpenGLOffScreenRenderTarget(newRenderTarget, width, height, format, sampleCount, depthBuffer);
    if (result != ysError::YDS_NO_ERROR) {
        m_renderTargets.Delete(newRenderTarget->GetIndex());
        return result;
    }

    *newTarget = newRenderTarget;

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::CreateSubRenderTarget(ysRenderTarget **newTarget, ysRenderTarget *parent, int x, int y, int width, int height) {
    YDS_ERROR_DECLARE("CreateSubRenderTarget");

    if (newTarget == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);
    if (parent->GetType() == ysRenderTarget::Type::Subdivision) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);

    ysOpenGLRenderTarget *newRenderTarget = m_renderTargets.NewGeneric<ysOpenGLRenderTarget>();

    newRenderTarget->m_type = ysRenderTarget::Type::Subdivision;
    newRenderTarget->m_posX = x;
    newRenderTarget->m_posY = y;
    newRenderTarget->m_width = width;
    newRenderTarget->m_height = height;
    newRenderTarget->m_format = ysRenderTarget::Format::RTF_R8G8B8A8_UNORM;
    newRenderTarget->m_hasDepthBuffer = parent->HasDepthBuffer();
    newRenderTarget->m_associatedContext = parent->GetAssociatedContext();
    newRenderTarget->m_parent = parent;

    *newTarget = static_cast<ysRenderTarget *>(newRenderTarget);

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::ResizeRenderTarget(ysRenderTarget *target, int width, int height) {
    YDS_ERROR_DECLARE("ResizeRenderTarget");

    if (target == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);

    YDS_NESTED_ERROR_CALL(ysDevice::ResizeRenderTarget(target, width, height));


    ysRenderTarget *prevTarget = m_activeRenderTarget;
    if (target == m_activeRenderTarget) {
        SetRenderTarget(NULL);
    }

    if (target->GetType() == ysRenderTarget::Type::OnScreen) {
        // Nothing needs to be done
    }
    else if (target->GetType() == ysRenderTarget::Type::OffScreen) {
        YDS_NESTED_ERROR_CALL(CreateOpenGLOffScreenRenderTarget(target, width, height, target->GetFormat(), target->GetSampleCount(), target->HasDepthBuffer()));
    }
    else if (target->GetType() == ysRenderTarget::Type::Subdivision) {
        // Nothing needs to be done
    }

    if (target == prevTarget) {
        SetRenderTarget(target);
    }

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::SetRenderTarget(ysRenderTarget *target) {
    YDS_ERROR_DECLARE("SetRenderTarget");

    if (target) {
        ysOpenGLRenderTarget *openglTarget = static_cast<ysOpenGLRenderTarget *>(target);
        ysOpenGLRenderTarget *realTarget = (target->GetType() == ysRenderTarget::Type::Subdivision) ?
            static_cast<ysOpenGLRenderTarget *>(target->GetParent()) : openglTarget;

        if (realTarget != m_activeRenderTarget) {
            if (target->GetAssociatedContext()) {
                SetRenderingContext(target->GetAssociatedContext());
            }
        }

        if (realTarget->GetType() == ysRenderTarget::Type::OnScreen) {
            m_realContext->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        }
        else if (realTarget->GetType() == ysRenderTarget::Type::OffScreen) {
            m_realContext->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, realTarget->GetFramebuffer());
        }

        if (target->HasDepthBuffer()) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);

        int refw, refh;

        refw = realTarget->GetWidth();
        refh = realTarget->GetHeight();

        glViewport(target->GetPosX(), refh - target->GetPosY() - target->GetHeight(), target->GetWidth(), target->GetHeight());
    }

    YDS_NESTED_ERROR_CALL(ysDevice::SetRenderTarget(target));

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::DestroyRenderTarget(ysRenderTarget *&target) {
    YDS_ERROR_DECLARE("DestroyRenderTarget");

    if (target == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);

    if (target == m_activeRenderTarget) {
        YDS_NESTED_ERROR_CALL(SetRenderTarget(NULL));
    }

    ysOpenGLRenderTarget *openglTarget = static_cast<ysOpenGLRenderTarget *>(target);

    if (target->GetType() == ysRenderTarget::Type::OffScreen) {
        unsigned int buffers[] = { openglTarget->GetFramebuffer(), openglTarget->GetDepthBuffer() };
        m_realContext->glDeleteRenderbuffers(2, buffers);

        unsigned int texture = openglTarget->GetTexture();
        glDeleteTextures(1, &texture);

    }

    YDS_NESTED_ERROR_CALL(ysDevice::DestroyRenderTarget(target));

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::ClearBuffers(const float *clearColor) {
    YDS_ERROR_DECLARE("ClearBuffers");

    if (!m_deviceCreated) return YDS_ERROR_RETURN(ysError::YDS_NO_DEVICE);

    if (m_activeRenderTarget != NULL) {
        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
        glClearDepth(1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
    }

    return YDS_ERROR_RETURN(ysError::YDS_NO_RENDER_TARGET);
}

ysError ysOpenGLDevice::Present() {
    YDS_ERROR_DECLARE("Present");

    if (m_activeContext == NULL) return YDS_ERROR_RETURN(ysError::YDS_NO_CONTEXT);
    if (m_activeRenderTarget->GetType() == ysRenderTarget::Type::Subdivision) return YDS_ERROR_RETURN(ysError::YDS_INVALID_OPERATION);

    ysOpenGLVirtualContext *currentContext = static_cast<ysOpenGLVirtualContext *>(m_activeContext);
    currentContext->Present();

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

// Vertex Buffers
ysError ysOpenGLDevice::CreateVertexBuffer(ysGPUBuffer **newBuffer, int size, char *data, bool mirrorToRam) {
    YDS_ERROR_DECLARE("CreateVertexBuffer");

    if (newBuffer == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);
    *newBuffer = NULL;

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
        if (data != NULL) memcpy(newOpenGLBuffer->m_RAMMirror, data, size);
    }

    *newBuffer = static_cast<ysGPUBuffer *>(newOpenGLBuffer);

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::CreateIndexBuffer(ysGPUBuffer **newBuffer, int size, char *data, bool mirrorToRam) {
    YDS_ERROR_DECLARE("CreateIndexBuffer");

    if (newBuffer == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);
    *newBuffer = NULL;

    ysOpenGLGPUBuffer *newOpenGLBuffer = m_gpuBuffers.NewGeneric<ysOpenGLGPUBuffer>();

    newOpenGLBuffer->m_size = size;
    newOpenGLBuffer->m_mirrorToRAM = mirrorToRam;
    newOpenGLBuffer->m_bufferType = ysGPUBuffer::GPU_INDEX_BUFFER;

    m_realContext->glGenBuffers(1, &newOpenGLBuffer->m_bufferHandle);
    m_realContext->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newOpenGLBuffer->m_bufferHandle);
    m_realContext->glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, (void *)data, GL_STATIC_DRAW);

    if (mirrorToRam) {
        newOpenGLBuffer->m_RAMMirror = new char[size];
        if (data != NULL) memcpy(newOpenGLBuffer->m_RAMMirror, data, size);

    }

    *newBuffer = static_cast<ysGPUBuffer *>(newOpenGLBuffer);

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::CreateConstantBuffer(ysGPUBuffer **newBuffer, int size, char *data, bool mirrorToRam) {
    YDS_ERROR_DECLARE("CreateConstantBuffer");

    if (newBuffer == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);
    *newBuffer = NULL;

    ysOpenGLGPUBuffer *newOpenGLBuffer = m_gpuBuffers.NewGeneric<ysOpenGLGPUBuffer>();

    newOpenGLBuffer->m_size = size;
    newOpenGLBuffer->m_mirrorToRAM = mirrorToRam;
    newOpenGLBuffer->m_bufferType = ysGPUBuffer::GPU_CONSTANT_BUFFER;

    m_realContext->glGenBuffers(1, &newOpenGLBuffer->m_bufferHandle);
    m_realContext->glBindBuffer(GL_UNIFORM_BUFFER, newOpenGLBuffer->m_bufferHandle);
    m_realContext->glBufferData(GL_UNIFORM_BUFFER, size, (void *)data, GL_DYNAMIC_DRAW);

    if (mirrorToRam) {
        newOpenGLBuffer->m_RAMMirror = new char[size];
        if (data != NULL) memcpy(newOpenGLBuffer->m_RAMMirror, data, size);
    }

    *newBuffer = static_cast<ysGPUBuffer *>(newOpenGLBuffer);

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::DestroyGPUBuffer(ysGPUBuffer *&buffer) {
    YDS_ERROR_DECLARE("DestroyGPUBuffer");

    if (!CheckCompatibility(buffer))	return YDS_ERROR_RETURN(ysError::YDS_INCOMPATIBLE_PLATFORMS);
    if (buffer == NULL)					return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);

    ysOpenGLGPUBuffer *openglBuffer = static_cast<ysOpenGLGPUBuffer *>(buffer);
    m_realContext->glDeleteBuffers(1, &openglBuffer->m_bufferHandle);

    if (openglBuffer->m_bufferType == ysGPUBuffer::GPU_DATA_BUFFER) {
        m_realContext->glDeleteVertexArrays(1, &openglBuffer->m_vertexArrayHandle);
    }

    YDS_NESTED_ERROR_CALL(ysDevice::DestroyGPUBuffer(buffer));

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::UseVertexBuffer(ysGPUBuffer *buffer, int stride, int offset) {
    YDS_ERROR_DECLARE("UseVertexBuffer");

    if (!CheckCompatibility(buffer)) return YDS_ERROR_RETURN(ysError::YDS_INCOMPATIBLE_PLATFORMS);

    if (buffer == NULL) {
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
            if (openglCurrentIndexBuffer) m_realContext->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openglCurrentIndexBuffer->m_bufferHandle);
        }
    }

    YDS_NESTED_ERROR_CALL(ysDevice::UseVertexBuffer(buffer, stride, offset));
    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::UseIndexBuffer(ysGPUBuffer *buffer, int offset) {
    YDS_ERROR_DECLARE("UseIndexBuffer");

    if (!CheckCompatibility(buffer)) return YDS_ERROR_RETURN(ysError::YDS_INCOMPATIBLE_PLATFORMS);

    if (buffer == NULL) {
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

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::UseConstantBuffer(ysGPUBuffer *buffer, int slot) {
    YDS_ERROR_DECLARE("UseConstantBuffer");

    if (buffer == NULL) {
        m_realContext->glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    else {
        ysOpenGLGPUBuffer *openglBuffer = static_cast<ysOpenGLGPUBuffer *>(buffer);
        m_realContext->glBindBufferRange(GL_UNIFORM_BUFFER, slot, openglBuffer->m_bufferHandle, 0, 12);
    }

    YDS_NESTED_ERROR_CALL(ysDevice::UseConstantBuffer(buffer, slot));

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::EditBufferDataRange(ysGPUBuffer *buffer, char *data, int size, int offset) {
    YDS_ERROR_DECLARE("EditBufferDataRange");

    if (!CheckCompatibility(buffer))			return YDS_ERROR_RETURN(ysError::YDS_INCOMPATIBLE_PLATFORMS);
    if (buffer == NULL || data == NULL)			return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);
    if ((size + offset) > buffer->GetSize())	return YDS_ERROR_RETURN(ysError::YDS_OUT_OF_BOUNDS);
    if (size < 0 || offset < 0)					return YDS_ERROR_RETURN(ysError::YDS_OUT_OF_BOUNDS);

    ysGPUBuffer *previous = GetActiveBuffer(buffer->GetType());

    ysOpenGLGPUBuffer *openglBuffer = static_cast<ysOpenGLGPUBuffer *>(buffer);
    int target = openglBuffer->GetTarget();

    m_realContext->glBindBuffer(target, openglBuffer->m_bufferHandle);

    char *mappedBuffer = (char *)m_realContext->glMapBufferRange(target, offset, size, GL_WRITE_ONLY);
    memcpy(mappedBuffer, data, buffer->GetSize());

    m_realContext->glUnmapBuffer(target);

    // Restore Previous State
    if (previous != buffer) {
        openglBuffer = static_cast<ysOpenGLGPUBuffer *>(previous);
        m_realContext->glBindBuffer(target, (openglBuffer) ? openglBuffer->m_bufferHandle : NULL);
    }

    YDS_NESTED_ERROR_CALL(ysDevice::EditBufferDataRange(buffer, data, size, offset));

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::EditBufferData(ysGPUBuffer *buffer, char *data) {
    YDS_ERROR_DECLARE("EditBufferData");

    if (!CheckCompatibility(buffer))		return YDS_ERROR_RETURN(ysError::YDS_INCOMPATIBLE_PLATFORMS);
    if (buffer == NULL || data == NULL)		return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);

    ysGPUBuffer *previous = GetActiveBuffer(buffer->GetType());

    ysOpenGLGPUBuffer *openglBuffer = static_cast<ysOpenGLGPUBuffer *>(buffer);
    int target = openglBuffer->GetTarget();

    m_realContext->glBindBuffer(target, openglBuffer->m_bufferHandle);

    char *mappedBuffer = (char *)m_realContext->glMapBuffer(target, GL_WRITE_ONLY);

    memcpy(mappedBuffer, data, buffer->GetSize());

    m_realContext->glUnmapBuffer(target);

    // Restore Previous State
    if (previous != buffer) {
        openglBuffer = static_cast<ysOpenGLGPUBuffer *>(previous);
        m_realContext->glBindBuffer(target, (openglBuffer) ? openglBuffer->m_bufferHandle : NULL);
    }

    ysDevice::EditBufferData(buffer, data);

    YDS_NESTED_ERROR_CALL(ysDevice::EditBufferData(buffer, data));

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

// Shaders
ysError ysOpenGLDevice::CreateVertexShader(ysShader **newShader, const char *shaderFilename, const char *shaderName) {
    YDS_ERROR_DECLARE("CreateVertexShader");

    if (newShader == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);
    *newShader = NULL;

    if (shaderFilename == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);
    if (shaderName == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);

    glGetError();

    ysFile file;
    int fileLength;
    char *fileBuffer;
    unsigned int handle;
    GLenum result;

    YDS_NESTED_ERROR_CALL(file.OpenFile(shaderFilename, ysFile::FILE_BINARY | ysFile::FILE_READ));
    fileLength = file.GetFileLength();
    fileBuffer = new char[fileLength + 1];
    file.ReadFileToBuffer(fileBuffer);
    fileBuffer[fileLength] = 0;

    handle = m_realContext->glCreateShader(GL_VERTEX_SHADER); glGetError();
    if (handle == NULL) return YDS_ERROR_RETURN(ysError::YDS_COULD_NOT_CREATE_SHADER);

    m_realContext->glShaderSource(handle, 1, &fileBuffer, NULL); result = glGetError();
    m_realContext->glCompileShader(handle);

    delete[] fileBuffer;

    // Check Compilation
    int shaderCompiled;

    m_realContext->glGetShaderiv(handle, GL_COMPILE_STATUS, &shaderCompiled);

    if (!shaderCompiled) {
        char errorBuffer[2048 + 1];
        m_realContext->glGetShaderInfoLog(handle, 2048, NULL, errorBuffer);

        return YDS_ERROR_RETURN_MSG(ysError::YDS_VERTEX_SHADER_COMPILATION_ERROR, errorBuffer);
    }

    ysOpenGLShader *newOpenGLShader = m_shaders.NewGeneric<ysOpenGLShader>();
    strcpy_s(newOpenGLShader->m_shaderName, 64, shaderName);
    strcpy_s(newOpenGLShader->m_filename, 64, shaderFilename);
    newOpenGLShader->m_shaderType = ysShader::SHADER_TYPE_VERTEX;
    newOpenGLShader->m_handle = handle;

    *newShader = static_cast<ysShader *>(newOpenGLShader);

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::CreatePixelShader(ysShader **newShader, const char *shaderFilename, const char *shaderName) {
    YDS_ERROR_DECLARE("CreatePixelShader");

    if (newShader == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);
    *newShader = NULL;

    if (shaderFilename == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);
    if (shaderName == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);

    GLenum result = glGetError();

    ysFile file;
    int fileLength;
    char *fileBuffer;

    unsigned int shaderHandle;

    shaderHandle = m_realContext->glCreateShader(GL_FRAGMENT_SHADER);
    if (shaderHandle == NULL) return YDS_ERROR_RETURN(ysError::YDS_COULD_NOT_CREATE_SHADER);

    YDS_NESTED_ERROR_CALL(file.OpenFile(shaderFilename, ysFile::FILE_BINARY | ysFile::FILE_READ));
    fileLength = file.GetFileLength();
    fileBuffer = new char[fileLength + 1];
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

        m_realContext->glGetShaderInfoLog(shaderHandle, 2048, NULL, errorBuffer);
        return YDS_ERROR_RETURN_MSG(ysError::YDS_FRAGMENT_SHADER_COMPILATION_ERROR, errorBuffer);

    }

    ysOpenGLShader *newOpenGLShader = m_shaders.NewGeneric<ysOpenGLShader>();
    strcpy_s(newOpenGLShader->m_shaderName, 64, shaderName);
    strcpy_s(newOpenGLShader->m_filename, 64, shaderFilename);
    newOpenGLShader->m_shaderType = ysShader::SHADER_TYPE_PIXEL;
    newOpenGLShader->m_handle = shaderHandle;

    *newShader = static_cast<ysShader *>(newOpenGLShader);

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::DestroyShader(ysShader *&shader) {
    YDS_ERROR_DECLARE("DestroyShader");

    if (shader == nullptr) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);

    ysOpenGLShader *openglShader = static_cast<ysOpenGLShader *>(shader);
    m_realContext->glDeleteShader(openglShader->m_handle);

    YDS_NESTED_ERROR_CALL(ysDevice::DestroyShader(shader));

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::DestroyShaderProgram(ysShaderProgram *&program, bool destroyShaders) {
    YDS_ERROR_DECLARE("DestroyShaderProgram");

    if (program == nullptr) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);

    ysOpenGLShaderProgram *openglProgram = static_cast<ysOpenGLShaderProgram *>(program);

    for (int i = 0; i < ysShader::SHADER_TYPE_NUM_TYPES; i++) {

        ysOpenGLShader *openglShader = static_cast<ysOpenGLShader *>(openglProgram->m_shaderSlots[i]);
        m_realContext->glDetachShader(openglProgram->m_handle, openglShader->m_handle);

    }

    m_realContext->glDeleteProgram(openglProgram->m_handle);

    YDS_NESTED_ERROR_CALL(ysDevice::DestroyShaderProgram(program, destroyShaders));

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::AttachShader(ysShaderProgram *program, ysShader *shader) {
    YDS_ERROR_DECLARE("AttachShader");

    YDS_NESTED_ERROR_CALL(ysDevice::AttachShader(program, shader));

    ysOpenGLShaderProgram *openglProgram = static_cast<ysOpenGLShaderProgram *>(program);
    ysOpenGLShader *openglShader = static_cast<ysOpenGLShader *>(shader);
    m_realContext->glAttachShader(openglProgram->m_handle, openglShader->m_handle);

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::LinkProgram(ysShaderProgram *program) {
    YDS_ERROR_DECLARE("LinkProgram");

    YDS_NESTED_ERROR_CALL(ysDevice::LinkProgram(program));

    ysOpenGLShaderProgram *openglProgram = static_cast<ysOpenGLShaderProgram *>(program);
    m_realContext->glLinkProgram(openglProgram->m_handle);

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::UseShaderProgram(ysShaderProgram *program) {
    YDS_ERROR_DECLARE("UseShaderProgram");

    if (!CheckCompatibility(program)) return YDS_ERROR_RETURN(ysError::YDS_INCOMPATIBLE_PLATFORMS);

    if (m_activeShaderProgram == program) return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);

    if (program) {
        ysOpenGLShaderProgram *openglProgram = static_cast<ysOpenGLShaderProgram *>(program);
        m_realContext->glUseProgram(openglProgram->m_handle);
    }
    else {
        m_realContext->glUseProgram(NULL);
    }

    YDS_NESTED_ERROR_CALL(ysDevice::UseShaderProgram(program));

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::CreateShaderProgram(ysShaderProgram **program) {
    YDS_ERROR_DECLARE("CreateShaderProgram");

    if (program == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);

    ysOpenGLShaderProgram *newProgram = m_shaderPrograms.NewGeneric<ysOpenGLShaderProgram>();
    newProgram->m_handle = m_realContext->glCreateProgram();

    *program = static_cast<ysShaderProgram *>(newProgram);

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

// Input Layouts
ysError ysOpenGLDevice::CreateInputLayout(ysInputLayout **newInputLayout, ysShader *shader, ysRenderGeometryFormat *format) {
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

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::UseInputLayout(ysInputLayout *layout) {
    YDS_ERROR_DECLARE("UseInputLayout");

    if (layout == m_activeInputLayout) return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
    if (layout == NULL) return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);

    YDS_NESTED_ERROR_CALL(ysDevice::UseInputLayout(layout));

    if (m_activeVertexBuffer == NULL) return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);

    ysOpenGLInputLayout *openglLayout = static_cast<ysOpenGLInputLayout *>(layout);
    int nChannels = openglLayout->m_channels.GetNumObjects();

    for (int i = 0; i < nChannels; i++) {
        ysOpenGLLayoutChannel *channel = openglLayout->m_channels.Get(i);
        m_realContext->glVertexAttribPointer(i, channel->m_length, channel->m_type, GL_FALSE, openglLayout->m_size, (void *)channel->m_offset);
        m_realContext->glEnableVertexAttribArray(i);
    }

    int lastError = glGetError();

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::DestroyInputLayout(ysInputLayout *&layout) {
    YDS_ERROR_DECLARE("DestroyInputLayout");

    YDS_NESTED_ERROR_CALL(ysDevice::DestroyInputLayout(layout));

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

unsigned int ysOpenGLDevice::GetPixel(SDL_Surface *surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
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
        return 0;	// Avoid warnings
    }

}

ysError ysOpenGLDevice::CreateTexture(ysTexture **texture, const char *fname) {
    YDS_ERROR_DECLARE("CreateTexture");

    if (texture == nullptr) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);
    *texture = nullptr;

    if (fname == nullptr) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);

    bool useAlpha = true;

    // Use SDL to load the image
    SDL_Surface *pTexSurface = IMG_Load(fname); 

    if (pTexSurface == nullptr) {
        const char *err = IMG_GetError();
        return YDS_ERROR_RETURN_MSG(ysError::YDS_COULD_NOT_OPEN_FILE, fname);
    }

    ysOpenGLTexture *newTexture = m_textures.NewGeneric<ysOpenGLTexture>();
    strcpy_s(newTexture->m_filename, 257, fname);

    glGenTextures(1, &newTexture->m_handle);
    glBindTexture(GL_TEXTURE_2D, newTexture->m_handle);

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

            imageData[index] = r; index++;
            imageData[index] = g; index++;
            imageData[index] = b; index++;

            if (useAlpha) {
                imageData[index] = a; index++;
            }
        }
    }

    int texType = (useAlpha) ? GL_RGBA : GL_RGB;

    if (!useAlpha) glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    else glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glTexImage2D(GL_TEXTURE_2D, 0, texType, newTexture->m_width, newTexture->m_height, 0, texType, GL_UNSIGNED_BYTE, imageData);
    m_realContext->glGenerateMipmap(GL_TEXTURE_2D); // TEMP

    SDL_FreeSurface(pTexSurface);
    delete[] imageData;

    *texture = static_cast<ysTexture *>(newTexture);

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::UseTexture(ysTexture *texture, int slot) {
    YDS_ERROR_DECLARE("UseTexture");
    YDS_NESTED_ERROR_CALL(ysDevice::UseTexture(texture, slot));

    if (texture) {
        ysOpenGLTexture *openglTexture = static_cast<ysOpenGLTexture *>(texture);

        m_realContext->glActiveTexture(slot + GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, openglTexture->m_handle);
    }
    else {
        m_realContext->glActiveTexture(slot + GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, NULL);

    }

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::UseRenderTargetAsTexture(ysRenderTarget *texture, int slot) {
    YDS_ERROR_DECLARE("UseTexture");
    YDS_NESTED_ERROR_CALL(ysDevice::UseRenderTargetAsTexture(texture, slot));

    if (texture != NULL) {
        ysOpenGLRenderTarget *openglTexture = static_cast<ysOpenGLRenderTarget *>(texture);

        m_realContext->glActiveTexture(slot + GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, openglTexture->GetTexture());
    }
    else {
        m_realContext->glActiveTexture(slot + GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, NULL);
    }

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::DestroyTexture(ysTexture *&texture) {
    YDS_ERROR_DECLARE("DestroyTexture");

    if (texture == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);

    ysOpenGLTexture *openglTexture = static_cast<ysOpenGLTexture *>(texture);

    for (int i = 0; i < m_maxTextureSlots; i++) {
        if (m_activeTextures[i].Texture == texture) {
            UseTexture(NULL, i);
        }
    }

    glDeleteTextures(1, &openglTexture->m_handle);

    YDS_NESTED_ERROR_CALL(ysDevice::DestroyTexture(texture));

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

void ysOpenGLDevice::ResubmitInputLayout() {
    if (m_activeInputLayout == NULL) return;

    int lastError = glGetError();

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

        lastError = glGetError();
        int a = 0;
    }
}

const ysOpenGLVirtualContext *ysOpenGLDevice::UpdateContext() {
    for (int i = 0; i < m_renderingContexts.GetNumObjects(); i++) {

        ysOpenGLVirtualContext *openglContext = static_cast<ysOpenGLVirtualContext *>(m_renderingContexts.Get(i));
        if (openglContext->IsRealContext())
            return m_realContext = openglContext;

    }

    return NULL;
}

ysOpenGLVirtualContext *ysOpenGLDevice::GetTransferContext() {
    for (int i = 0; i < m_renderingContexts.GetNumObjects(); i++) {
        ysOpenGLVirtualContext *openglContext = static_cast<ysOpenGLVirtualContext *>(m_renderingContexts.Get(i));
        if (!openglContext->IsRealContext()) return openglContext;
    }

    return NULL;
}

int ysOpenGLDevice::GetFormatGLType(ysRenderGeometryChannel::CHANNEL_FORMAT format) {
    switch (format) {
    case ysRenderGeometryChannel::CHANNEL_FORMAT_R32G32B32_FLOAT:
    case ysRenderGeometryChannel::CHANNEL_FORMAT_R32G32_FLOAT:
    case ysRenderGeometryChannel::CHANNEL_FORMAT_R32G32B32A32_FLOAT:
        return GL_FLOAT;
    case ysRenderGeometryChannel::CHANNEL_FORMAT_R32G32B32A32_UINT:
        return GL_UNSIGNED_INT;
    case ysRenderGeometryChannel::CHANNEL_FORMAT_R32G32B32_UINT:
        return GL_UNSIGNED_INT;
    default:
        // No real option here
        return GL_4_BYTES;
    }
}

// TEMP
void ysOpenGLDevice::Draw(int numFaces, int indexOffset, int vertexOffset) {
    if (m_activeVertexBuffer) {
        m_realContext->glDrawElementsBaseVertex(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_SHORT, (void *)(indexOffset * 2), vertexOffset);
    }
}

ysError ysOpenGLDevice::CreateOpenGLOffScreenRenderTarget(ysRenderTarget *target, int width, int height, ysRenderTarget::Format format, int sampleCount, bool depthBuffer) {
    YDS_ERROR_DECLARE("CreateOpenGLOffScreenRenderTarget");

    // Generate the empty texture
    unsigned int newTexture;
    glGenTextures(1, &newTexture);
    glBindTexture(GL_TEXTURE_2D, newTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int glFormat;
    int glType;

    switch (format) {
    case ysRenderTarget::Format::RTF_R8G8B8A8_UNORM:
        glFormat = GL_RGBA8;
        glType = GL_UNSIGNED_BYTE;
        break;
    case ysRenderTarget::Format::RTF_R32G32B32_FLOAT:
        glFormat = GL_RGBA32F_ARB;
        glType = GL_FLOAT;
        break;
    default:
        return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, glFormat, width, height, 0, GL_RGBA, glType, NULL);

    // Create a depth buffer
    unsigned int depthBufferHandle = 0;

    if (depthBuffer) {
        m_realContext->glGenRenderbuffers(1, &depthBufferHandle);
        m_realContext->glBindRenderbuffer(GL_RENDERBUFFER, depthBufferHandle);
        m_realContext->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    }

    // Create a framebuffer
    unsigned int framebuffer;
    m_realContext->glGenFramebuffers(1, &framebuffer);
    m_realContext->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    m_realContext->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, newTexture, 0);

    if (depthBuffer) m_realContext->glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferHandle);

    ysOpenGLRenderTarget *newRenderTarget = static_cast<ysOpenGLRenderTarget *>(target);

    newRenderTarget->m_type = ysRenderTarget::Type::OffScreen;
    newRenderTarget->m_posX = 0;
    newRenderTarget->m_posY = 0;
    newRenderTarget->m_width = width;
    newRenderTarget->m_height = height;
    newRenderTarget->m_format = format;
    newRenderTarget->m_hasDepthBuffer = depthBuffer;
    newRenderTarget->m_associatedContext = NULL;

    newRenderTarget->m_textureHandle = newTexture;
    newRenderTarget->m_depthBufferHandle = depthBufferHandle;
    newRenderTarget->m_framebufferHandle = framebuffer;

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysOpenGLDevice::DestroyOpenGLRenderTarget(ysRenderTarget *target) {
    YDS_ERROR_DECLARE("DestroyOpenGLRenderTarget");

    ysOpenGLRenderTarget *openglTarget = static_cast<ysOpenGLRenderTarget *>(target);

    if (target->GetType() == ysRenderTarget::Type::OffScreen) {
        unsigned int buffers[] = { openglTarget->GetFramebuffer(), openglTarget->GetDepthBuffer() };
        m_realContext->glDeleteRenderbuffers(2, buffers);

        unsigned int texture = openglTarget->GetTexture();
        glDeleteTextures(1, &texture);
    }

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}
