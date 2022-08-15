#include "../include/yds_opengl_sdl_context.h"

#include "../include/yds_opengl_device.h"
#include "../include/yds_sdl_window.h"
#include "../include/yds_sdl_window_system.h"

ysOpenGLSDLContext::ysOpenGLSDLContext() : ysOpenGLVirtualContext(ysWindowSystem::Platform::Sdl) {
    m_device = nullptr;
}

ysOpenGLSDLContext::~ysOpenGLSDLContext() {
    /* void */
}

ysError ysOpenGLSDLContext::CreateRenderingContext(ysOpenGLDevice *device, ysWindow *window, int major, int minor) {
    YDS_ERROR_DECLARE("CreateRenderingContext");

    if (window->GetPlatform() != ysWindowSystemObject::Platform::Sdl) {
        return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    }

    ysSdlWindow *sdlWindow = static_cast<ysSdlWindow *>(window);

    // TODO: more options here to match Windows impl
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    m_context = SDL_GL_CreateContext(sdlWindow->m_window);

    LoadAllExtensions();

    m_device = device;
    m_isRealContext = true;
    m_targetWindow = window;

    device->UpdateContext();

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLSDLContext::DestroyContext() {
    YDS_ERROR_DECLARE("DestroyContext");

    SDL_GL_MakeCurrent(nullptr, nullptr);
    if (m_context) {
        SDL_GL_DeleteContext(m_context);
        m_context = nullptr;
        //if (result == FALSE) return YDS_ERROR_RETURN(ysError::CouldNotDestroyContext);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLSDLContext::TransferContext(ysOpenGLVirtualContext *context) {
    YDS_ERROR_DECLARE("TransferContext");

    if (context->GetPlatform() != ysWindowSystemObject::Platform::Sdl) return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);

    ysOpenGLSDLContext *windowsContext = static_cast<ysOpenGLSDLContext *>(context);
    windowsContext->m_context = m_context;
    windowsContext->m_isRealContext = true;

    m_isRealContext = false;
    m_context = nullptr;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLSDLContext::SetContextMode(ContextMode mode) {
    YDS_ERROR_DECLARE("SetContextMode");

    ysWindow *window = GetWindow();

    if (mode == ysRenderingContext::ContextMode::Fullscreen) {
        window->SetWindowStyle(ysWindow::WindowStyle::Fullscreen);
    } else if (mode == ysRenderingContext::ContextMode::Windowed) {
        window->SetWindowStyle(ysWindow::WindowStyle::Windowed);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLSDLContext::SetContext(ysRenderingContext *realContext) {
    YDS_ERROR_DECLARE("SetContext");

    ysOpenGLSDLContext *realOpenglContext = static_cast<ysOpenGLSDLContext *>(realContext);
    ysSdlWindow *sdlWindow = static_cast<ysSdlWindow *>(realOpenglContext->m_targetWindow);

    if (realContext != nullptr) {
        int result = SDL_GL_MakeCurrent(sdlWindow->m_window, realOpenglContext->m_context);
        if (result != 0) {
            return YDS_ERROR_RETURN(ysError::CouldNotActivateContext);
        }
    } else {
        SDL_GL_MakeCurrent(NULL, NULL);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLSDLContext::Present() {
    YDS_ERROR_DECLARE("Present");

    ysSdlWindow *sdlWindow = static_cast<ysSdlWindow *>(m_targetWindow);
    SDL_GL_SwapWindow(sdlWindow->m_window);

    return YDS_ERROR_RETURN(ysError::None);
}

void ysOpenGLSDLContext::LoadAllExtensions() {
    // TODO: assert that these have been loaded

    glGenBuffers = (PFNGLGENBUFFERSPROC)SDL_GL_GetProcAddress("glGenBuffers");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)SDL_GL_GetProcAddress("glDeleteBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)SDL_GL_GetProcAddress("glBindBuffer");
    glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)SDL_GL_GetProcAddress("glBindBufferRange");
    glBufferData = (PFNGLBUFFERDATAPROC)SDL_GL_GetProcAddress("glBufferData");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)SDL_GL_GetProcAddress("glGenVertexArrays");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)SDL_GL_GetProcAddress("glDeleteVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)SDL_GL_GetProcAddress("glBindVertexArray");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)SDL_GL_GetProcAddress("glEnableVertexAttribArray");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)SDL_GL_GetProcAddress("glVertexAttribPointer");
    glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)SDL_GL_GetProcAddress("glVertexAttribIPointer");
    glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC)SDL_GL_GetProcAddress("glVertexAttrib3f");
    glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC)SDL_GL_GetProcAddress("glVertexAttrib4f");

    // Shaders
    glDeleteShader = (PFNGLDELETESHADERPROC)SDL_GL_GetProcAddress("glDeleteShader");
    glDeleteProgram = (PFNGLDELETEPROGRAMPROC)SDL_GL_GetProcAddress("glDeleteProgram");

    glCreateShader = (PFNGLCREATESHADERPROC)SDL_GL_GetProcAddress("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)SDL_GL_GetProcAddress("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)SDL_GL_GetProcAddress("glCompileShader");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)SDL_GL_GetProcAddress("glCreateProgram");
    glAttachShader = (PFNGLATTACHSHADERPROC)SDL_GL_GetProcAddress("glAttachShader");
    glDetachShader = (PFNGLDETACHSHADERPROC)SDL_GL_GetProcAddress("glDetachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)SDL_GL_GetProcAddress("glLinkProgram");
    glUseProgram = (PFNGLUSEPROGRAMPROC)SDL_GL_GetProcAddress("glUseProgram");
    glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)SDL_GL_GetProcAddress("glBindAttribLocation");
    glBindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONPROC)SDL_GL_GetProcAddress("glBindFragDataLocation");
    glGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC)SDL_GL_GetProcAddress("glGetFragDataLocation");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)SDL_GL_GetProcAddress("glGetUniformLocation");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)SDL_GL_GetProcAddress("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)SDL_GL_GetProcAddress("glGetShaderInfoLog");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)SDL_GL_GetProcAddress("glGetProgramInfoLog");
    glDrawBuffers = (PFNGLDRAWBUFFERSPROC)SDL_GL_GetProcAddress("glDrawBuffers");

    glUniform4fv = (PFNGLUNIFORM4FVPROC)SDL_GL_GetProcAddress("glUniform4fv");
    glUniform3fv = (PFNGLUNIFORM3FVPROC)SDL_GL_GetProcAddress("glUniform3fv");
    glUniform2fv = (PFNGLUNIFORM2FVPROC)SDL_GL_GetProcAddress("glUniform2fv");
    glUniform4f = (PFNGLUNIFORM4FPROC)SDL_GL_GetProcAddress("glUniform4f");
    glUniform3f = (PFNGLUNIFORM3FPROC)SDL_GL_GetProcAddress("glUniform3f");
    glUniform2f = (PFNGLUNIFORM2FPROC)SDL_GL_GetProcAddress("glUniform2f");
    glUniform1f = (PFNGLUNIFORM1FPROC)SDL_GL_GetProcAddress("glUniform1f");
    glUniform1i = (PFNGLUNIFORM1IPROC)SDL_GL_GetProcAddress("glUniform1i");

    glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)SDL_GL_GetProcAddress("glUniformMatrix4fv");
    glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)SDL_GL_GetProcAddress("glUniformMatrix3fv");

    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)SDL_GL_GetProcAddress("glGetProgramiv");

    glGetActiveUniformName = (PFNGLGETACTIVEUNIFORMNAMEPROC)SDL_GL_GetProcAddress("glGetActiveUniformName");
    glGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC)SDL_GL_GetProcAddress("glGetActiveUniformsiv");
    glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)SDL_GL_GetProcAddress("glGetActiveUniform");

    glMapBuffer = (PFNGLMAPBUFFERPROC)SDL_GL_GetProcAddress("glMapBuffer");
    glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC)SDL_GL_GetProcAddress("glMapBufferRange");
    glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)SDL_GL_GetProcAddress("glUnmapBuffer");

    glDrawElementsBaseVertex = (PFNGLDRAWELEMENTSBASEVERTEXPROC)SDL_GL_GetProcAddress("glDrawElementsBaseVertex");

    // Textures
    glActiveTexture = (PFNGLACTIVETEXTUREPROC)SDL_GL_GetProcAddress("glActiveTexture");
    glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)SDL_GL_GetProcAddress("glGenerateMipmap");

    glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)SDL_GL_GetProcAddress("glTexImage2DMultisample");

    // Buffers
    glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)SDL_GL_GetProcAddress("glGenRenderbuffers");
    glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)SDL_GL_GetProcAddress("glDeleteRenderbuffers");
    glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)SDL_GL_GetProcAddress("glBindRenderbuffer");
    glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)SDL_GL_GetProcAddress("glRenderbufferStorage");
    glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)SDL_GL_GetProcAddress("glRenderbufferStorageMultisample");
    glCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC)SDL_GL_GetProcAddress("glCopyBufferSubData");
    glBufferSubData = (PFNGLBUFFERSUBDATAPROC)SDL_GL_GetProcAddress("glBufferSubData");

    glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)SDL_GL_GetProcAddress("glGenFramebuffers");
    glDeleteFramebuffers = (PFNGLDELETERENDERBUFFERSPROC)SDL_GL_GetProcAddress("glDeleteFramebuffers");
    glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)SDL_GL_GetProcAddress("glBindFramebuffer");
    glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)SDL_GL_GetProcAddress("glFramebufferTexture2D");
    glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)SDL_GL_GetProcAddress("glFramebufferRenderbuffer");
    glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)SDL_GL_GetProcAddress("glCheckFramebufferStatus");

    glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)SDL_GL_GetProcAddress("glBlitFramebuffer");

    // Blending
    glBlendEquation = (PFNGLBLENDEQUATIONPROC)SDL_GL_GetProcAddress("glBlendEquation");
}
