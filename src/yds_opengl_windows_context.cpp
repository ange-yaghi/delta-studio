#include "../include/yds_opengl_windows_context.h"

#include "../include/yds_opengl_device.h"
#include "../include/yds_windows_window.h"
#include "../include/yds_windows_window_system.h"

ysOpenGLWindowsContext::ysOpenGLWindowsContext() : ysOpenGLVirtualContext(ysWindowSystem::Platform::Windows) {
    m_contextHandle = nullptr;
    m_device = nullptr;
    m_deviceHandle = nullptr;
}

ysOpenGLWindowsContext::~ysOpenGLWindowsContext() {
    /* void */
}

ysError ysOpenGLWindowsContext::CreateRenderingContext(ysOpenGLDevice *device, ysWindow *window, int major, int minor) {
    YDS_ERROR_DECLARE("CreateRenderingContext");

    if (window->GetPlatform() != ysWindowSystemObject::Platform::Windows) {
        return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    }

    GLenum result = glGetError();
    BOOL wresult = 0;

    ysWindowsWindow *windowsWindow = static_cast<ysWindowsWindow *>(window);

    HDC deviceHandle = GetDC(windowsWindow->GetWindowHandle());

    // Default
    m_device = nullptr;
    m_deviceHandle = NULL;
    m_targetWindow = NULL;
    m_contextHandle = NULL;
    m_isRealContext = false;
    
    // Create dummy context
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)(ysWindowsWindowSystem::WinProc);
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = windowsWindow->GetInstance();
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "DUMMY_WINDOW";
    wc.hIconSm = NULL;

    RegisterClassEx(&wc);

    HWND dummyWnd = CreateWindow("DUMMY_WINDOW", "", 0, 0, 0, 0, 0, 0, 0, windowsWindow->GetInstance(), 0);
    HDC dummyDeviceHandle = GetDC(dummyWnd);

    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;

    unsigned int pixelFormat = ChoosePixelFormat(dummyDeviceHandle, &pfd);
    SetPixelFormat(dummyDeviceHandle, pixelFormat, &pfd);

    HGLRC tempContext = wglCreateContext(dummyDeviceHandle);
    if (tempContext == nullptr) return YDS_ERROR_RETURN(ysError::CouldNotCreateTemporaryContext);

    wresult = wglMakeCurrent(dummyDeviceHandle, tempContext);
    if (wresult == FALSE) return YDS_ERROR_RETURN(ysError::CouldNotActivateTemporaryContext);

    LoadContextCreationExtension();

    const int contextAttribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, major,
        WGL_CONTEXT_MINOR_VERSION_ARB, minor,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    const int pixelFormatAttributes[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        WGL_SAMPLE_BUFFERS_ARB, 1,
        WGL_SAMPLES_ARB, 8,
        0
    };

    int msPixelFormat;
    UINT numFormats;

    wglChoosePixelFormatARB(deviceHandle, pixelFormatAttributes, NULL, 1, &msPixelFormat, &numFormats);
    SetPixelFormat(deviceHandle, msPixelFormat, &pfd);

    wglMakeCurrent(0, 0);

    // Create a context if one doesn't already exist
    if (device->UpdateContext() == nullptr) {
        HGLRC hRC = wglCreateContextAttribsARB(deviceHandle, 0, contextAttribs);
        if (hRC == 0) return YDS_ERROR_RETURN(ysError::CouldNotCreateContext);

        wresult = wglMakeCurrent(deviceHandle, hRC);
        if (wresult == FALSE) {
            wglDeleteContext(hRC);
            return YDS_ERROR_RETURN(ysError::CouldNotActivateContext);
        }

        m_contextHandle = hRC;
        m_isRealContext = true;

        device->UpdateContext();
    }

    wglDeleteContext(tempContext);
    DestroyWindow(dummyWnd);

    m_device = device;
    m_deviceHandle = deviceHandle;
    m_targetWindow = window;

    LoadAllExtensions();

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLWindowsContext::DestroyContext() {
    YDS_ERROR_DECLARE("DestroyContext");

    wglMakeCurrent(NULL, NULL);
    if (m_contextHandle) {
        BOOL result = wglDeleteContext(m_contextHandle);
        if (result == FALSE) return YDS_ERROR_RETURN(ysError::CouldNotDestroyContext);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLWindowsContext::TransferContext(ysOpenGLVirtualContext *context) {
    YDS_ERROR_DECLARE("TransferContext");

    if (context->GetPlatform() != ysWindowSystemObject::Platform::Windows) return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);

    ysOpenGLWindowsContext *windowsContext = static_cast<ysOpenGLWindowsContext *>(context);
    windowsContext->m_contextHandle = m_contextHandle;
    windowsContext->m_isRealContext = true;

    m_isRealContext = false;
    m_contextHandle = NULL;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLWindowsContext::SetContextMode(ContextMode mode) {
    YDS_ERROR_DECLARE("SetContextMode");

    ysWindow *window = GetWindow();
    ysMonitor *monitor = window->GetMonitor();
    int result;

    if (mode == ysRenderingContext::ContextMode::Fullscreen) {
        window->SetWindowStyle(ysWindow::WindowStyle::Fullscreen);

        DEVMODE dmScreenSettings;
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = monitor->GetPhysicalWidth();
        dmScreenSettings.dmPelsHeight = monitor->GetPhysicalHeight();
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        result = ChangeDisplaySettingsEx(monitor->GetDeviceName(), &dmScreenSettings, NULL, CDS_FULLSCREEN | CDS_UPDATEREGISTRY, NULL);
        if (result != DISP_CHANGE_SUCCESSFUL) {
            return YDS_ERROR_RETURN(ysError::CouldNotEnterFullscreen);
        }
    }
    else if (mode == ysRenderingContext::ContextMode::Windowed) {
        window->SetWindowStyle(ysWindow::WindowStyle::Windowed);

        result = ChangeDisplaySettingsEx(NULL, NULL, NULL, 0, NULL);
        if (result != DISP_CHANGE_SUCCESSFUL) {
            return YDS_ERROR_RETURN(ysError::CouldNotExitFullscreen);
        }
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLWindowsContext::SetContext(ysRenderingContext *realContext) {
    YDS_ERROR_DECLARE("SetContext");

    ysOpenGLWindowsContext *realOpenglContext = static_cast<ysOpenGLWindowsContext *>(realContext);
    BOOL result;

    if (realContext != nullptr) {
        result = wglMakeCurrent(m_deviceHandle, realOpenglContext->m_contextHandle);
        if (result == FALSE) {
            DWORD lastError = GetLastError();

            return YDS_ERROR_RETURN(ysError::CouldNotActivateContext);
        }
    }
    else {
        wglMakeCurrent(NULL, NULL);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysOpenGLWindowsContext::Present() {
    YDS_ERROR_DECLARE("Present");

    if (SwapBuffers(m_deviceHandle) == FALSE) return YDS_ERROR_RETURN(ysError::BufferSwapError);

    return YDS_ERROR_RETURN(ysError::None);
}

void ysOpenGLWindowsContext::LoadContextCreationExtension() {
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
}

void ysOpenGLWindowsContext::LoadAllExtensions() {
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

    glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
    glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)wglGetProcAddress("glBindBufferRange");
    glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
    glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)wglGetProcAddress("glVertexAttribIPointer");
    glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC)wglGetProcAddress("glVertexAttrib3f");
    glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC)wglGetProcAddress("glVertexAttrib4f");

    // Shaders
    glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
    glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");

    glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
    glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
    glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
    glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
    glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
    glBindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONPROC)wglGetProcAddress("glBindFragDataLocation");
    glGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC)wglGetProcAddress("glGetFragDataLocation");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
    glDrawBuffers = (PFNGLDRAWBUFFERSPROC)wglGetProcAddress("glDrawBuffers");

    glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
    glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
    glUniform2fv = (PFNGLUNIFORM2FVPROC)wglGetProcAddress("glUniform2fv");
    glUniform4f = (PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f");
    glUniform3f = (PFNGLUNIFORM3FPROC)wglGetProcAddress("glUniform3f");
    glUniform2f = (PFNGLUNIFORM2FPROC)wglGetProcAddress("glUniform2f");
    glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
    glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");

    glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
    glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)wglGetProcAddress("glUniformMatrix3fv");

    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");

    glGetActiveUniformName = (PFNGLGETACTIVEUNIFORMNAMEPROC)wglGetProcAddress("glGetActiveUniformName");
    glGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC)wglGetProcAddress("glGetActiveUniformsiv");
    glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)wglGetProcAddress("glGetActiveUniform");

    glMapBuffer = (PFNGLMAPBUFFERPROC)wglGetProcAddress("glMapBuffer");
    glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC)wglGetProcAddress("glMapBufferRange");
    glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)wglGetProcAddress("glUnmapBuffer");

    glDrawElementsBaseVertex = (PFNGLDRAWELEMENTSBASEVERTEXPROC)wglGetProcAddress("glDrawElementsBaseVertex");

    // Textures
    glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
    glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");

    glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)wglGetProcAddress("glTexImage2DMultisample");

    // Buffers
    glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)wglGetProcAddress("glGenRenderbuffers");
    glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)wglGetProcAddress("glDeleteRenderbuffers");
    glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)wglGetProcAddress("glBindRenderbuffer");
    glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)wglGetProcAddress("glRenderbufferStorage");
    glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)wglGetProcAddress("glRenderbufferStorageMultisample");
    glCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC)wglGetProcAddress("glCopyBufferSubData");
    glBufferSubData = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");

    glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers");
    glDeleteFramebuffers = (PFNGLDELETERENDERBUFFERSPROC)wglGetProcAddress("glDeleteFramebuffers");
    glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
    glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D");
    glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)wglGetProcAddress("glFramebufferRenderbuffer");
    glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus");

    glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)wglGetProcAddress("glBlitFramebuffer");

    // Blending
    glBlendEquation = (PFNGLBLENDEQUATIONPROC)wglGetProcAddress("glBlendEquation");

    wglMakeContextCurrent = (PFNWGLMAKECONTEXTCURRENTARBPROC)wglGetProcAddress("wglMakeContextCurrentARB");
}
