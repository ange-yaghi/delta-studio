#ifndef YDS_D3D11_CONTEXT_H
#define YDS_D3D11_CONTEXT_H

#include "yds_rendering_context.h"
#include "yds_window.h"

struct IDXGISwapChain;
struct IDXGISwapChain1;
struct ID3D11Texture2D;
struct ID3D11DepthStencilView;
struct ID3D11RenderTargetView;

#define YDS_D3D11_USE_FLIP_MODEL true

class ysD3D11RenderTarget;
class ysD3D11Context : public ysRenderingContext {
    friend class ysD3D11Device;

public:
    ysD3D11Context();
    virtual ~ysD3D11Context();

private:
    IDXGISwapChain *m_swapChain;
    IDXGISwapChain1 *m_swapChain1;

#if YDS_D3D11_USE_FLIP_MODEL
    ysD3D11RenderTarget *m_msaaRenderTarget = nullptr;
#endif
};

#endif /* YDS_D3D11_CONTEXT_H */
