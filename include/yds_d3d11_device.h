#ifndef YDS_D3D11_DEVICE_H
#define YDS_D3D11_DEVICE_H

#include "yds_device.h"

// DirectX forward declarations
struct IDXGIDevice;
struct ID3D11DeviceContext;
struct IDXGIFactory1;
struct IDXGIFactory2;
struct ID3D11Device;
struct ID3D11RasterizerState;
struct ID3D11DepthStencilView;
struct ID3D11ShaderResourceView;
enum DXGI_FORMAT;

class ysD3D11Device : public ysDevice {
    friend ysDevice;

private:
    ysD3D11Device();
    virtual ~ysD3D11Device();

public:
    // Setup
    virtual ysError InitializeDevice() override;
    virtual ysError DestroyDevice() override;
    virtual bool CheckSupport() override;

    // State
    virtual ysError SetFaceCulling(bool faceCulling) override;
    virtual ysError SetFaceCullingMode(CullMode cullMode) override;

    // Rendering Contexts
    virtual ysError
    CreateRenderingContext(ysRenderingContext **renderingContext,
                           ysWindow *window) override;
    virtual ysError
    UpdateRenderingContext(ysRenderingContext *context) override;
    virtual ysError
    DestroyRenderingContext(ysRenderingContext *&context) override;
    virtual ysError
    SetContextMode(ysRenderingContext *context,
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
                                       int height, int pwidth,
                                       int pheight) override;
    virtual ysError DestroyRenderTarget(ysRenderTarget *&target) override;
    virtual ysError SetRenderTarget(ysRenderTarget *target,
                                    int slot = 0) override;
    virtual ysError SetDepthTestEnabled(ysRenderTarget *target,
                                        bool enable) override;
    virtual ysError ReadRenderTarget(ysRenderTarget *src,
                                     uint8_t *target) override;

    virtual ysError ClearBuffers(const float *clearColor) override;
    virtual ysError Present() override;

    // GPU Buffers
    virtual ysError CreateVertexBuffer(ysGPUBuffer **newBuffer, int size,
                                       char *data,
                                       bool mirrorToRam = false) override;
    virtual ysError CreateIndexBuffer(ysGPUBuffer **newBuffer, int size,
                                      char *data,
                                      bool mirrorToRam = false) override;
    virtual ysError CreateConstantBuffer(ysGPUBuffer **newBuffer, int size,
                                         char *data,
                                         bool mirrorToRam = false) override;
    virtual ysError UseVertexBuffer(ysGPUBuffer *buffer, int stride,
                                    int offset) override;
    virtual ysError UseInstanceBuffer(ysGPUBuffer *buffer, int stride,
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
    virtual ysError DestroyShader(ysShader *&shader) override;

    // Shader Programs
    virtual ysError CreateShaderProgram(ysShaderProgram **newProgram) override;
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
    virtual ysError CreateTexture(ysTexture **texture,
                                  const wchar_t *fname) override;
    virtual ysError CreateTexture(ysTexture **texture, int width, int height,
                                  const unsigned char *buffer) override;
    virtual ysError CreateAlphaTexture(ysTexture **texture, int width,
                                       int height,
                                       const unsigned char *buffer) override;
    virtual ysError UpdateTexture(ysTexture *texture,
                                  const unsigned char *buffer) override;
    virtual ysError DestroyTexture(ysTexture *&texture) override;
    virtual ysError UseTexture(ysTexture *texture, int slot) override;
    virtual ysError UseRenderTargetAsTexture(ysRenderTarget *renderTarget,
                                             int slot) override;

    virtual void DrawInstanced(int numFaces, int indexOffset, int vertexOffset,
                               int instanceCount, int instanceOffset) override;
    virtual void Draw(int numFaces, int indexOffset, int vertexOffset) override;
    virtual void DrawLines(int numIndices, int indexOffset,
                           int vertexOffset) override;

public:
    // Non-standard interface
    __forceinline ID3D11DeviceContext *GetImmediateContext() {
        return m_deviceContext;
    }

    IDXGIFactory1 *GetDXGIFactory1() { return m_DXGIFactory1; }
    __forceinline ID3D11Device *GetDevice() { return m_device; }

    static DXGI_FORMAT
    ConvertInputLayoutFormat(ysRenderGeometryChannel::ChannelFormat format);

    // TEMP
    struct ID3D11RasterizerState *m_rasterizerState;
    struct ID3D11SamplerState *m_samplerState;

private:
    ID3D11Device *m_device;
    ID3D11DeviceContext *m_deviceContext;

    struct ID3D11DepthStencilState *m_depthStencilEnabledState;
    struct ID3D11DepthStencilState *m_depthStencilDisabledState;

    struct ID3D11BlendState *m_blendState;

    IDXGIFactory1 *m_DXGIFactory1;
    IDXGIFactory2 *m_DXGIFactory2;

    int m_multisampleCount;
    int m_multisampleQuality;

private:
    // Platform specific functionality
    ysError
    CreateD3D11DepthStencilView(ID3D11DepthStencilView **newDepthStencil,
                                ID3D11ShaderResourceView **shaderResourceView,
                                int width, int height, int count, int quality,
                                bool shaderResource);
    ysError DestroyD3D11DepthStencilView(ID3D11DepthStencilView *&depthStencil);

    ysError CreateD3D11OnScreenRenderTarget(ysRenderTarget *target,
                                            ysRenderingContext *context,
                                            bool depthBuffer);
    ysError CreateD3D11OffScreenRenderTarget(ysRenderTarget *target, int width,
                                             int height,
                                             ysRenderTarget::Format format,
                                             int msaa, bool colorData,
                                             bool depthBuffer);

    ysError DestroyD3D11RenderTarget(ysRenderTarget *target);
};

#endif /* YDS_D3D11_DEVICE_H */
