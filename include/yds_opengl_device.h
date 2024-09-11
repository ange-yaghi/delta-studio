#ifndef YDS_OPENGL_DEVICE_H
#define YDS_OPENGL_DEVICE_H

#include "yds_device.h"
#include "yds_opengl_context.h"
#include "yds_window.h"

//#if defined(_WIN64)

// External libraries forward declarations
struct SDL_Surface;

class ysOpenGLDevice : public ysDevice {
public:
    ysOpenGLDevice();
    ~ysOpenGLDevice();

    // Setup
    virtual ysError InitializeDevice();
    virtual ysError DestroyDevice();
    virtual bool CheckSupport();

    // Rendering Contexts
    virtual ysError
    CreateRenderingContext(ysRenderingContext **renderingContext,
                           ysWindow *window);
    virtual ysError UpdateRenderingContext(ysRenderingContext *context);
    virtual ysError DestroyRenderingContext(ysRenderingContext *&context);
    virtual ysError SetContextMode(ysRenderingContext *context,
                                   ysRenderingContext::ContextMode mode);

    virtual ysError CreateOnScreenRenderTarget(ysRenderTarget **newTarget,
                                               ysRenderingContext *context,
                                               bool depthBuffer);
    virtual ysError CreateOffScreenRenderTarget(ysRenderTarget **newTarget,
                                                int width, int height,
                                                ysRenderTarget::Format format,
                                                bool colorData,
                                                bool depthBuffer);
    virtual ysError CreateSubRenderTarget(ysRenderTarget **newTarget,
                                          ysRenderTarget *parent, int x, int y,
                                          int width, int height);
    virtual ysError ResizeRenderTarget(ysRenderTarget *target, int width,
                                       int height, int pwidth, int pheight);
    virtual ysError DestroyRenderTarget(ysRenderTarget *&target);
    virtual ysError SetRenderTarget(ysRenderTarget *target, int slot = 0);
    virtual ysError SetDepthTestEnabled(ysRenderTarget *target, bool enable);
    virtual ysError ReadRenderTarget(ysRenderTarget *src, uint8_t *target);

    virtual ysError ClearBuffers(const float *clearColor);
    virtual ysError Present();

    // State
    virtual ysError SetFaceCulling(bool faceCulling);
    virtual ysError SetFaceCullingMode(CullMode cullMode);

    // GPU Buffers
    virtual ysError CreateVertexBuffer(ysGPUBuffer **newBuffer, int size,
                                       char *data, bool mirrorToRam = false);
    virtual ysError CreateIndexBuffer(ysGPUBuffer **newBuffer, int size,
                                      char *data, bool mirrorToRam = false);
    virtual ysError CreateConstantBuffer(ysGPUBuffer **newBuffer, int size,
                                         char *data, bool mirrorToRam = false);
    virtual ysError UseVertexBuffer(ysGPUBuffer *buffer, int stride,
                                    int offset);
    virtual ysError UseIndexBuffer(ysGPUBuffer *buffer, int offset);
    virtual ysError UseConstantBuffer(ysGPUBuffer *buffer, int slot);
    virtual ysError EditBufferDataRange(ysGPUBuffer *buffer, char *data,
                                        int size, int offset);
    virtual ysError EditBufferData(ysGPUBuffer *buffer, char *data);
    virtual ysError DestroyGPUBuffer(ysGPUBuffer *&buffer);

    // Shaders
    virtual ysError CreateVertexShader(ysShader **newShader,
                                       const wchar_t *shaderFilename,
                                       const wchar_t *compiledFilename,
                                       const char *shaderName,
                                       bool compile) override;
    virtual ysError CreatePixelShader(ysShader **newShader,
                                      const wchar_t *shaderFilename,
                                      const wchar_t *compiledFilename,
                                      const char *shaderName,
                                      bool compile) override;
    virtual ysError DestroyShader(ysShader *&shader) override;

    // Shader Programs
    virtual ysError CreateShaderProgram(ysShaderProgram **newProgram);
    virtual ysError DestroyShaderProgram(ysShaderProgram *&shader,
                                         bool destroyShaders = false);
    virtual ysError AttachShader(ysShaderProgram *targetProgram,
                                 ysShader *shader);
    virtual ysError LinkProgram(ysShaderProgram *program);
    virtual ysError UseShaderProgram(ysShaderProgram *);

    // Input Layouts
    virtual ysError
    CreateInputLayout(ysInputLayout **newLayout, ysShader *shader,
                      const ysRenderGeometryFormat *format,
                      const ysRenderGeometryFormat *instanceFormat);
    virtual ysError UseInputLayout(ysInputLayout *layout);
    virtual ysError DestroyInputLayout(ysInputLayout *&layout);

    // Textures
    virtual ysError CreateTexture(ysTexture **texture,
                                  const wchar_t *fname) override;
    virtual ysError CreateTexture(ysTexture **texture, int width, int height,
                                  const unsigned char *buffer) override;
    virtual ysError UpdateTexture(ysTexture *texture,
                                  const unsigned char *buffer) override;
    virtual ysError CreateAlphaTexture(ysTexture **texture, int width,
                                       int height,
                                       const unsigned char *buffer) override;
    virtual ysError DestroyTexture(ysTexture *&texture) override;
    virtual ysError UseTexture(ysTexture *texture, int slot) override;
    virtual ysError UseRenderTargetAsTexture(ysRenderTarget *renderTarget,
                                             int slot) override;

    virtual void Draw(int numFaces, int indexOffset, int vertexOffset);

    const ysOpenGLVirtualContext *UpdateContext();

protected:
    // Set the current rendering context
    void SetRenderingContext(ysRenderingContext *context);

    // Hidden Functions
    void ResubmitInputLayout();

    ysOpenGLVirtualContext *GetTransferContext();

    // Static functions
    // --------------------------------------------------------------

    // Get a GL type from a geometry channel format
    static int GetFormatGLType(ysRenderGeometryChannel::ChannelFormat format);
    static int GetFramebufferName(int slot);

protected:
    ysOpenGLVirtualContext *m_realContext;

    bool m_deviceCreated;

protected:
    // Hidden functionality
    ysError CreateOpenGLOffScreenRenderTarget(ysRenderTarget *target, int width,
                                              int height,
                                              ysRenderTarget::Format format,
                                              bool colorData, bool depthBuffer);
    ysError DestroyOpenGLRenderTarget(ysRenderTarget *target);
};

//#endif /* Windows */

#endif /* YDS_OPENGL_DEVICE_H */
