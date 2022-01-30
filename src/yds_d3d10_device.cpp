#include "../include/yds_d3d10_device.h"

#include "../include/yds_d3d10_context.h"
#include "../include/yds_d3d10_render_target.h"
#include "../include/yds_d3d10_gpu_buffer.h"
#include "../include/yds_d3d10_shader.h"
#include "../include/yds_d3d10_shader_program.h"
#include "../include/yds_d3d10_input_layout.h"
#include "../include/yds_d3d10_texture.h"

#include "../include/yds_windows_window.h"

#include <D3D10.h>

#pragma warning(push, 0)
#include <d3dx10.h>
#include <d3dx10async.h>
#include <d3dx10tex.h>
#pragma warning(pop)

ysD3D10Device::ysD3D10Device() : ysDevice(DeviceAPI::DirectX10) {
    m_device = nullptr;
    m_DXGIFactory = nullptr;

    // TEMP
    m_rasterizerState = nullptr;
}

ysD3D10Device::~ysD3D10Device() {
    /* void */
}

ysError ysD3D10Device::InitializeDevice() {
    YDS_ERROR_DECLARE("InitializeDevice");

    HRESULT result = D3D10CreateDevice(nullptr,
        D3D10_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D10_CREATE_DEVICE_DEBUG,
        D3D10_SDK_VERSION,
        &m_device);

    if (FAILED(result)) {
        m_device = nullptr;
        return YDS_ERROR_RETURN(ysError::CouldNotCreateGraphicsDevice);
    }

    result = CreateDXGIFactory(IID_IDXGIFactory, (void**)(&m_DXGIFactory));
    if (FAILED(result)) {
        m_device->Release();
        m_device = nullptr;

        return YDS_ERROR_RETURN(ysError::CouldNotCreateGraphicsDevice);
    }

    // TEMP
    // This initialization probably should not be here.
    m_device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    InitializeTextureSlots(D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::DestroyDevice() {
    YDS_ERROR_DECLARE("DestroyDevice");

    if (m_device)        m_device->Release();
    if (m_DXGIFactory)    m_DXGIFactory->Release();

    return YDS_ERROR_RETURN(ysError::None);
}

bool ysD3D10Device::CheckSupport() {
    // TEMP
    return true;
}

ysError ysD3D10Device::SetFaceCulling(bool faceCulling) {
    return ysError::NotImplemented;
}

ysError ysD3D10Device::SetFaceCullingMode(CullMode cullMode) {
    return ysError::NotImplemented;
}

ysError ysD3D10Device::CreateRenderingContext(ysRenderingContext **context, ysWindow *window) {
    YDS_ERROR_DECLARE("CreateRenderingContext");

    if (window->GetPlatform() != ysWindowSystem::Platform::Windows) return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    if (context == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (m_device == nullptr) return YDS_ERROR_RETURN(ysError::NoDevice);
    *context = nullptr;

    ysWindowsWindow *windowsWindow = static_cast<ysWindowsWindow *>(window);

    IDXGIDevice *pDXGIDevice = nullptr;
    GetDXGIDevice(&pDXGIDevice);
    if (pDXGIDevice == nullptr) {
        return YDS_ERROR_RETURN(ysError::CouldNotObtainDevice);
    }

    ysD3D10Context *newContext = m_renderingContexts.NewGeneric<ysD3D10Context>();
    newContext->m_targetWindow = window;

    // Create the swap chain

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory( &swapChainDesc, sizeof( DXGI_SWAP_CHAIN_DESC ) );
    swapChainDesc.BufferDesc.Width = window->GetGameWidth();
    swapChainDesc.BufferDesc.Height = window->GetGameHeight();
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.OutputWindow = windowsWindow->GetWindowHandle();
    swapChainDesc.Windowed = window->GetWindowStyle() != ysWindow::WindowStyle::Fullscreen;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    IDXGIFactory *factory = GetDXGIFactory();

    HRESULT result;
    result = factory->CreateSwapChain(pDXGIDevice, &swapChainDesc, &newContext->m_swapChain);
    pDXGIDevice->Release();

    if (FAILED(result)) {
        m_renderingContexts.Delete(newContext->GetIndex());
        *context = nullptr;

        return YDS_ERROR_RETURN(ysError::CouldNotCreateSwapChain);
    }

    // TEMP

    if (!m_rasterizerState) {
        D3D10_RASTERIZER_DESC rasterizerDescription;
        ZeroMemory(&rasterizerDescription, sizeof(D3D10_RASTERIZER_DESC));
        rasterizerDescription.FillMode = D3D10_FILL_SOLID;
        rasterizerDescription.CullMode = D3D10_CULL_FRONT;
        rasterizerDescription.FrontCounterClockwise = false;
        rasterizerDescription.DepthBias = false;
        rasterizerDescription.DepthBiasClamp = 0;
        rasterizerDescription.SlopeScaledDepthBias = 0;
        rasterizerDescription.DepthClipEnable = false;
        rasterizerDescription.ScissorEnable = false;
        rasterizerDescription.MultisampleEnable = true;
        rasterizerDescription.AntialiasedLineEnable = true;

        m_device->CreateRasterizerState(&rasterizerDescription, &m_rasterizerState);
        m_device->RSSetState(m_rasterizerState);

        // TEMPORARY ALPHA ENABLING
        ID3D10BlendState* g_pBlendStateNoBlend = nullptr;

        D3D10_BLEND_DESC BlendState;
        ZeroMemory(&BlendState, sizeof(D3D10_BLEND_DESC));
        BlendState.BlendEnable[0] = TRUE;
        BlendState.SrcBlend = D3D10_BLEND_SRC_ALPHA;
        BlendState.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
        BlendState.BlendOp = D3D10_BLEND_OP_ADD;
        BlendState.SrcBlendAlpha = D3D10_BLEND_SRC_ALPHA;
        BlendState.DestBlendAlpha = D3D10_BLEND_DEST_ALPHA;
        BlendState.BlendOpAlpha = D3D10_BLEND_OP_ADD;
        BlendState.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
        m_device->CreateBlendState(&BlendState, &g_pBlendStateNoBlend);

        float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        UINT sampleMask = 0xffffffff;

        m_device->OMSetBlendState(g_pBlendStateNoBlend, blendFactor, sampleMask);

        // END TEMPORARY ALPHA BLENDING
    }

    // END TEMP

    *context = static_cast<ysRenderingContext *>(newContext);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::UpdateRenderingContext(ysRenderingContext *context) {
    YDS_ERROR_DECLARE("UpdateRenderingContext");

    if (context == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (m_device == nullptr) return YDS_ERROR_RETURN(ysError::NoDevice);

    // Check the window
    if (!context->GetWindow()->IsVisible()) return YDS_ERROR_RETURN(ysError::None);

    ysD3D10Context *d3d11Context = static_cast<ysD3D10Context *>(context);
    
    const int width = context->GetWindow()->GetGameWidth();
    const int height = context->GetWindow()->GetGameHeight();

    ysD3D10RenderTarget *attachedTarget = static_cast<ysD3D10RenderTarget *>(context->GetAttachedRenderTarget());

    // Destroy render target first

    if (attachedTarget != nullptr) {
        YDS_NESTED_ERROR_CALL( DestroyD3D10RenderTarget(attachedTarget) );
    }

    HRESULT result = d3d11Context->m_swapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::ApiError);
    }

    const int pwidth = context->GetWindow()->GetScreenWidth();
    const int pheight = context->GetWindow()->GetScreenHeight();

    if (context->GetAttachedRenderTarget() != nullptr) {
        YDS_NESTED_ERROR_CALL( ResizeRenderTarget(context->GetAttachedRenderTarget(), width, height, pwidth, pheight) );
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::DestroyRenderingContext(ysRenderingContext *&context) {
    YDS_ERROR_DECLARE("DestroyRenderingContext");

    // TEMP

    if (m_rasterizerState) {
        m_rasterizerState->Release();
        m_rasterizerState = nullptr;
    }

    // END TEMP

    if (context) {
        YDS_NESTED_ERROR_CALL( SetContextMode(context, ysRenderingContext::ContextMode::Windowed) );

        ysD3D10Context *d3d10Context = static_cast<ysD3D10Context *>(context);
        ULONG result;

        if (d3d10Context->m_swapChain) 
            result = d3d10Context->m_swapChain->Release();
    }

    YDS_NESTED_ERROR_CALL( ysDevice::DestroyRenderingContext(context) );

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::SetContextMode(ysRenderingContext *context, ysRenderingContext::ContextMode mode) {
    YDS_ERROR_DECLARE("SetContextMode");

    if (context == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (!CheckCompatibility(context)) return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);

    ysD3D10Context *d3d10Context = static_cast<ysD3D10Context *>(context);
    ysWindow *window = context->GetWindow();
    ysMonitor *monitor = window->GetMonitor();

    HRESULT result;

    if (mode == ysRenderingContext::ContextMode::Fullscreen) {
        window->SetWindowStyle(ysWindow::WindowStyle::Fullscreen);
        //d3d10Context->m_swapChain->ResizeBuffers
        result = d3d10Context->m_swapChain->SetFullscreenState(TRUE, nullptr);

        if (FAILED(result))
            return YDS_ERROR_RETURN(ysError::CouldNotEnterFullscreen);
    }
    else if (mode == ysRenderingContext::ContextMode::Windowed) {
        window->SetWindowStyle(ysWindow::WindowStyle::Windowed);
        result = d3d10Context->m_swapChain->SetFullscreenState(FALSE, nullptr);

        if (FAILED(result))
            return YDS_ERROR_RETURN(ysError::CouldNotExitFullscreen);
    }

    YDS_NESTED_ERROR_CALL( ysDevice::SetContextMode(context, mode) );

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::CreateOnScreenRenderTarget(ysRenderTarget **newTarget, ysRenderingContext *context, bool depthBuffer) {
    YDS_ERROR_DECLARE("CreateOnScreenRenderTarget");

    if (newTarget == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newTarget = nullptr;

    if (context == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (context->GetAttachedRenderTarget() != nullptr) return YDS_ERROR_RETURN(ysError::ContextAlreadyHasRenderTarget);

    ysD3D10RenderTarget *newRenderTarget = m_renderTargets.NewGeneric<ysD3D10RenderTarget>();

    ysError result = CreateD3D10OnScreenRenderTarget(newRenderTarget, context, depthBuffer);

    if (result != ysError::None) {
        m_renderTargets.Delete(newRenderTarget->GetIndex());
        return YDS_ERROR_RETURN(result);
    }

    *newTarget = static_cast<ysRenderTarget *>(newRenderTarget);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::CreateOffScreenRenderTarget(ysRenderTarget **newTarget, int width, int height, ysRenderTarget::Format format, bool colorData, bool depthBuffer) {
    YDS_ERROR_DECLARE("CreateOffScreenRenderTarget");

    if (newTarget == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newTarget = nullptr;

    ysD3D10RenderTarget *d3d10Target = m_renderTargets.NewGeneric<ysD3D10RenderTarget>();

    ysError result = CreateD3D10OffScreenRenderTarget(d3d10Target, width, height, format, colorData, depthBuffer);

    if (result != ysError::None) {
        m_renderTargets.Delete(d3d10Target->GetIndex());
        return YDS_ERROR_RETURN(result);
    }

    *newTarget = static_cast<ysRenderTarget *>(d3d10Target);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::CreateSubRenderTarget(ysRenderTarget **newTarget, ysRenderTarget *parent, int x, int y, int width, int height) {
    YDS_ERROR_DECLARE("CreateSubRenderTarget");

    if (newTarget == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (parent->GetType() == ysRenderTarget::Type::Subdivision) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ysD3D10RenderTarget *newRenderTarget = m_renderTargets.NewGeneric<ysD3D10RenderTarget>();

    newRenderTarget->m_type = ysRenderTarget::Type::Subdivision;
    newRenderTarget->m_posX = x;
    newRenderTarget->m_posY = y;
    newRenderTarget->m_width = width;
    newRenderTarget->m_height = height;
    newRenderTarget->m_format = ysRenderTarget::Format::R8G8B8A8_UNORM;
    newRenderTarget->m_hasDepthBuffer = parent->HasDepthBuffer();
    newRenderTarget->m_associatedContext = parent->GetAssociatedContext();
    newRenderTarget->m_parent = parent;

    newRenderTarget->m_renderTargetView = nullptr;
    newRenderTarget->m_depthStencil = nullptr;

    *newTarget = static_cast<ysRenderTarget *>(newRenderTarget);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::ResizeRenderTarget(ysRenderTarget *target, int width, int height, int pwidth, int pheight) {
    YDS_ERROR_DECLARE("ResizeRenderTarget");

    YDS_NESTED_ERROR_CALL( ysDevice::ResizeRenderTarget(target, width, height, pwidth, pheight) );

    ysD3D10RenderTarget *d3d10Target = static_cast<ysD3D10RenderTarget *>(target);

    if (target->GetType() == ysRenderTarget::Type::OnScreen) {
        YDS_NESTED_ERROR_CALL( CreateD3D10OnScreenRenderTarget(target, target->GetAssociatedContext(), target->HasDepthBuffer()) );
        return YDS_ERROR_RETURN(ysError::None);
    }
    else if (target->GetType() == ysRenderTarget::Type::OffScreen) {
        YDS_NESTED_ERROR_CALL( CreateD3D10OffScreenRenderTarget(target, width, height, target->GetFormat(), target->HasColorData(), target->HasDepthBuffer()) );
        return YDS_ERROR_RETURN(ysError::NotImplemented);
    }
    else if (target->GetType() == ysRenderTarget::Type::Subdivision) {
        // Nothing needs to be done
        return YDS_ERROR_RETURN(ysError::None);
    }

    for (int i = 0; i < MaxRenderTargets; ++i) {
        if (target == m_activeRenderTarget[i]) {
            SetRenderTarget(target, i);
        }
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::SetRenderTarget(ysRenderTarget *newTarget, int slot) {
    YDS_ERROR_DECLARE("SetRenderTarget");

    if (newTarget) {
        ysD3D10RenderTarget* d3d10Target = static_cast<ysD3D10RenderTarget*>(newTarget);
        ysD3D10RenderTarget* realTarget = (newTarget->GetType() == ysRenderTarget::Type::Subdivision) ?
            static_cast<ysD3D10RenderTarget*>(newTarget->GetParent()) : d3d10Target;

        if (realTarget != m_activeRenderTarget[slot]) {
            GetDevice()->OMSetRenderTargets(1, &realTarget->m_renderTargetView, realTarget->m_depthStencil);
        }

        if (newTarget->GetAssociatedContext()) {
            m_activeContext = newTarget->GetAssociatedContext();
        }

        D3D10_VIEWPORT vp;
        vp.Width = (UINT)d3d10Target->GetWidth();
        vp.Height = (UINT)d3d10Target->GetHeight();
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = (UINT)d3d10Target->GetPosX();
        vp.TopLeftY = (UINT)d3d10Target->GetPosY();
        GetDevice()->RSSetViewports(1, &vp);

    }
    else {
        GetDevice()->OMSetRenderTargets(0, nullptr, nullptr);
        m_activeContext = nullptr;
    }

    YDS_NESTED_ERROR_CALL( ysDevice::SetRenderTarget(newTarget, slot) );

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::ReadRenderTarget(ysRenderTarget *src, uint8_t *target) {
    return ysError();
}

ysError ysD3D10Device::DestroyRenderTarget(ysRenderTarget *&target) {
    YDS_ERROR_DECLARE("DestroyRenderTarget");

    if (target == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    for (int i = 0; i < MaxRenderTargets; ++i) {
        if (target == m_activeRenderTarget[i]) {
            YDS_NESTED_ERROR_CALL(SetRenderTarget(nullptr, i));
        }
    }

    YDS_NESTED_ERROR_CALL( DestroyD3D10RenderTarget(target) );
    YDS_NESTED_ERROR_CALL( ysDevice::DestroyRenderTarget(target) );

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::ClearBuffers(const float *clearColor) {
    YDS_ERROR_DECLARE("ClearBuffers");

    if (GetDevice() == nullptr) return YDS_ERROR_RETURN(ysError::NoDevice);

    for (int i = 0; i < MaxRenderTargets; ++i) {
        ysRenderTarget *actualRenderTarget = GetActualRenderTarget(i);
        if (m_activeRenderTarget != nullptr) {
            ysD3D10RenderTarget *renderTarget = static_cast<ysD3D10RenderTarget *>(actualRenderTarget);
            m_device->ClearRenderTargetView(renderTarget->m_renderTargetView, clearColor);
            if (renderTarget->m_hasDepthBuffer) {
                m_device->ClearDepthStencilView(renderTarget->m_depthStencil, D3D10_CLEAR_DEPTH, 1.0f, 0);
            }

            return YDS_ERROR_RETURN(ysError::None);
        }
    }

    return YDS_ERROR_RETURN(ysError::NoRenderTarget);
}

ysError ysD3D10Device::Present() {
    YDS_ERROR_DECLARE("Present");

    if (m_activeContext == nullptr) return YDS_ERROR_RETURN(ysError::NoContext);
    if (m_activeRenderTarget[0]->GetType() == ysRenderTarget::Type::Subdivision) return YDS_ERROR_RETURN(ysError::InvalidOperation);

    ysD3D10Context *context = static_cast<ysD3D10Context *>(m_activeContext);
    if (context->m_swapChain == nullptr) return YDS_ERROR_RETURN(ysError::NoContext);

    context->m_swapChain->Present(1, 0);

    return YDS_ERROR_RETURN(ysError::None);
}

// Vertex Buffers
ysError ysD3D10Device::CreateVertexBuffer(ysGPUBuffer **newBuffer, int size, char *data, bool mirrorToRam) {
    YDS_ERROR_DECLARE("CreateVertexBuffer");

    if (newBuffer == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newBuffer = nullptr;
    
    D3D10_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D10_USAGE_DEFAULT;
    bd.ByteWidth = size;
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    D3D10_SUBRESOURCE_DATA InitData, *pInitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = data;

    if (data) pInitData = &InitData;
    else pInitData = nullptr;

    ID3D10Buffer *buffer;
    HRESULT result = m_device->CreateBuffer(&bd, pInitData, &buffer);
    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotCreateGpuBuffer);
    }

    ysD3D10GPUBuffer *newD3D10Buffer = m_gpuBuffers.NewGeneric<ysD3D10GPUBuffer>();

    newD3D10Buffer->m_size = size;
    newD3D10Buffer->m_mirrorToRAM = mirrorToRam;
    newD3D10Buffer->m_bufferType = ysGPUBuffer::GPU_DATA_BUFFER;
    newD3D10Buffer->m_buffer = buffer;

    if (mirrorToRam) {
        newD3D10Buffer->m_RAMMirror = new char[size];
        if (data != nullptr) memcpy(newD3D10Buffer->m_RAMMirror, data, size);
    }

    *newBuffer = static_cast<ysGPUBuffer *>(newD3D10Buffer);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::CreateIndexBuffer(ysGPUBuffer **newBuffer, int size, char *data, bool mirrorToRam) {
    YDS_ERROR_DECLARE("CreateIndexBuffer");

    if (newBuffer == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newBuffer = nullptr;
    
    D3D10_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D10_USAGE_DEFAULT;
    bd.ByteWidth = size;
    bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    D3D10_SUBRESOURCE_DATA InitData, *pInitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = data;

    if (data) pInitData = &InitData;
    else pInitData = nullptr;

    ID3D10Buffer *buffer;
    HRESULT result = m_device->CreateBuffer(&bd, pInitData, &buffer);
    if (FAILED(result)) {
        *newBuffer = nullptr;
        return YDS_ERROR_RETURN(ysError::CouldNotCreateGpuBuffer);
    }

    ysD3D10GPUBuffer *newD3D10Buffer = m_gpuBuffers.NewGeneric<ysD3D10GPUBuffer>();

    newD3D10Buffer->m_size = size;
    newD3D10Buffer->m_mirrorToRAM = mirrorToRam;
    newD3D10Buffer->m_bufferType = ysGPUBuffer::GPU_INDEX_BUFFER;
    newD3D10Buffer->m_buffer = buffer;

    if (mirrorToRam) {
        newD3D10Buffer->m_RAMMirror = new char[size];
        if (data != nullptr) memcpy(newD3D10Buffer->m_buffer, data, size);
    }

    *newBuffer = static_cast<ysGPUBuffer *>(newD3D10Buffer);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::CreateConstantBuffer(ysGPUBuffer **newBuffer, int size, char *data, bool mirrorToRam) {
    YDS_ERROR_DECLARE("CreateConstantBuffer");

    if (newBuffer == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newBuffer = nullptr;
    
    D3D10_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D10_USAGE_DEFAULT;
    bd.ByteWidth = size;
    bd.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    D3D10_SUBRESOURCE_DATA InitData, *pInitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = data;

    if (data != nullptr) pInitData = &InitData;
    else pInitData = nullptr;

    HRESULT result;
    ID3D10Buffer *buffer;
    result = m_device->CreateBuffer(&bd, pInitData, &buffer);
    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotCreateGpuBuffer);
    }

    ysD3D10GPUBuffer *newD3D10Buffer = m_gpuBuffers.NewGeneric<ysD3D10GPUBuffer>();

    newD3D10Buffer->m_size = size;
    newD3D10Buffer->m_mirrorToRAM = mirrorToRam;
    newD3D10Buffer->m_bufferType = ysGPUBuffer::GPU_CONSTANT_BUFFER;
    newD3D10Buffer->m_buffer = buffer;

    if (mirrorToRam) {
        newD3D10Buffer->m_RAMMirror = new char[size];
        if (data != nullptr) memcpy(newD3D10Buffer->m_buffer, data, size);
    }

    *newBuffer = static_cast<ysGPUBuffer *>(newD3D10Buffer);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::UseVertexBuffer(ysGPUBuffer *buffer, int stride, int offset) {
    YDS_ERROR_DECLARE("UseVertexBuffer");

    if (!CheckCompatibility(buffer)) return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);

    if (buffer) {
        UINT uoffset = (UINT)offset;
        UINT ustride = (UINT)stride;

        ysD3D10GPUBuffer *d3d10Buffer = static_cast<ysD3D10GPUBuffer *>(buffer);

        if (d3d10Buffer->m_bufferType == ysGPUBuffer::GPU_DATA_BUFFER && (buffer != m_activeVertexBuffer || stride != d3d10Buffer->m_currentStride)) {
            GetDevice()->IASetVertexBuffers(0, 1, &d3d10Buffer->m_buffer, &ustride, &uoffset);
        }
    }
    else {
        GetDevice()->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
    }

    YDS_NESTED_ERROR_CALL( ysDevice::UseVertexBuffer(buffer, stride, offset) );

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::UseIndexBuffer(ysGPUBuffer *buffer, int offset) {
    YDS_ERROR_DECLARE("UseIndexBuffer");

    if (!CheckCompatibility(buffer)) return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);

    if (buffer) {
        UINT uoffset = (UINT)offset;

        ysD3D10GPUBuffer *d3d10Buffer = static_cast<ysD3D10GPUBuffer *>(buffer);

        if (d3d10Buffer->m_bufferType == ysGPUBuffer::GPU_INDEX_BUFFER && buffer != m_activeIndexBuffer) {
            GetDevice()->IASetIndexBuffer(d3d10Buffer->m_buffer, DXGI_FORMAT_R16_UINT, uoffset);
        }
    }
    else {
        GetDevice()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
    }

    YDS_NESTED_ERROR_CALL( ysDevice::UseIndexBuffer(buffer, offset) );

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::UseConstantBuffer(ysGPUBuffer *buffer, int slot) {
    YDS_ERROR_DECLARE("UseConstantBuffer");

    if (!CheckCompatibility(buffer)) return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);

    if (buffer) {
        ysD3D10GPUBuffer *d3d10Buffer = static_cast<ysD3D10GPUBuffer *>(buffer);

        GetDevice()->VSSetConstantBuffers(slot, 1, &d3d10Buffer->m_buffer);
        GetDevice()->PSSetConstantBuffers(slot, 1, &d3d10Buffer->m_buffer);
    }
    else {
        GetDevice()->VSSetConstantBuffers(slot, 0, nullptr);
        GetDevice()->PSSetConstantBuffers(slot, 0, nullptr);
    }

    YDS_NESTED_ERROR_CALL( ysDevice::UseConstantBuffer(buffer, slot) );

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::EditBufferDataRange(ysGPUBuffer *buffer, char *data, int size, int offset) {
    YDS_ERROR_DECLARE("EditBufferDataRange");

    if (!CheckCompatibility(buffer))            return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    if (buffer == nullptr || data == nullptr)            return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if ((size + offset) > buffer->GetSize())    return YDS_ERROR_RETURN(ysError::OutOfBounds);
    if (size < 0 || offset < 0)                    return YDS_ERROR_RETURN(ysError::OutOfBounds);

    ysD3D10GPUBuffer *d3d10Buffer = static_cast<ysD3D10GPUBuffer *>(buffer);

    D3D10_BOX box;
    ZeroMemory(&box, sizeof(box));
    box.left = offset;
    box.right = offset + size;
    box.bottom = 1;
    box.back = 1;

    GetDevice()->UpdateSubresource(d3d10Buffer->m_buffer, 0, &box, data, buffer->GetSize(), 0);

    YDS_NESTED_ERROR_CALL( ysDevice::EditBufferDataRange(buffer, data, size, offset) );

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::EditBufferData(ysGPUBuffer *buffer, char *data) {
    YDS_ERROR_DECLARE("EditBufferData");

    if (!CheckCompatibility(buffer))        return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    if (buffer == nullptr || data == nullptr)        return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ysD3D10GPUBuffer *d3d10Buffer = static_cast<ysD3D10GPUBuffer *>(buffer);

    GetDevice()->UpdateSubresource(d3d10Buffer->m_buffer, 0, nullptr, data, 0, 0);

    YDS_NESTED_ERROR_CALL( ysDevice::EditBufferData(buffer, data) );

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::DestroyGPUBuffer(ysGPUBuffer *&buffer) {
    YDS_ERROR_DECLARE("DestroyGPUBuffer");

    if (!CheckCompatibility(buffer))    return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    if (buffer == nullptr)                    return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ysD3D10GPUBuffer *d3d10Buffer = static_cast<ysD3D10GPUBuffer *>(buffer);

    switch(buffer->GetType()) {
    case ysGPUBuffer::GPU_CONSTANT_BUFFER:
        {
            if (buffer == GetActiveBuffer(ysGPUBuffer::GPU_CONSTANT_BUFFER)) {
                YDS_NESTED_ERROR_CALL( UseConstantBuffer(nullptr, 0) );
            }

            break;
        }
    case ysGPUBuffer::GPU_DATA_BUFFER:
        {
            if (buffer == GetActiveBuffer(ysGPUBuffer::GPU_DATA_BUFFER)) {
                YDS_NESTED_ERROR_CALL( UseVertexBuffer(nullptr, 0, 0) );
            }

            break;
        }
    case ysGPUBuffer::GPU_INDEX_BUFFER:
        {
            if (buffer == GetActiveBuffer(ysGPUBuffer::GPU_INDEX_BUFFER)) {
                YDS_NESTED_ERROR_CALL( UseIndexBuffer(nullptr, 0) );
            }

            break;
        }
    }

    d3d10Buffer->m_buffer->Release();

    YDS_NESTED_ERROR_CALL( ysDevice::DestroyGPUBuffer(buffer) );

    return YDS_ERROR_RETURN(ysError::None);
}

// Shaders

ysError ysD3D10Device::CreateVertexShader(ysShader **newShader, const char *shaderFilename, const char *shaderName) {
    YDS_ERROR_DECLARE("CreateVertexShader");

    if (newShader == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newShader = nullptr;
    
    if (shaderFilename == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (shaderName == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ID3D10VertexShader *vertexShader;
    ID3D10Blob *error;
    ID3D10Blob *shaderBlob;
    char errorBuffer[2048];

    HRESULT result;

    result = D3DX10CompileFromFile(shaderFilename, nullptr, nullptr, shaderName, "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &shaderBlob, &error, nullptr);
    
    if (FAILED(result)) {
        return YDS_ERROR_RETURN_MSG(ysError::VertexShaderCompilationError, errorBuffer);
    }

    result = m_device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &vertexShader);
    if (FAILED(result)) {
        shaderBlob->Release();

        return YDS_ERROR_RETURN(ysError::CouldNotCreateShader);
    }

    ysD3D10Shader *newD3D10Shader = m_shaders.NewGeneric<ysD3D10Shader>();
    newD3D10Shader->m_vertexShader = vertexShader;
    newD3D10Shader->m_shaderBlob = shaderBlob;

    strcpy_s(newD3D10Shader->m_filename, 256, shaderFilename);
    strcpy_s(newD3D10Shader->m_shaderName, 64, shaderName);
    newD3D10Shader->m_shaderType = ysShader::ShaderType::Vertex;

    *newShader = static_cast<ysShader *>(newD3D10Shader);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::CreatePixelShader(ysShader **newShader, const char *shaderFilename, const char *shaderName) {
    YDS_ERROR_DECLARE("CreatePixelShader");

    if (newShader == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newShader = nullptr;

    if (shaderFilename == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (shaderName == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ID3D10PixelShader *pixelShader;
    ID3D10Blob *error;
    ID3D10Blob *shaderBlob;
    char errorBuffer[2048];

    HRESULT result;
    result = D3DX10CompileFromFile(shaderFilename, nullptr, nullptr, shaderName, "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &shaderBlob, &error, nullptr);

    if (FAILED(result)) {
        return YDS_ERROR_RETURN_MSG(ysError::FragmentShaderCompilationError, errorBuffer);
    }

    result = m_device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &pixelShader);
    if (FAILED(result)) {
        shaderBlob->Release();
        return YDS_ERROR_RETURN(ysError::CouldNotCreateShader);
    }

    ysD3D10Shader *newD3D10Shader = m_shaders.NewGeneric<ysD3D10Shader>();
    newD3D10Shader->m_shaderBlob = shaderBlob;
    newD3D10Shader->m_pixelShader = pixelShader;

    strcpy_s(newD3D10Shader->m_filename, 256, shaderFilename);
    strcpy_s(newD3D10Shader->m_shaderName, 64, shaderName);
    newD3D10Shader->m_shaderType = ysShader::ShaderType::Pixel;

    *newShader = static_cast<ysShader *>(newD3D10Shader);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::DestroyShader(ysShader *&shader) {
    YDS_ERROR_DECLARE("DestroyShader");

    if (!CheckCompatibility(shader)) return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    if (shader == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ysD3D10Shader *d3d10Shader = static_cast<ysD3D10Shader *>(shader);
    bool active = (m_activeShaderProgram != nullptr) && (m_activeShaderProgram->GetShader(shader->GetShaderType()) == shader);

    d3d10Shader->m_shaderBlob->Release();
    
    switch(shader->GetShaderType()) {
    case ysShader::ShaderType::Vertex:
        {
            if (active) GetDevice()->VSSetShader(nullptr);
            d3d10Shader->m_vertexShader->Release();
            break;
        }
    case ysShader::ShaderType::Pixel:
        {
            if (active) GetDevice()->PSSetShader(nullptr);
            d3d10Shader->m_pixelShader->Release();
            break;
        }
    }

    YDS_NESTED_ERROR_CALL( ysDevice::DestroyShader(shader) );

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::DestroyShaderProgram(ysShaderProgram *&program, bool destroyShaders) {
    YDS_ERROR_DECLARE("DestroyShaderProgram");

    YDS_NESTED_ERROR_CALL( ysDevice::DestroyShaderProgram(program, destroyShaders) );

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::CreateShaderProgram(ysShaderProgram **newProgram) {
    YDS_ERROR_DECLARE("CreateShaderProgram");

    if (newProgram == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newProgram = nullptr;

    ysD3D10ShaderProgram *newD3D10Program = m_shaderPrograms.NewGeneric<ysD3D10ShaderProgram>();
    *newProgram = static_cast<ysShaderProgram *>(newD3D10Program);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::AttachShader(ysShaderProgram *program, ysShader *shader) {
    YDS_ERROR_DECLARE("AttachShader");

    YDS_NESTED_ERROR_CALL( ysDevice::AttachShader(program, shader) );

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::LinkProgram(ysShaderProgram *program) {
    YDS_ERROR_DECLARE("LinkProgram");

    YDS_NESTED_ERROR_CALL( ysDevice::LinkProgram(program) );

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::UseShaderProgram(ysShaderProgram *program) {
    YDS_ERROR_DECLARE("UseShaderProgram");

    if (!CheckCompatibility(program)) return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);

    ysD3D10ShaderProgram *d3d10Program = static_cast<ysD3D10ShaderProgram *>(program);
    ysD3D10ShaderProgram *currentProgram = static_cast<ysD3D10ShaderProgram *>(m_activeShaderProgram);
    ysD3D10Shader *vertexShader = (program) ? d3d10Program->GetShader(ysShader::ShaderType::Vertex) : nullptr;
    ysD3D10Shader *fragmentShader = (program) ? d3d10Program->GetShader(ysShader::ShaderType::Pixel) : nullptr;

    if (d3d10Program == currentProgram) return YDS_ERROR_RETURN(ysError::None);

    ysD3D10Shader *currentVertexShader = (currentProgram) ? currentProgram->GetShader(ysShader::ShaderType::Vertex) : nullptr;
    ysD3D10Shader *currentPixelShader = (currentProgram) ? currentProgram->GetShader(ysShader::ShaderType::Pixel) : nullptr;

    if (vertexShader != currentVertexShader) {
        GetDevice()->VSSetShader((vertexShader) ? vertexShader->m_vertexShader : nullptr);
    }

    if (fragmentShader != currentPixelShader) {
        GetDevice()->PSSetShader((fragmentShader) ? fragmentShader->m_pixelShader : nullptr);
    }

    YDS_NESTED_ERROR_CALL( ysDevice::UseShaderProgram(program) );

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::CreateInputLayout(ysInputLayout **newInputLayout, ysShader *shader, const ysRenderGeometryFormat *format) {
    YDS_ERROR_DECLARE("CreateInputLayout");

    if (newInputLayout == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newInputLayout = nullptr;

    if (shader == nullptr || format == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (!CheckCompatibility(shader)) return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);

    ysD3D10Shader *d3d10Shader = static_cast<ysD3D10Shader *>(shader);

    int nChannels = format->GetChannelCount();
    D3D10_INPUT_ELEMENT_DESC descArray[64];

    for(int i = 0; i < nChannels; i++) {
        const ysRenderGeometryChannel *channel = format->GetChannel(i);

        descArray[i].SemanticName = channel->GetName();
        descArray[i].SemanticIndex = 0;
        descArray[i].Format = ConvertInputLayoutFormat(channel->GetFormat());
        descArray[i].InputSlot = 0;
        descArray[i].AlignedByteOffset = channel->GetOffset();
        descArray[i].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
        descArray[i].InstanceDataStepRate = 0;
    }

    HRESULT result;
    ID3D10InputLayout *layout;

    result = m_device->CreateInputLayout(
        descArray, 
        nChannels, 
        d3d10Shader->m_shaderBlob->GetBufferPointer(), 
        d3d10Shader->m_shaderBlob->GetBufferSize(),
        &layout);

    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::IncompatibleInputFormat);
    }

    ysD3D10InputLayout *newD3D10Layout = m_inputLayouts.NewGeneric<ysD3D10InputLayout>();
    newD3D10Layout->m_layout = layout;
    
    *newInputLayout = static_cast<ysInputLayout *>(newD3D10Layout);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::UseInputLayout(ysInputLayout *layout) {
    YDS_ERROR_DECLARE("UseInputLayout");

    if (!CheckCompatibility(layout)) return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);

    ysD3D10InputLayout *d3d10Layout = static_cast<ysD3D10InputLayout *>(layout);

    if (layout != nullptr) {
        if (layout != m_activeInputLayout) {
            GetDevice()->IASetInputLayout(d3d10Layout->m_layout);
        }
    }
    else {
        GetDevice()->IASetInputLayout(nullptr);
    }

    YDS_NESTED_ERROR_CALL( ysDevice::UseInputLayout(layout) );

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::DestroyInputLayout(ysInputLayout *&layout) {
    YDS_ERROR_DECLARE("DestroyInputLayout");

    if (layout == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (!CheckCompatibility(layout)) return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);

    if (layout == m_activeInputLayout) {
        UseInputLayout(nullptr);
    }

    ysD3D10InputLayout *d3d10Layout = static_cast<ysD3D10InputLayout *>(layout);
    d3d10Layout->m_layout->Release();

    YDS_NESTED_ERROR_CALL( ysDevice::DestroyInputLayout(layout) );

    return YDS_ERROR_RETURN(ysError::None);
}

// Textures
ysError ysD3D10Device::CreateTexture(ysTexture **newTexture, const char *fname) {
    YDS_ERROR_DECLARE("CreateTexture");

    if (newTexture == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newTexture = nullptr;

    if (fname == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ID3D10Texture2D *newD3DTexture = nullptr;
    ID3D10ShaderResourceView *resourceView;

    D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
    D3D10_TEXTURE2D_DESC desc;
    HRESULT result;
    
    result = D3DX10CreateTextureFromFile(m_device, fname, nullptr, nullptr, (ID3D10Resource **)&newD3DTexture, nullptr);
    if (FAILED(result)) {
        return YDS_ERROR_RETURN_MSG(ysError::CouldNotOpenTexture, fname);
    }

    newD3DTexture->GetDesc(&desc);
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;

    result = m_device->CreateShaderResourceView(newD3DTexture, &srvDesc, &resourceView);
    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotMakeShaderResourceView);
    }

    ysD3D10Texture *newD3D10Texture = m_textures.NewGeneric<ysD3D10Texture>();
    newD3D10Texture->m_resourceView = resourceView;
    newD3D10Texture->m_width = desc.Width;
    newD3D10Texture->m_height = desc.Height;
    *newTexture = newD3D10Texture;

    newD3DTexture->Release();

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::CreateTexture(ysTexture **texture, int width, int height, const unsigned char *buffer) {
    YDS_ERROR_DECLARE("CreateTexture");

    *texture = nullptr;

    return YDS_ERROR_RETURN(ysError::NotImplemented);
}

ysError ysD3D10Device::UpdateTexture(ysTexture *texture, const unsigned char *buffer) {
    YDS_ERROR_DECLARE("UpdateTexture");
    return YDS_ERROR_RETURN(ysError::NotImplemented);
}

ysError ysD3D10Device::CreateAlphaTexture(ysTexture **texture, int width, int height, const unsigned char *buffer) {
    YDS_ERROR_DECLARE("CreateTexture");

    *texture = nullptr;

    return YDS_ERROR_RETURN(ysError::NotImplemented);
}

ysError ysD3D10Device::UseTexture(ysTexture *texture, int slot) {
    YDS_ERROR_DECLARE("UseTexture");

    if (slot >= m_maxTextureSlots) return YDS_ERROR_RETURN(ysError::OutOfBounds);

    ysD3D10Texture *d3d10Texture = static_cast<ysD3D10Texture *>(texture);

    if (texture != m_activeTextures[slot].Texture) {
        ID3D10ShaderResourceView *nullView = nullptr;
        GetDevice()->PSSetShaderResources(slot, 1, (texture) ? &d3d10Texture->m_resourceView : &nullView);
    }

    YDS_NESTED_ERROR_CALL( ysDevice::UseTexture(texture, slot) );

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::UseRenderTargetAsTexture(ysRenderTarget *texture, int slot) {
    YDS_ERROR_DECLARE("UseTexture");

    if (slot >= m_maxTextureSlots) return YDS_ERROR_RETURN(ysError::OutOfBounds);

    ysD3D10RenderTarget *d3d10Texture = static_cast<ysD3D10RenderTarget *>(texture);

    if (texture != m_activeTextures[slot].RenderTarget) {
        ID3D10ShaderResourceView *nullView = nullptr;
        GetDevice()->PSSetShaderResources(slot, 1, (texture) ? &d3d10Texture->m_resourceView : &nullView);
    }

    YDS_NESTED_ERROR_CALL( ysDevice::UseRenderTargetAsTexture(texture, slot) );

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::DestroyTexture(ysTexture *&texture) {
    YDS_ERROR_DECLARE("DestroyTexture");

    if (texture == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ysD3D10Texture *d3d10Texture = static_cast<ysD3D10Texture *>(texture);

    for(int i = 0; i < m_maxTextureSlots; i++) {
        if (m_activeTextures[i].Texture == texture) {
            YDS_NESTED_ERROR_CALL(UseTexture(nullptr, i));
        }
    }

    if (d3d10Texture->m_renderTargetView) d3d10Texture->m_renderTargetView->Release();
    if (d3d10Texture->m_resourceView) d3d10Texture->m_resourceView->Release();

    YDS_NESTED_ERROR_CALL( ysDevice::DestroyTexture(texture) );

    return YDS_ERROR_RETURN(ysError::None);
}

void ysD3D10Device::Draw(int numFaces, int indexOffset, int vertexOffset) {
    GetDevice()->DrawIndexed(numFaces * 3, indexOffset, vertexOffset);
}

// Non-standard interface

void ysD3D10Device::GetDXGIDevice(IDXGIDevice **device) {
    HRESULT hr = m_device->QueryInterface(IID_IDXGIDevice, (void **)device);
    if (FAILED(hr)) (*device) = nullptr;
}

DXGI_FORMAT ysD3D10Device::ConvertInputLayoutFormat(ysRenderGeometryChannel::ChannelFormat format) {
    switch (format) {
    case ysRenderGeometryChannel::ChannelFormat::R32G32B32A32_FLOAT:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case ysRenderGeometryChannel::ChannelFormat::R32G32B32_FLOAT:
        return DXGI_FORMAT_R32G32B32_FLOAT;
    case ysRenderGeometryChannel::ChannelFormat::R32G32_FLOAT:
        return DXGI_FORMAT_R32G32_FLOAT;
    case ysRenderGeometryChannel::ChannelFormat::R32G32B32A32_UINT:
        return DXGI_FORMAT_R32G32B32A32_UINT;
    case ysRenderGeometryChannel::ChannelFormat::R32G32B32_UINT:
        return DXGI_FORMAT_R32G32B32_UINT;
    case ysRenderGeometryChannel::ChannelFormat::Undefined:
    default:
        return DXGI_FORMAT_UNKNOWN;
    }
}

ysError ysD3D10Device::CreateD3D10DepthBuffer(ID3D10DepthStencilView **newDepthStencil, int width, int height, int count, int quality) {
    YDS_ERROR_DECLARE("CreateD3D10DepthBuffer");

    if (newDepthStencil == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newDepthStencil = nullptr;

    HRESULT result;

    ID3D10Texture2D *depthBuffer;

    D3D10_TEXTURE2D_DESC descDepth;
    ZeroMemory( &descDepth, sizeof(descDepth) );
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = count;
    descDepth.SampleDesc.Quality = quality;
    descDepth.Usage = D3D10_USAGE_DEFAULT;
    descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    result = m_device->CreateTexture2D( &descDepth, nullptr, &depthBuffer );

    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotCreateDepthBuffer);
    }

    D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory( &descDSV, sizeof(descDSV) );
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    result = m_device->CreateDepthStencilView( depthBuffer, nullptr, newDepthStencil);

    if (FAILED(result)) {
        depthBuffer->Release();
        return YDS_ERROR_RETURN(ysError::CouldNotCreateDepthBuffer);
    }

    depthBuffer->Release();

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::DestroyD3D10DepthBuffer(ID3D10DepthStencilView *&depthStencil) {
    YDS_ERROR_DECLARE("DestroyD3D10DepthBuffer");

    if (depthStencil == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    depthStencil->Release();
    depthStencil = nullptr;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::CreateD3D10OnScreenRenderTarget(ysRenderTarget *newTarget, ysRenderingContext *context, bool depthBuffer) {
    YDS_ERROR_DECLARE("CreateD3D10OnScreenRenderTarget");
    
    ysD3D10Context *d3d10Context = static_cast<ysD3D10Context *>(context);

    ID3D10Texture2D *backBuffer;
    ID3D10RenderTargetView *newRenderTargetView;
    ID3D10DepthStencilView *newDepthStencil = nullptr;
    HRESULT result;

    result = d3d10Context->m_swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID *)(&backBuffer));
    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotGetBackBuffer);
    }
    
    result = m_device->CreateRenderTargetView(backBuffer, nullptr, &newRenderTargetView);
    backBuffer->Release();

    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotCreateRenderTarget);
    }

    // Create Depth Buffer
    if (depthBuffer) {
        ysError depthResult;
        depthResult = CreateD3D10DepthBuffer(&newDepthStencil, context->GetWindow()->GetGameWidth(), context->GetWindow()->GetGameHeight(), 1, 0);

        if (depthResult != ysError::None) {
            newRenderTargetView->Release();
            return YDS_ERROR_RETURN(depthResult);
        }
    }

    ysD3D10RenderTarget *newRenderTarget = static_cast<ysD3D10RenderTarget *>(newTarget);
    d3d10Context->m_attachedRenderTarget = newRenderTarget;

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

    newRenderTarget->m_renderTargetView = newRenderTargetView;
    newRenderTarget->m_depthStencil = newDepthStencil;
    newRenderTarget->m_resourceView = nullptr;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::CreateD3D10OffScreenRenderTarget(ysRenderTarget *target, int width, int height, ysRenderTarget::Format format, bool colorData, bool depthBuffer) {
    YDS_ERROR_DECLARE("CreateD3D10OffScreenRenderTarget");

    HRESULT result;

    ID3D10Texture2D *renderTarget;
    ID3D10RenderTargetView *newRenderTargetView = nullptr;
    ID3D10ShaderResourceView *shaderResourceView = nullptr;
    ID3D10DepthStencilView *newDepthStencil = nullptr;

    // Create the texture
    D3D10_TEXTURE2D_DESC descBuffer;
    ZeroMemory( &descBuffer, sizeof(descBuffer) );
    descBuffer.Width = width;
    descBuffer.Height = height;
    descBuffer.MipLevels = 1;
    descBuffer.ArraySize = 1;

    if (format == ysRenderTarget::Format::R32G32B32_FLOAT)
        descBuffer.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
    else if (format == ysRenderTarget::Format::R8G8B8A8_UNORM)
        descBuffer.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;

    descBuffer.SampleDesc.Count = 1;
    descBuffer.SampleDesc.Quality = 0;
    descBuffer.Usage = D3D10_USAGE_DEFAULT;
    descBuffer.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
    descBuffer.CPUAccessFlags = 0;
    descBuffer.MiscFlags = 0;
    result = m_device->CreateTexture2D( &descBuffer, nullptr, &renderTarget );

    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotCreateRenderTarget);
    }

    // Create the render target view
    D3D10_RENDER_TARGET_VIEW_DESC rtDesc;
    ZeroMemory(&rtDesc, sizeof(rtDesc));
    rtDesc.Format = descBuffer.Format;
    rtDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
    rtDesc.Texture2D.MipSlice = 0;

    result = m_device->CreateRenderTargetView(renderTarget, &rtDesc, &newRenderTargetView);

    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotCreateRenderTarget);
    }

    // Create the shader resource view
    D3D10_SHADER_RESOURCE_VIEW_DESC srDesc;
    ZeroMemory(&srDesc, sizeof(srDesc));
    srDesc.Format = descBuffer.Format;
    srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srDesc.Texture2D.MostDetailedMip = 0;
    srDesc.Texture2D.MipLevels = 1;

    result = m_device->CreateShaderResourceView(renderTarget, &srDesc, &shaderResourceView);

    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotCreateRenderTarget);

    }

    renderTarget->Release();

    // Create Depth Buffer
    if (depthBuffer) {
        ysError depthResult;
        depthResult = CreateD3D10DepthBuffer(&newDepthStencil, width, height, 1, 0);

        if (depthResult != ysError::None) {
            newRenderTargetView->Release();
            return YDS_ERROR_RETURN(depthResult);

        }
    }

    // Create the render target

    ysD3D10RenderTarget *newRenderTarget = static_cast<ysD3D10RenderTarget *>(target);

    newRenderTarget->m_type = ysRenderTarget::Type::OffScreen;
    newRenderTarget->m_posX = 0;
    newRenderTarget->m_posY = 0;
    newRenderTarget->m_width = width;
    newRenderTarget->m_height = height;
    newRenderTarget->m_physicalWidth = width;
    newRenderTarget->m_physicalHeight = height;
    newRenderTarget->m_format = ysRenderTarget::Format::R8G8B8A8_UNORM;
    newRenderTarget->m_hasDepthBuffer = depthBuffer;
    newRenderTarget->m_associatedContext = nullptr;

    newRenderTarget->m_renderTargetView = newRenderTargetView;
    newRenderTarget->m_depthStencil = newDepthStencil;
    newRenderTarget->m_resourceView = shaderResourceView;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D10Device::DestroyD3D10RenderTarget(ysRenderTarget *target) {
    YDS_ERROR_DECLARE("DestroyD3D10RenderTarget");

    ysD3D10RenderTarget *d3d10Target = static_cast<ysD3D10RenderTarget *>(target);
    if (d3d10Target->m_renderTargetView != nullptr) d3d10Target->m_renderTargetView->Release();
    if (d3d10Target->m_depthStencil != nullptr) d3d10Target->m_depthStencil->Release();
    if (d3d10Target->m_resourceView != nullptr) d3d10Target->m_resourceView->Release();

    d3d10Target->m_renderTargetView = nullptr;
    d3d10Target->m_depthStencil = nullptr;
    d3d10Target->m_resourceView = nullptr;

    return YDS_ERROR_RETURN(ysError::None);
}
