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

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)
    #include "win32/windows_modular.h"

    #ifndef DXGI_FORMAT_DEFINED
    #define DXGI_FORMAT_DEFINED 1
    typedef enum DXGI_FORMAT
    {
        DXGI_FORMAT_UNKNOWN                        = 0,
        DXGI_FORMAT_R32G32B32A32_TYPELESS       = 1,
        DXGI_FORMAT_R32G32B32A32_FLOAT          = 2,
        DXGI_FORMAT_R32G32B32A32_UINT           = 3,
        DXGI_FORMAT_R32G32B32A32_SINT           = 4,
        DXGI_FORMAT_R32G32B32_TYPELESS          = 5,
        DXGI_FORMAT_R32G32B32_FLOAT             = 6,
        DXGI_FORMAT_R32G32B32_UINT              = 7,
        DXGI_FORMAT_R32G32B32_SINT              = 8,
        DXGI_FORMAT_R16G16B16A16_TYPELESS       = 9,
        DXGI_FORMAT_R16G16B16A16_FLOAT          = 10,
        DXGI_FORMAT_R16G16B16A16_UNORM          = 11,
        DXGI_FORMAT_R16G16B16A16_UINT           = 12,
        DXGI_FORMAT_R16G16B16A16_SNORM          = 13,
        DXGI_FORMAT_R16G16B16A16_SINT           = 14,
        DXGI_FORMAT_R32G32_TYPELESS             = 15,
        DXGI_FORMAT_R32G32_FLOAT                = 16,
        DXGI_FORMAT_R32G32_UINT                 = 17,
        DXGI_FORMAT_R32G32_SINT                 = 18,
        DXGI_FORMAT_R32G8X24_TYPELESS           = 19,
        DXGI_FORMAT_D32_FLOAT_S8X24_UINT        = 20,
        DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS    = 21,
        DXGI_FORMAT_X32_TYPELESS_G8X24_UINT     = 22,
        DXGI_FORMAT_R10G10B10A2_TYPELESS        = 23,
        DXGI_FORMAT_R10G10B10A2_UNORM           = 24,
        DXGI_FORMAT_R10G10B10A2_UINT            = 25,
        DXGI_FORMAT_R11G11B10_FLOAT             = 26,
        DXGI_FORMAT_R8G8B8A8_TYPELESS           = 27,
        DXGI_FORMAT_R8G8B8A8_UNORM              = 28,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB         = 29,
        DXGI_FORMAT_R8G8B8A8_UINT               = 30,
        DXGI_FORMAT_R8G8B8A8_SNORM              = 31,
        DXGI_FORMAT_R8G8B8A8_SINT               = 32,
        DXGI_FORMAT_R16G16_TYPELESS             = 33,
        DXGI_FORMAT_R16G16_FLOAT                = 34,
        DXGI_FORMAT_R16G16_UNORM                = 35,
        DXGI_FORMAT_R16G16_UINT                 = 36,
        DXGI_FORMAT_R16G16_SNORM                = 37,
        DXGI_FORMAT_R16G16_SINT                 = 38,
        DXGI_FORMAT_R32_TYPELESS                = 39,
        DXGI_FORMAT_D32_FLOAT                   = 40,
        DXGI_FORMAT_R32_FLOAT                   = 41,
        DXGI_FORMAT_R32_UINT                    = 42,
        DXGI_FORMAT_R32_SINT                    = 43,
        DXGI_FORMAT_R24G8_TYPELESS              = 44,
        DXGI_FORMAT_D24_UNORM_S8_UINT           = 45,
        DXGI_FORMAT_R24_UNORM_X8_TYPELESS       = 46,
        DXGI_FORMAT_X24_TYPELESS_G8_UINT        = 47,
        DXGI_FORMAT_R8G8_TYPELESS               = 48,
        DXGI_FORMAT_R8G8_UNORM                  = 49,
        DXGI_FORMAT_R8G8_UINT                   = 50,
        DXGI_FORMAT_R8G8_SNORM                  = 51,
        DXGI_FORMAT_R8G8_SINT                   = 52,
        DXGI_FORMAT_R16_TYPELESS                = 53,
        DXGI_FORMAT_R16_FLOAT                   = 54,
        DXGI_FORMAT_D16_UNORM                   = 55,
        DXGI_FORMAT_R16_UNORM                   = 56,
        DXGI_FORMAT_R16_UINT                    = 57,
        DXGI_FORMAT_R16_SNORM                   = 58,
        DXGI_FORMAT_R16_SINT                    = 59,
        DXGI_FORMAT_R8_TYPELESS                 = 60,
        DXGI_FORMAT_R8_UNORM                    = 61,
        DXGI_FORMAT_R8_UINT                     = 62,
        DXGI_FORMAT_R8_SNORM                    = 63,
        DXGI_FORMAT_R8_SINT                     = 64,
        DXGI_FORMAT_A8_UNORM                    = 65,
        DXGI_FORMAT_R1_UNORM                    = 66,
        DXGI_FORMAT_R9G9B9E5_SHAREDEXP          = 67,
        DXGI_FORMAT_R8G8_B8G8_UNORM             = 68,
        DXGI_FORMAT_G8R8_G8B8_UNORM             = 69,
        DXGI_FORMAT_BC1_TYPELESS                = 70,
        DXGI_FORMAT_BC1_UNORM                   = 71,
        DXGI_FORMAT_BC1_UNORM_SRGB              = 72,
        DXGI_FORMAT_BC2_TYPELESS                = 73,
        DXGI_FORMAT_BC2_UNORM                   = 74,
        DXGI_FORMAT_BC2_UNORM_SRGB              = 75,
        DXGI_FORMAT_BC3_TYPELESS                = 76,
        DXGI_FORMAT_BC3_UNORM                   = 77,
        DXGI_FORMAT_BC3_UNORM_SRGB              = 78,
        DXGI_FORMAT_BC4_TYPELESS                = 79,
        DXGI_FORMAT_BC4_UNORM                   = 80,
        DXGI_FORMAT_BC4_SNORM                   = 81,
        DXGI_FORMAT_BC5_TYPELESS                = 82,
        DXGI_FORMAT_BC5_UNORM                   = 83,
        DXGI_FORMAT_BC5_SNORM                   = 84,
        DXGI_FORMAT_B5G6R5_UNORM                = 85,
        DXGI_FORMAT_B5G5R5A1_UNORM              = 86,
        DXGI_FORMAT_B8G8R8A8_UNORM              = 87,
        DXGI_FORMAT_B8G8R8X8_UNORM              = 88,
        DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM  = 89,
        DXGI_FORMAT_B8G8R8A8_TYPELESS           = 90,
        DXGI_FORMAT_B8G8R8A8_UNORM_SRGB         = 91,
        DXGI_FORMAT_B8G8R8X8_TYPELESS           = 92,
        DXGI_FORMAT_B8G8R8X8_UNORM_SRGB         = 93,
        DXGI_FORMAT_BC6H_TYPELESS               = 94,
        DXGI_FORMAT_BC6H_UF16                   = 95,
        DXGI_FORMAT_BC6H_SF16                   = 96,
        DXGI_FORMAT_BC7_TYPELESS                = 97,
        DXGI_FORMAT_BC7_UNORM                   = 98,
        DXGI_FORMAT_BC7_UNORM_SRGB              = 99,
        DXGI_FORMAT_FORCE_UINT                  = 0xffffffff
    } DXGI_FORMAT;
    #endif

    #define FMT_DX10                (0x30315844)
    #define FMT_DXT1                (0x31545844) /* DXGI_FORMAT_BC1_UNORM */
    #define FMT_DXT3                (0x33545844) /* DXGI_FORMAT_BC2_UNORM */
    #define FMT_DXT5                (0x35545844) /* DXGI_FORMAT_BC3_UNORM */

#elif defined(_WIN64)
    enum DXGI_FORMAT;
#endif

// compiler-specific keyword which suggests that a function should be inlined
//#if defined(__APPLE__) && defined(__MACH__)
//    #define forceInline __attribute__((always_inline))
//#elif defined(_WIN64)
//    #define forceInline __forceinline
//#endif

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
    forceInline ID3D11DeviceContext *GetImmediateContext() {
        return m_deviceContext;
    }

    IDXGIFactory1 *GetDXGIFactory1() { return m_DXGIFactory1; }
    forceInline ID3D11Device *GetDevice() { return m_device; }

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
