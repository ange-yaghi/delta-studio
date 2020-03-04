#ifndef YDS_D3D11_DEVICE_H
#define YDS_D3D11_DEVICE_H

#include "yds_device.h"

// DirectX forward declarations
struct IDXGIDevice;
struct ID3D11DeviceContext;
struct IDXGIFactory;
struct ID3D11Device;
struct ID3D11RasterizerState;
struct ID3D11DepthStencilView;
enum DXGI_FORMAT;

class ysD3D11Device : public ysDevice {
    friend ysDevice;

private:
    ysD3D11Device();
    virtual ~ysD3D11Device();

public:
    // Setup
    virtual ysError InitializeDevice();
    virtual ysError DestroyDevice();
    virtual bool CheckSupport();

    // Rendering Contexts
    virtual ysError CreateRenderingContext(ysRenderingContext **renderingContext, ysWindow *window);
    virtual ysError UpdateRenderingContext(ysRenderingContext *context);
    virtual ysError DestroyRenderingContext(ysRenderingContext *&context);
    virtual ysError SetContextMode(ysRenderingContext *context, ysRenderingContext::ContextMode mode);

    virtual ysError CreateOnScreenRenderTarget(ysRenderTarget **newTarget, ysRenderingContext *context, bool depthBuffer);
    virtual ysError CreateOffScreenRenderTarget(ysRenderTarget **newTarget, int width, int height, ysRenderTarget::RENDER_TARGET_FORMAT format, int sampleCount, bool depthBuffer);
    virtual ysError CreateSubRenderTarget(ysRenderTarget **newTarget, ysRenderTarget *parent, int x, int y, int width, int height);
    virtual ysError ResizeRenderTarget(ysRenderTarget *target, int width, int height);
    virtual ysError DestroyRenderTarget(ysRenderTarget *&target);
    virtual ysError SetRenderTarget(ysRenderTarget *target);

    virtual ysError ClearBuffers(const float *clearColor);
    virtual ysError Present();

    // GPU Buffers
    virtual ysError CreateVertexBuffer(ysGPUBuffer **newBuffer, int size, char *data, bool mirrorToRam = false);
    virtual ysError CreateIndexBuffer(ysGPUBuffer **newBuffer, int size, char *data, bool mirrorToRam = false);
    virtual ysError CreateConstantBuffer(ysGPUBuffer **newBuffer, int size, char *data, bool mirrorToRam = false);
    virtual ysError UseVertexBuffer(ysGPUBuffer *buffer, int stride, int offset);
    virtual ysError UseIndexBuffer(ysGPUBuffer *buffer, int offset);
    virtual ysError UseConstantBuffer(ysGPUBuffer *buffer, int slot);
    virtual ysError EditBufferDataRange(ysGPUBuffer *buffer, char *data, int size, int offset);
    virtual ysError EditBufferData(ysGPUBuffer *buffer, char *data);
    virtual ysError DestroyGPUBuffer(ysGPUBuffer *&buffer);

    // Shaders
    virtual ysError CreateVertexShader(ysShader **newShader, const char *shaderFilename, const char *shaderName);
    virtual ysError CreatePixelShader(ysShader **newShader, const char *shaderFilename, const char *shaderName);
    virtual ysError DestroyShader(ysShader *&shader);

    // Shader Programs
    virtual ysError CreateShaderProgram(ysShaderProgram **newProgram);
    virtual ysError DestroyShaderProgram(ysShaderProgram *&shader, bool destroyShaders = false);
    virtual ysError AttachShader(ysShaderProgram *targetProgram, ysShader *shader);
    virtual ysError LinkProgram(ysShaderProgram *program);
    virtual ysError UseShaderProgram(ysShaderProgram *);

    // Input Layouts 
    virtual ysError CreateInputLayout(ysInputLayout **newLayout, ysShader *shader, ysRenderGeometryFormat *format);
    virtual ysError UseInputLayout(ysInputLayout *layout);
    virtual ysError DestroyInputLayout(ysInputLayout *&layout);


    // Textures 
    virtual ysError CreateTexture(ysTexture **texture, const char *fname);
    virtual ysError DestroyTexture(ysTexture *&texture);
    virtual ysError UseTexture(ysTexture *texture, int slot);
    virtual ysError UseRenderTargetAsTexture(ysRenderTarget *texture, int slot);

    virtual void Draw(int numFaces, int indexOffset, int vertexOffset);

public:
    // Non-standard interface
    void GetDXGIDevice(IDXGIDevice **device);
    ID3D11DeviceContext *GetImmediateContext() { return m_deviceContext; }
    IDXGIFactory *GetDXGIFactory() { return m_DXGIFactory; }
    ID3D11Device *GetDevice() { return m_device; }

    static DXGI_FORMAT ConvertInputLayoutFormat(ysRenderGeometryChannel::CHANNEL_FORMAT format);

    // TEMP
    ID3D11RasterizerState *m_rasterizerState;

protected:
    ID3D11Device *m_device;
    ID3D11DeviceContext *m_deviceContext;

    IDXGIFactory *m_DXGIFactory;

    int m_multisampleCount;
    int m_multisampleQuality;

protected:
    // Platform specific functionality
    ysError CreateD3D11DepthBuffer(ID3D11DepthStencilView **newDepthStencil, int width, int height, int count, int quality);
    ysError DestroyD3D11DepthBuffer(ID3D11DepthStencilView *&depthStencil);

    ysError CreateD3D11OnScreenRenderTarget(ysRenderTarget *target, ysRenderingContext *context, bool depthBuffer);
    ysError CreateD3D11OffScreenRenderTarget(ysRenderTarget *target, int width, int height, ysRenderTarget::RENDER_TARGET_FORMAT format, int sampleCount, bool depthBuffer);

    ysError DestroyD3D11RenderTarget(ysRenderTarget *target);
};

#endif /* YDS_D3D11_DEVICE_H */
