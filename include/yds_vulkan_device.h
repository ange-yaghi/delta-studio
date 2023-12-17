#ifndef YDS_VULKAN_DEVICE_H
#define YDS_VULKAN_DEVICE_H

#include "yds_device.h"

#include "yds_vulkan_context.h"
#include "yds_window.h"

#include "yds_vulkan_decl.h"

class ysVulkanDevice : public ysDevice {
public:
    ysVulkanDevice();
    ~ysVulkanDevice();

    // Setup
    virtual ysError InitializeDevice() override;
    virtual ysError DestroyDevice() override;
    virtual bool CheckSupport() override;

    // Rendering Contexts
    virtual ysError
    CreateRenderingContext(ysRenderingContext **renderingContext,
                           ysWindow *window) override;
    virtual ysError UpdateRenderingContext(ysRenderingContext *context) override;
    virtual ysError DestroyRenderingContext(ysRenderingContext *&context) override;
    virtual ysError SetContextMode(ysRenderingContext *context,
                                   ysRenderingContext::ContextMode mode) override;

    virtual ysError CreateOnScreenRenderTarget(ysRenderTarget **newTarget,
                                               ysRenderingContext *context,
                                               bool depthBuffer) override;
    virtual ysError CreateOffScreenRenderTarget(ysRenderTarget **newTarget,
                                                int width, int height,
                                                ysRenderTarget::Format format,
                                                bool colorData,
                                                bool depthBuffer) override;
    virtual ysError CreateSubRenderTarget(ysRenderTarget **newTarget,
                                          ysRenderTarget *parent, int x, int y,
                                          int width, int height) override;
    virtual ysError ResizeRenderTarget(ysRenderTarget *target, int width,
                                       int height, int pwidth, int pheight) override;
    virtual ysError DestroyRenderTarget(ysRenderTarget *&target) override;
    virtual ysError SetRenderTarget(ysRenderTarget *target);
    virtual ysError SetDepthTestEnabled(ysRenderTarget *target, bool enable) override;
    virtual ysError ReadRenderTarget(ysRenderTarget *src, uint8_t *target) override;

    virtual ysError ClearBuffers(const float *clearColor) override;
    virtual ysError Present() override;

    // State
    virtual ysError SetFaceCulling(bool faceCulling) override;
    virtual ysError SetFaceCullingMode(CullMode cullMode) override;

    // GPU Buffers
    virtual ysError CreateVertexBuffer(ysGPUBuffer **newBuffer, int size,
                                       char *data, bool mirrorToRam = false) override;
    virtual ysError CreateIndexBuffer(ysGPUBuffer **newBuffer, int size,
                                      char *data, bool mirrorToRam = false) override;
    virtual ysError CreateConstantBuffer(ysGPUBuffer **newBuffer, int size,
                                         char *data, bool mirrorToRam = false) override;
    virtual ysError UseVertexBuffer(ysGPUBuffer *buffer, int stride,
                                    int offset) override;
    virtual ysError UseIndexBuffer(ysGPUBuffer *buffer, int offset) override;
    virtual ysError UseConstantBuffer(ysGPUBuffer *buffer, int slot) override;
    virtual ysError EditBufferDataRange(ysGPUBuffer *buffer, char *data,
                                        int size, int offset) override;
    virtual ysError EditBufferData(ysGPUBuffer *buffer, char *data) override;
    virtual ysError DestroyGPUBuffer(ysGPUBuffer *&buffer) override;

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
    virtual ysError DestroyShader(ysShader *&shader);

    // Shader Programs
    virtual ysError CreateShaderProgram(ysShaderProgram **newProgram);
    virtual ysError DestroyShaderProgram(ysShaderProgram *&shader,
                                         bool destroyShaders = false) override;
    virtual ysError AttachShader(ysShaderProgram *targetProgram,
                                 ysShader *shader) override;
    virtual ysError LinkProgram(ysShaderProgram *program) override;
    virtual ysError UseShaderProgram(ysShaderProgram *) override;

    // Input Layouts
    virtual ysError
    CreateInputLayout(ysInputLayout **newLayout, ysShader *shader,
                      const ysRenderGeometryFormat *format,
                      const ysRenderGeometryFormat *instanceFormat) override;
    virtual ysError UseInputLayout(ysInputLayout *layout) override;
    virtual ysError DestroyInputLayout(ysInputLayout *&layout) override;

    // Textures
    virtual ysError CreateTexture(ysTexture **texture, const wchar_t *fname);
    virtual ysError CreateTexture(ysTexture **texture, int width, int height,
                                  const unsigned char *buffer) override;
    virtual ysError UpdateTexture(ysTexture *texture,
                                  const unsigned char *buffer) override;
    virtual ysError CreateAlphaTexture(ysTexture **texture, int width,
                                       int height, const unsigned char *buffer) override;
    virtual ysError DestroyTexture(ysTexture *&texture) override;
    virtual ysError UseTexture(ysTexture *texture, int slot) override;
    virtual ysError UseRenderTargetAsTexture(ysRenderTarget *renderTarget,
                                             int slot) override;

    virtual void Draw(int numFaces, int indexOffset, int vertexOffset) override;

    VkInstance GetInstance() const { return m_instance; }

protected:
    ysError CreateVulkanInstance();
    ysError CreateVulkanDevice(VkSurfaceKHR surface);

protected:
    VkInstance m_instance;
    VkDevice m_device;
};

#endif /* YDS_VULKAN_DEVICE_H */
