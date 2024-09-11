#include "../include/yds_d3d11_device.h"

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)

// TODO: -


#elif defined(_WIN64)

#define NOMINMAX
#include "../include/yds_d3d11_context.h"
#include "../include/yds_d3d11_gpu_buffer.h"
#include "../include/yds_d3d11_input_layout.h"
#include "../include/yds_d3d11_render_target.h"
#include "../include/yds_d3d11_shader.h"
#include "../include/yds_d3d11_shader_program.h"
#include "../include/yds_d3d11_texture.h"
#include "../include/yds_d3d11_utilities.h"

#include "../include/yds_windows_window.h"

#include "../include/yds_stb_image.h"
// TODO: Replace d3d
#include <d3d11_1.h>

#include <fstream>
#include <vector>

#pragma warning(push, 0)

#if YDS_SUPPORTS_SHADER_COMPILATION
#include <d3dcompiler.h>
#endif

#ifdef _DEBUG
#include <dxgi1_3.h>
#include <dxgidebug.h>
#include <initguid.h>

#include <wrl.h>

typedef HRESULT(WINAPI *DXGIGetDebugInterface_proc)(const IID &riid,
                                                    void **ppDebug);
#endif /* _DEBUG */

#pragma warning(pop)

#include <codecvt>
#include <locale>

#if !defined(_WIN64)
    #include "include/safe_lib.h"
#endif

ysD3D11Device::ysD3D11Device() : ysDevice(DeviceAPI::DirectX11) {
    m_device = nullptr;
    m_deviceContext = nullptr;
    m_DXGIFactory1 = nullptr;
    m_DXGIFactory2 = nullptr;

    m_multisampleCount = 0;
    m_multisampleQuality = 0;

    // TEMP
    m_rasterizerState = nullptr;

    m_depthStencilDisabledState = nullptr;
    m_depthStencilEnabledState = nullptr;
    m_samplerState = nullptr;

    m_blendState = nullptr;
}

ysD3D11Device::~ysD3D11Device() {}

#define ERROR_CODE(name)                                                       \
    case name:                                                                 \
        return L#name

std::wstring errorCodeToString(HRESULT result) {
    switch (result) {
        ERROR_CODE(D3D11_ERROR_FILE_NOT_FOUND);
        ERROR_CODE(D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS);
        ERROR_CODE(D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS);
        ERROR_CODE(D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD);

        ERROR_CODE(DXGI_ERROR_ACCESS_DENIED);
        ERROR_CODE(DXGI_ERROR_ACCESS_LOST);
        ERROR_CODE(DXGI_ERROR_ALREADY_EXISTS);
        ERROR_CODE(DXGI_ERROR_CANNOT_PROTECT_CONTENT);
        ERROR_CODE(DXGI_ERROR_DEVICE_HUNG);
        ERROR_CODE(DXGI_ERROR_DEVICE_REMOVED);
        ERROR_CODE(DXGI_ERROR_DEVICE_RESET);
        ERROR_CODE(DXGI_ERROR_DRIVER_INTERNAL_ERROR);
        ERROR_CODE(DXGI_ERROR_FRAME_STATISTICS_DISJOINT);
        ERROR_CODE(DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE);
        ERROR_CODE(DXGI_ERROR_INVALID_CALL);
        ERROR_CODE(DXGI_ERROR_MORE_DATA);
        ERROR_CODE(DXGI_ERROR_NAME_ALREADY_EXISTS);
        ERROR_CODE(DXGI_ERROR_NONEXCLUSIVE);
        ERROR_CODE(DXGI_ERROR_NOT_CURRENTLY_AVAILABLE);
        ERROR_CODE(DXGI_ERROR_NOT_FOUND);
        ERROR_CODE(DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED);
        ERROR_CODE(DXGI_ERROR_REMOTE_OUTOFMEMORY);
        ERROR_CODE(DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE);
        ERROR_CODE(DXGI_ERROR_SDK_COMPONENT_MISSING);
        ERROR_CODE(DXGI_ERROR_SESSION_DISCONNECTED);
        ERROR_CODE(DXGI_ERROR_UNSUPPORTED);
        ERROR_CODE(DXGI_ERROR_WAIT_TIMEOUT);
        ERROR_CODE(DXGI_ERROR_WAS_STILL_DRAWING);

        ERROR_CODE(E_FAIL);
        ERROR_CODE(E_INVALIDARG);
        ERROR_CODE(E_OUTOFMEMORY);
        ERROR_CODE(E_NOTIMPL);
        ERROR_CODE(S_FALSE);
        ERROR_CODE(S_OK);
        default: {
            std::wstringstream ss;
            ss << L"0x" << std::hex << result;
            return ss.str();
        }
    }
}

// A device is created using D3D11CreateDevice.
ysError ysD3D11Device::InitializeDevice() {
    YDS_ERROR_DECLARE("InitializeDevice");

    HRESULT result;
    D3D_FEATURE_LEVEL highestFeatureLevel;

    UINT deviceCreationFlags = 0;
#ifdef _DEBUG
    deviceCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif /* _DEBUG */

    const D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
    };

    result = D3D11CreateDevice(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceCreationFlags,
            featureLevels, sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL),
            D3D11_SDK_VERSION, &m_device, &highestFeatureLevel,
            &m_deviceContext);

    if (result == E_INVALIDARG) {
        // This is needed because D3D11CreateDevice will fail if on a
        // 11_0 device since D3D_FEATURE_LEVEL_11_1 won't be recongized
        result = D3D11CreateDevice(
                nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceCreationFlags,
                &featureLevels[1],
                (sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL)) - 1,
                D3D11_SDK_VERSION, &m_device, &highestFeatureLevel,
                &m_deviceContext);
    }

    if (FAILED(result)) {
        YDS_ERROR_LOG() << L"Could not connect to graphics device.\n\n";
        YDS_ERROR_LOG() << L"D3D11CreateDevice failed with error: ";

        const std::wstring errorString = errorCodeToString(result);
        YDS_ERROR_LOG() << errorString << L" {0x" << std::hex << result
                        << std::dec << L"}\n\n";

        IDXGIFactory1 *factory;
        HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1),
                                        (void **) (&factory));
        if (FAILED(hr)) {
            YDS_ERROR_LOG() << L"CreateDXGIFactory1() failed with error code: "
                            << errorCodeToString(hr) << L"\n\n";
        } else {
            UINT i = 0;
            std::vector<IDXGIAdapter *> adapters;
            for (IDXGIAdapter *adapter = nullptr;
                 factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND;) {
                adapters.push_back(adapter);
                ++i;
            }

            YDS_ERROR_LOG()
                    << adapters.size()
                    << L" graphics devices were found on this system:\n";
            for (size_t i = 0; i < adapters.size(); ++i) {
                IDXGIAdapter *adapter = adapters[i];
                YDS_ERROR_LOG() << L" (" << i + 1 << L") - ";
                DXGI_ADAPTER_DESC adapterDescription;
                hr = adapter->GetDesc(&adapterDescription);
                if (FAILED(hr)) {
                    YDS_ERROR_LOG()
                            << L"<COULD NOT RETRIEVE DEVICE INFORMATION>\n";
                } else {
                    YDS_ERROR_LOG() << adapterDescription.Description << L"\n";
                }
            }

            YDS_ERROR_LOG()
                    << L"\nD3D11 by default will use the FIRST device in this "
                       L"list. If this device is not the device you intend to "
                       L"use, you may need to "
                       L"review your system settings and make sure the correct "
                       L"graphics device is listed first. This issue has been "
                       L"reported occasionally with emulators like Proton.";

            factory->Release();
        }

        m_device = nullptr;
        return YDS_ERROR_RETURN(ysError::CouldNotCreateGraphicsDevice);
    }

    m_DXGIFactory1 = nullptr;
    {
        IDXGIDevice *dxgiDevice = nullptr;
        result = m_device->QueryInterface(
                __uuidof(IDXGIDevice), reinterpret_cast<void **>(&dxgiDevice));
        if (SUCCEEDED(result)) {
            IDXGIAdapter *adapter = nullptr;
            result = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(result)) {
                result = adapter->GetParent(
                        __uuidof(IDXGIFactory1),
                        reinterpret_cast<void **>(&m_DXGIFactory1));
                adapter->Release();
            }

            dxgiDevice->Release();
        }
    }

    m_DXGIFactory2 = nullptr;
    result = m_DXGIFactory1->QueryInterface(
            __uuidof(IDXGIFactory2),
            reinterpret_cast<void **>(&m_DXGIFactory2));

    if (FAILED(result)) {
        m_device->Release();
        m_device = nullptr;

        return YDS_ERROR_RETURN(ysError::CouldNotCreateGraphicsDevice);
    }

    // TEMP
    // Temporary location for this initialization
    GetImmediateContext()->IASetPrimitiveTopology(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    InitializeTextureSlots(D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::DestroyDevice() {
    YDS_ERROR_DECLARE("DestroyDevice");

    if (m_deviceContext != nullptr) {
        m_deviceContext->ClearState();
        m_deviceContext->Flush();
    }

    if (m_depthStencilDisabledState != nullptr)
        m_depthStencilDisabledState->Release();
    if (m_depthStencilEnabledState != nullptr)
        m_depthStencilEnabledState->Release();

    if (m_blendState != nullptr) { m_blendState->Release(); }
    if (m_samplerState != nullptr) { m_samplerState->Release(); }

    if (m_rasterizerState != nullptr) { m_rasterizerState->Release(); }

    if (m_DXGIFactory1 != nullptr) { m_DXGIFactory1->Release(); }
    if (m_DXGIFactory2 != nullptr) { m_DXGIFactory2->Release(); }
    if (m_deviceContext != nullptr) { m_deviceContext->Release(); }
    if (m_device != nullptr) { m_device->Release(); }

#ifdef _DEBUG
    Microsoft::WRL::ComPtr<IDXGIDebug> dxgiDebug;

    HMODULE debugModule = GetModuleHandle(TEXT("Dxgidebug.dll"));

    if (debugModule != NULL) {
        DXGIGetDebugInterface_proc proc =
                (DXGIGetDebugInterface_proc) GetProcAddress(
                        debugModule, "DXGIGetDebugInterface");
        const IID &pD = DXGI_DEBUG_ALL;
        HRESULT r = proc(IID_PPV_ARGS(dxgiDebug.GetAddressOf()));
        dxgiDebug.Get()->ReportLiveObjects(pD, DXGI_DEBUG_RLO_DETAIL);
    }
#endif /* _DEBUG */

    return YDS_ERROR_RETURN(ysError::None);
}

bool ysD3D11Device::CheckSupport() {
    // TEMP
    return true;
}

ysError ysD3D11Device::SetFaceCulling(bool faceCulling) { return ysError(); }

ysError ysD3D11Device::SetFaceCullingMode(CullMode cullMode) {
    return ysError();
}

ysError ysD3D11Device::CreateRenderingContext(ysRenderingContext **context,
                                              ysWindow *window) {
    YDS_ERROR_DECLARE("CreateRenderingContext");

    if (window->GetPlatform() != ysWindowSystem::Platform::Windows) {
        return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    } else if (context == nullptr) {
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    } else if (m_device == nullptr) {
        return YDS_ERROR_RETURN(ysError::NoDevice);
    }
    *context = nullptr;

    ysWindowsWindow *windowsWindow = static_cast<ysWindowsWindow *>(window);

    ysD3D11Context *newContext =
            m_renderingContexts.NewGeneric<ysD3D11Context>();
    newContext->m_targetWindow = window;

    const int width = std::max(1, window->GetGameWidth());
    const int height = std::max(1, window->GetGameHeight());

    // Get max MSAA quality
    UINT multisamplesPerPixel = 1;
    UINT maxQuality;
    DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
    for (UINT sampleCount = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; sampleCount > 0;
         sampleCount /= 2) {
        if (SUCCEEDED(m_device->CheckMultisampleQualityLevels(
                    format, sampleCount, &maxQuality)) &&
            maxQuality > 0) {
            multisamplesPerPixel = sampleCount;
            break;
        }
    }

    m_multisampleCount = (int) multisamplesPerPixel;
    m_multisampleQuality = (int) maxQuality - 1;

    if (m_DXGIFactory2 != nullptr) {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
        swapChainDesc.Width = width;
        swapChainDesc.Height = height;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
#if YDS_D3D11_USE_FLIP_MODEL
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
#else
        swapChainDesc.SampleDesc.Count = m_multisampleCount;
        swapChainDesc.SampleDesc.Quality = m_multisampleQuality;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
#endif
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2;

        newContext->m_swapChain1 = nullptr;
        HRESULT result = m_DXGIFactory2->CreateSwapChainForHwnd(
                m_device, windowsWindow->GetWindowHandle(), &swapChainDesc,
                nullptr, nullptr, &newContext->m_swapChain1);
        if (SUCCEEDED(result)) {
            result = newContext->m_swapChain1->QueryInterface(
                    __uuidof(IDXGISwapChain),
                    reinterpret_cast<void **>(&newContext->m_swapChain));
        } else {
            YDS_ERROR_LOG() << L"IDXGIFactory2::CreateSwapChainForHwnd() "
                               L"failed with error code: "
                            << errorCodeToString(result) << L"\n\n";

            m_renderingContexts.Delete(newContext->GetIndex());
            *context = nullptr;
            return YDS_ERROR_RETURN(ysError::CouldNotCreateSwapChain);
        }
    } else {
        DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
        ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
        swapChainDesc.BufferDesc.Width = width;
        swapChainDesc.BufferDesc.Height = height;
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
        swapChainDesc.BufferDesc.Format = format;
        swapChainDesc.BufferDesc.ScanlineOrdering =
                DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
#if YDS_D3D11_USE_FLIP_MODEL
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
#else
        swapChainDesc.SampleDesc.Count = m_multisampleCount;
        swapChainDesc.SampleDesc.Quality = m_multisampleQuality;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
#endif
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 1;
        swapChainDesc.OutputWindow = windowsWindow->GetWindowHandle();
        swapChainDesc.Windowed = TRUE;
        swapChainDesc.Flags = 0;

        HRESULT result = m_DXGIFactory1->CreateSwapChain(
                m_device, &swapChainDesc, &newContext->m_swapChain);
        if (FAILED(result)) {
            m_renderingContexts.Delete(newContext->GetIndex());
            *context = nullptr;

            YDS_ERROR_LOG() << L"IDXGIFactory1::CreateSwapChainForHwnd() "
                               L"failed with error code: "
                            << errorCodeToString(result) << L"\n\n";

            return YDS_ERROR_RETURN(ysError::CouldNotCreateSwapChain);
        }
    }

    m_DXGIFactory1->MakeWindowAssociation(windowsWindow->GetWindowHandle(),
                                          DXGI_MWA_NO_ALT_ENTER);

#if YDS_D3D11_USE_FLIP_MODEL
    newContext->m_msaaRenderTarget =
            m_renderTargets.NewGeneric<ysD3D11RenderTarget>();
    YDS_NESTED_ERROR_CALL(CreateD3D11OffScreenRenderTarget(
            newContext->m_msaaRenderTarget, width, height,
            ysRenderTarget::Format::R8G8B8A8_UNORM, m_multisampleCount, true,
            true));
#endif

    *context = static_cast<ysRenderingContext *>(newContext);

    // TEMP
    if (m_rasterizerState == nullptr) {
        D3D11_RASTERIZER_DESC rasterizerDescription;
        ZeroMemory(&rasterizerDescription, sizeof(D3D11_RASTERIZER_DESC));
        rasterizerDescription.FillMode = D3D11_FILL_SOLID;
        rasterizerDescription.CullMode = D3D11_CULL_BACK;
        rasterizerDescription.FrontCounterClockwise = TRUE;
        rasterizerDescription.DepthBias = FALSE;
        rasterizerDescription.DepthBiasClamp = 0;
        rasterizerDescription.SlopeScaledDepthBias = 0;
        rasterizerDescription.DepthClipEnable = TRUE;
        rasterizerDescription.ScissorEnable = FALSE;
        rasterizerDescription.MultisampleEnable = FALSE;
        rasterizerDescription.AntialiasedLineEnable = FALSE;

        m_device->CreateRasterizerState(&rasterizerDescription,
                                        &m_rasterizerState);
        GetImmediateContext()->RSSetState(m_rasterizerState);

        D3D11SetDebugName(m_rasterizerState, "RASTERIZER_STATE");

        // TEMPORARY ALPHA ENABLING
        D3D11_BLEND_DESC BlendState;
        ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
        BlendState.RenderTarget[0].BlendEnable = TRUE;
        BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
        BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        BlendState.RenderTarget[0].RenderTargetWriteMask =
                D3D11_COLOR_WRITE_ENABLE_ALL;
        m_device->CreateBlendState(&BlendState, &m_blendState);

        D3D11SetDebugName(m_blendState, "ALPHA_BLEND_STATE");

        float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
        UINT sampleMask = 0xffffffff;

        GetImmediateContext()->OMSetBlendState(m_blendState, blendFactor,
                                               sampleMask);

        // END TEMPORARY ALPHA BLENDING
    }

    // END TEMP

    // Create standard depth stencil states
    D3D11_DEPTH_STENCIL_DESC dsDesc;

    // Depth test parameters
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    // Stencil test parameters
    dsDesc.StencilEnable = true;
    dsDesc.StencilReadMask = 0xFF;
    dsDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    if (m_depthStencilEnabledState == nullptr) {
        // Create depth stencil state
        m_device->CreateDepthStencilState(&dsDesc, &m_depthStencilEnabledState);
    }

    if (m_depthStencilDisabledState == nullptr) {
        dsDesc.DepthEnable = false;
        m_device->CreateDepthStencilState(&dsDesc,
                                          &m_depthStencilDisabledState);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::UpdateRenderingContext(ysRenderingContext *context) {
    YDS_ERROR_DECLARE("UpdateRenderingContext");

    if (context == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (m_device == nullptr) return YDS_ERROR_RETURN(ysError::NoDevice);

    // Check the window
    if (!context->GetWindow()->IsVisible()) {
        return YDS_ERROR_RETURN(ysError::None);
    }

    ysD3D11Context *d3d11Context = static_cast<ysD3D11Context *>(context);

    const int width = context->GetWindow()->GetGameWidth();
    const int height = context->GetWindow()->GetGameHeight();

    ysD3D11RenderTarget *attachedTarget = static_cast<ysD3D11RenderTarget *>(
            context->GetAttachedRenderTarget());

    if (width == 0 || height == 0) {
        // Don't do anything in this case
        return YDS_ERROR_RETURN(ysError::None);
    }

    // Destroy render target first
    if (attachedTarget != nullptr) {
#if YDS_D3D11_USE_FLIP_MODEL
        YDS_NESTED_ERROR_CALL(
                DestroyD3D11RenderTarget(d3d11Context->m_msaaRenderTarget));
#endif
        YDS_NESTED_ERROR_CALL(DestroyD3D11RenderTarget(attachedTarget));
    }

    HRESULT result = d3d11Context->m_swapChain->ResizeBuffers(
            2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM,
            DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
    if (FAILED(result)) { return YDS_ERROR_RETURN(ysError::ApiError); }

    const int pwidth = context->GetWindow()->GetScreenWidth();
    const int pheight = context->GetWindow()->GetScreenHeight();

    if (context->GetAttachedRenderTarget() != nullptr) {
        YDS_NESTED_ERROR_CALL(
                ResizeRenderTarget(context->GetAttachedRenderTarget(), width,
                                   height, pwidth, pheight));
#if YDS_D3D11_USE_FLIP_MODEL
        YDS_NESTED_ERROR_CALL(
                ResizeRenderTarget(d3d11Context->m_msaaRenderTarget, width,
                                   height, pwidth, pheight));
#endif
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::DestroyRenderingContext(ysRenderingContext *&context) {
    YDS_ERROR_DECLARE("DestroyRenderingContext");

    if (context != nullptr) {
        ysD3D11Context *d3d11Context = static_cast<ysD3D11Context *>(context);
        if (d3d11Context->m_swapChain != nullptr) {
            d3d11Context->m_swapChain->Release();
        }

        if (d3d11Context->m_swapChain1 != nullptr) {
            d3d11Context->m_swapChain1->Release();
        }
    }

    YDS_NESTED_ERROR_CALL(ysDevice::DestroyRenderingContext(context));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::SetContextMode(ysRenderingContext *context,
                                      ysRenderingContext::ContextMode mode) {
    YDS_ERROR_DECLARE("SetContextMode");

    if (context == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (!CheckCompatibility(context))
        return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);

    ysD3D11Context *d3d11Context = static_cast<ysD3D11Context *>(context);
    ysWindow *window = context->GetWindow();
    ysMonitor *monitor = window->GetMonitor();

    HRESULT result;

    if (mode == ysRenderingContext::ContextMode::Fullscreen) {
        window->SetWindowStyle(ysWindow::WindowStyle::Fullscreen);
        result = d3d11Context->m_swapChain->SetFullscreenState(TRUE, nullptr);

        if (FAILED(result)) {
            return YDS_ERROR_RETURN(ysError::CouldNotEnterFullscreen);
        }
    }

    else if (mode == ysRenderingContext::ContextMode::Windowed) {
        window->SetWindowStyle(ysWindow::WindowStyle::Windowed);
        result = d3d11Context->m_swapChain->SetFullscreenState(FALSE, nullptr);

        if (FAILED(result)) {
            return YDS_ERROR_RETURN(ysError::CouldNotExitFullscreen);
        }
    }

    YDS_NESTED_ERROR_CALL(ysDevice::SetContextMode(context, mode));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::CreateOnScreenRenderTarget(ysRenderTarget **newTarget,
                                                  ysRenderingContext *context,
                                                  bool depthBuffer) {
    YDS_ERROR_DECLARE("CreateOnScreenRenderTarget");

    if (newTarget == nullptr) {
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    }

    *newTarget = nullptr;

    if (context == nullptr) {
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    }

    if (context->GetAttachedRenderTarget() != nullptr) {
        return YDS_ERROR_RETURN(ysError::ContextAlreadyHasRenderTarget);
    }

    ysD3D11RenderTarget *newRenderTarget =
            m_renderTargets.NewGeneric<ysD3D11RenderTarget>();

    const ysError result = CreateD3D11OnScreenRenderTarget(
            newRenderTarget, context, depthBuffer);
    if (result != ysError::None) {
        m_renderTargets.Delete(newRenderTarget->GetIndex());
        return YDS_ERROR_RETURN(result);
    }

    *newTarget = static_cast<ysRenderTarget *>(newRenderTarget);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::CreateOffScreenRenderTarget(
        ysRenderTarget **newTarget, int width, int height,
        ysRenderTarget::Format format, bool colorData, bool depthBuffer) {
    YDS_ERROR_DECLARE("CreateOffScreenRenderTarget");

    if (newTarget == nullptr)
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newTarget = nullptr;

    ysD3D11RenderTarget *d3d11Target =
            m_renderTargets.NewGeneric<ysD3D11RenderTarget>();

    ysError result = CreateD3D11OffScreenRenderTarget(
            d3d11Target, width, height, format, 1, colorData, depthBuffer);

    if (result != ysError::None) {
        m_renderTargets.Delete(d3d11Target->GetIndex());
        return YDS_ERROR_RETURN(result);
    }

    *newTarget = static_cast<ysRenderTarget *>(d3d11Target);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::CreateSubRenderTarget(ysRenderTarget **newTarget,
                                             ysRenderTarget *parent, int x,
                                             int y, int width, int height) {
    YDS_ERROR_DECLARE("CreateSubRenderTarget");

    if (newTarget == nullptr)
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (parent->GetType() == ysRenderTarget::Type::Subdivision)
        return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ysD3D11RenderTarget *newRenderTarget =
            m_renderTargets.NewGeneric<ysD3D11RenderTarget>();
    ysD3D11RenderTarget *d3d11Parent =
            static_cast<ysD3D11RenderTarget *>(parent);

    newRenderTarget->m_type = ysRenderTarget::Type::Subdivision;
    newRenderTarget->m_posX = x;
    newRenderTarget->m_posY = y;
    newRenderTarget->m_width = width;
    newRenderTarget->m_height = height;
    newRenderTarget->m_physicalWidth = width;
    newRenderTarget->m_physicalHeight = height;
    newRenderTarget->m_format = ysRenderTarget::Format::R8G8B8A8_UNORM;
    newRenderTarget->m_hasDepthBuffer = parent->HasDepthBuffer();
    newRenderTarget->m_associatedContext = parent->GetAssociatedContext();
    newRenderTarget->m_parent = parent;

    newRenderTarget->m_renderTargetView = nullptr;
    newRenderTarget->m_depthStencilView = nullptr;

    *newTarget = static_cast<ysRenderTarget *>(newRenderTarget);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::SetRenderTarget(ysRenderTarget *target, int slot) {
    YDS_ERROR_DECLARE("SetRenderTarget");

    ysD3D11RenderTarget *d3d11Target =
            static_cast<ysD3D11RenderTarget *>(target);
    ysD3D11RenderTarget *realTarget =
            (target != nullptr)
                    ? static_cast<ysD3D11RenderTarget *>(target->GetRoot())
                    : nullptr;

    if (target != nullptr) {
        if (target->IsDepthTestEnabled() && target->HasDepthBuffer()) {
            m_deviceContext->OMSetDepthStencilState(m_depthStencilEnabledState,
                                                    1);
        } else {
            m_deviceContext->OMSetDepthStencilState(m_depthStencilDisabledState,
                                                    1);
        }

        if (target->GetAssociatedContext() != nullptr) {
            m_activeContext = target->GetAssociatedContext();
        }

        D3D11_VIEWPORT vp;
        vp.Width = FLOAT(target->GetWidth());
        vp.Height = FLOAT(target->GetHeight());
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = FLOAT(target->GetPosX());
        vp.TopLeftY = FLOAT(target->GetPosY());
        GetImmediateContext()->RSSetViewports(1, &vp);
    }

    {
        ID3D11RenderTargetView *views[MaxRenderTargets];
        int colorTargets = 0;
        for (int i = 0; i < MaxRenderTargets; ++i) {
            ysD3D11RenderTarget *activeTarget =
                    static_cast<ysD3D11RenderTarget *>(m_activeRenderTarget[i]);

            views[colorTargets++] =
                    (i == slot) ? (realTarget != nullptr)
                                          ? realTarget->m_renderTargetView
                                          : nullptr
                    : (activeTarget != nullptr)
                            ? static_cast<ysD3D11RenderTarget *>(
                                      activeTarget->GetRoot())
                                      ->m_renderTargetView
                            : nullptr;
        }

        GetImmediateContext()->OMSetRenderTargets(
                colorTargets, views,
                (realTarget != nullptr) ? realTarget->m_depthStencilView
                                        : nullptr);
    }

    YDS_NESTED_ERROR_CALL(ysDevice::SetRenderTarget(target, slot));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::SetDepthTestEnabled(ysRenderTarget *target,
                                           bool enable) {
    YDS_ERROR_DECLARE("SetDepthTestEnabled");

    const bool previousState = target->IsDepthTestEnabled();
    YDS_NESTED_ERROR_CALL(ysDevice::SetDepthTestEnabled(target, enable));

    if (target == GetActiveRenderTarget() && previousState != enable) {
        SetRenderTarget(target);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::ReadRenderTarget(ysRenderTarget *src, uint8_t *target) {
    YDS_ERROR_DECLARE("ReadRenderTarget");

    ysD3D11RenderTarget *d3d11Target = static_cast<ysD3D11RenderTarget *>(src);

    D3D11_TEXTURE2D_DESC desc;
    desc.Width = src->GetWidth();
    desc.Height = src->GetHeight();
    desc.MipLevels = desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = 0;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    ID3D11Texture2D *resolveTexture = nullptr, *stagingTexture = nullptr;
    HRESULT result = m_device->CreateTexture2D(&desc, nullptr, &resolveTexture);
    if (FAILED(result) || resolveTexture == nullptr) {
        return YDS_ERROR_RETURN(ysError::ApiError);
    }

    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    result = m_device->CreateTexture2D(&desc, nullptr, &stagingTexture);
    if (FAILED(result) || stagingTexture == nullptr) {
        return YDS_ERROR_RETURN(ysError::ApiError);
    }

    ysD3D11Context *context = static_cast<ysD3D11Context *>(m_activeContext);
    m_deviceContext->ResolveSubresource(resolveTexture, 0,
                                        d3d11Target->m_texture, 0,
                                        DXGI_FORMAT_R8G8B8A8_UNORM);
    m_deviceContext->CopyResource(stagingTexture, resolveTexture);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    result = m_deviceContext->Map(stagingTexture, 0, D3D11_MAP_READ, 0,
                                  &mappedResource);

    for (int i = 0; i < src->GetHeight(); ++i) {
        memcpy(&target[i * src->GetWidth() * 4],
               &(((uint8_t *)
                          mappedResource.pData)[i * mappedResource.RowPitch]),
               sizeof(uint8_t) * src->GetWidth() * 4);
    }

    m_deviceContext->Unmap(stagingTexture, 0);

    stagingTexture->Release();
    resolveTexture->Release();

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::ResizeRenderTarget(ysRenderTarget *target, int width,
                                          int height, int pwidth, int pheight) {
    YDS_ERROR_DECLARE("ResizeRenderTarget");

    YDS_NESTED_ERROR_CALL(ysDevice::ResizeRenderTarget(target, width, height,
                                                       pwidth, pheight));

    ysD3D11RenderTarget *d3d11Target =
            static_cast<ysD3D11RenderTarget *>(target);

    ysRenderTarget *prevTargets[MaxRenderTargets];
    for (int i = 0; i < MaxRenderTargets; ++i) {
        prevTargets[i] = m_activeRenderTarget[i];
    }

    // Disable the target if it is active
    for (int i = 0; i < MaxRenderTargets; ++i) {
        if (target == prevTargets[i]) { SetRenderTarget(nullptr, i); }
    }

    if (target->GetType() == ysRenderTarget::Type::OnScreen) {
        YDS_NESTED_ERROR_CALL(CreateD3D11OnScreenRenderTarget(
                target, target->GetAssociatedContext(),
                target->HasDepthBuffer()));
    } else if (target->GetType() == ysRenderTarget::Type::OffScreen) {
        YDS_NESTED_ERROR_CALL(CreateD3D11OffScreenRenderTarget(
                target, width, height, target->GetFormat(), target->GetMsaa(),
                target->HasColorData(), target->HasDepthBuffer()));
    } else if (target->GetType() == ysRenderTarget::Type::Subdivision) {
        // Nothing needs to be done
    }

    // Re-enable the target if it was active
    for (int i = 0; i < MaxRenderTargets; ++i) {
        if (target == prevTargets[i]) { SetRenderTarget(target, i); }
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::DestroyRenderTarget(ysRenderTarget *&target) {
    YDS_ERROR_DECLARE("DestroyRenderTarget");

    if (target == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    for (int i = 0; i < MaxRenderTargets; ++i) {
        if (target == m_activeRenderTarget[i]) {
            YDS_NESTED_ERROR_CALL(SetRenderTarget(nullptr, i));
        }
    }

    YDS_NESTED_ERROR_CALL(DestroyD3D11RenderTarget(target));
    YDS_NESTED_ERROR_CALL(ysDevice::DestroyRenderTarget(target));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::ClearBuffers(const float *clearColor) {
    YDS_ERROR_DECLARE("ClearBuffers");

    if (GetDevice() == nullptr) { return YDS_ERROR_RETURN(ysError::NoDevice); }

    for (int i = 0; i < MaxRenderTargets; ++i) {
        if (m_activeRenderTarget[i] != nullptr) {
            ysD3D11RenderTarget *renderTarget =
                    static_cast<ysD3D11RenderTarget *>(m_activeRenderTarget[i]);
            ysD3D11RenderTarget *actualTarget =
                    static_cast<ysD3D11RenderTarget *>(renderTarget->GetRoot());

            if (actualTarget->HasColorData()) {
                GetImmediateContext()->ClearRenderTargetView(
                        actualTarget->m_renderTargetView, clearColor);
            }

            if (actualTarget->HasDepthBuffer()) {
                GetImmediateContext()->ClearDepthStencilView(
                        actualTarget->m_depthStencilView, D3D11_CLEAR_DEPTH,
                        1.0f, 0);
            }

            return YDS_ERROR_RETURN(ysError::None);
        }
    }

    return YDS_ERROR_RETURN(ysError::NoRenderTarget);
}

ysError ysD3D11Device::Present() {
    YDS_ERROR_DECLARE("Present");

    if (m_activeContext == nullptr) {
        return YDS_ERROR_RETURN(ysError::NoContext);
    } else if (m_activeRenderTarget[0]->GetType() ==
               ysRenderTarget::Type::Subdivision) {
        return YDS_ERROR_RETURN(ysError::InvalidOperation);
    } else if (!m_activeContext->GetWindow()->IsOpen()) {
        return YDS_ERROR_RETURN(ysError::InvalidOperation);
    }

    ysD3D11Context *context = static_cast<ysD3D11Context *>(m_activeContext);
    if (context->m_swapChain == nullptr) {
        return YDS_ERROR_RETURN(ysError::NoContext);
    }

#if YDS_D3D11_USE_FLIP_MODEL
    ysD3D11RenderTarget *renderTarget =
            static_cast<ysD3D11RenderTarget *>(context->m_attachedRenderTarget);
    GetImmediateContext()->ResolveSubresource(
            renderTarget->m_texture, 0, context->m_msaaRenderTarget->m_texture,
            0, DXGI_FORMAT_R8G8B8A8_UNORM);
#endif

    if (m_verticalSyncEnabled) {
        context->m_swapChain->Present(1, 0);
    } else {
        context->m_swapChain->Present(0, 0);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

// Vertex Buffers

ysError ysD3D11Device::CreateVertexBuffer(ysGPUBuffer **newBuffer, int size,
                                          char *data, bool mirrorToRam) {
    YDS_ERROR_DECLARE("CreateVertexBuffer");

    if (newBuffer == nullptr) {
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    }

    *newBuffer = nullptr;

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = size;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData, *pInitData = nullptr;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = data;

    if (data != nullptr) { pInitData = &InitData; }

    ID3D11Buffer *buffer;
    HRESULT result = m_device->CreateBuffer(&bd, pInitData, &buffer);

    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotCreateGpuBuffer);
    }

    D3D11SetDebugName(buffer, "VERTEX_BUFFER");

    ysD3D11GPUBuffer *newD3D11Buffer =
            m_gpuBuffers.NewGeneric<ysD3D11GPUBuffer>();

    newD3D11Buffer->m_size = size;
    newD3D11Buffer->m_mirrorToRAM = mirrorToRam;
    newD3D11Buffer->m_bufferType = ysGPUBuffer::GPU_DATA_BUFFER;
    newD3D11Buffer->m_buffer = buffer;

    if (mirrorToRam) {
        newD3D11Buffer->m_RAMMirror = new char[size];

        if (data != nullptr) {
            memcpy(newD3D11Buffer->m_RAMMirror, data, size);
        }
    }

    *newBuffer = static_cast<ysGPUBuffer *>(newD3D11Buffer);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::CreateIndexBuffer(ysGPUBuffer **newBuffer, int size,
                                         char *data, bool mirrorToRam) {
    YDS_ERROR_DECLARE("CreateIndexBuffer");

    if (newBuffer == nullptr) {
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    }

    *newBuffer = nullptr;

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = size;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData, *pInitData = nullptr;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = data;

    if (data != nullptr) { pInitData = &InitData; }

    ID3D11Buffer *buffer;
    HRESULT result = m_device->CreateBuffer(&bd, pInitData, &buffer);
    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotCreateGpuBuffer);
    }

    D3D11SetDebugName(buffer, "INDEX_BUFFER");

    ysD3D11GPUBuffer *newD3D11Buffer =
            m_gpuBuffers.NewGeneric<ysD3D11GPUBuffer>();

    newD3D11Buffer->m_size = size;
    newD3D11Buffer->m_mirrorToRAM = mirrorToRam;
    newD3D11Buffer->m_bufferType = ysGPUBuffer::GPU_INDEX_BUFFER;
    newD3D11Buffer->m_buffer = buffer;

    if (mirrorToRam) {
        newD3D11Buffer->m_RAMMirror = new char[size];

        if (data != nullptr) {
            memcpy(newD3D11Buffer->m_RAMMirror, data, size);
        }
    }

    *newBuffer = static_cast<ysGPUBuffer *>(newD3D11Buffer);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::CreateConstantBuffer(ysGPUBuffer **newBuffer, int size,
                                            char *data, bool mirrorToRam) {
    YDS_ERROR_DECLARE("CreateConstantBuffer");

    if (newBuffer == nullptr) {
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    }

    *newBuffer = nullptr;

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = size;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData, *pInitData = nullptr;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = data;

    if (data != nullptr) { pInitData = &InitData; }

    HRESULT result;
    ID3D11Buffer *buffer;
    result = m_device->CreateBuffer(&bd, pInitData, &buffer);
    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotCreateGpuBuffer);
    }

    D3D11SetDebugName(buffer, "CONSTANT_BUFFER");

    ysD3D11GPUBuffer *newD3D11Buffer =
            m_gpuBuffers.NewGeneric<ysD3D11GPUBuffer>();

    newD3D11Buffer->m_size = size;
    newD3D11Buffer->m_mirrorToRAM = mirrorToRam;
    newD3D11Buffer->m_bufferType = ysGPUBuffer::GPU_CONSTANT_BUFFER;
    newD3D11Buffer->m_buffer = buffer;

    *newBuffer = static_cast<ysGPUBuffer *>(newD3D11Buffer);

    if (mirrorToRam) {
        newD3D11Buffer->m_RAMMirror = new char[size];

        if (data != nullptr) {
            memcpy(newD3D11Buffer->m_RAMMirror, data, size);
        }
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::UseVertexBuffer(ysGPUBuffer *buffer, int stride,
                                       int offset) {
    YDS_ERROR_DECLARE("UseVertexBuffer");

    if (!CheckCompatibility(buffer)) {
        return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    }

    if (buffer != nullptr) {
        const UINT uoffset = UINT(offset);
        const UINT ustride = UINT(stride);

        ysD3D11GPUBuffer *d3d11Buffer = static_cast<ysD3D11GPUBuffer *>(buffer);
        if (d3d11Buffer->m_bufferType == ysGPUBuffer::GPU_DATA_BUFFER &&
            (buffer != m_activeVertexBuffer ||
             stride != d3d11Buffer->m_currentStride)) {
            GetImmediateContext()->IASetVertexBuffers(
                    0, 1, &d3d11Buffer->m_buffer, &ustride, &uoffset);
        }
    } else {
        GetImmediateContext()->IASetVertexBuffers(0, 0, nullptr, nullptr,
                                                  nullptr);
    }

    YDS_NESTED_ERROR_CALL(ysDevice::UseVertexBuffer(buffer, stride, offset));
    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::UseInstanceBuffer(ysGPUBuffer *buffer, int stride,
                                         int offset) {
    YDS_ERROR_DECLARE("UseInstanceBuffer");

    if (!CheckCompatibility(buffer)) {
        return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    }

    if (buffer != nullptr) {
        const UINT uoffset = UINT(offset);
        const UINT ustride = UINT(stride);

        ysD3D11GPUBuffer *d3d11Buffer = static_cast<ysD3D11GPUBuffer *>(buffer);
        if (d3d11Buffer->m_bufferType == ysGPUBuffer::GPU_DATA_BUFFER &&
            (buffer != m_activeInstanceBuffer ||
             stride != d3d11Buffer->m_currentStride)) {
            GetImmediateContext()->IASetVertexBuffers(
                    1, 1, &d3d11Buffer->m_buffer, &ustride, &uoffset);
        }
    } else if (m_activeInstanceBuffer != nullptr) {
        GetImmediateContext()->IASetVertexBuffers(1, 0, nullptr, nullptr,
                                                  nullptr);
    }

    YDS_NESTED_ERROR_CALL(ysDevice::UseInstanceBuffer(buffer, stride, offset));
    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::UseIndexBuffer(ysGPUBuffer *buffer, int offset) {
    YDS_ERROR_DECLARE("UseIndexBuffer");

    if (!CheckCompatibility(buffer))
        return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);

    if (buffer) {
        UINT uoffset = (UINT) offset;

        ysD3D11GPUBuffer *d3d11Buffer = static_cast<ysD3D11GPUBuffer *>(buffer);

        if (d3d11Buffer->m_bufferType == ysGPUBuffer::GPU_INDEX_BUFFER &&
            buffer != m_activeIndexBuffer) {
            GetImmediateContext()->IASetIndexBuffer(
                    d3d11Buffer->m_buffer, DXGI_FORMAT_R16_UINT, uoffset);
        }
    } else {
        GetImmediateContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN,
                                                0);
    }

    YDS_NESTED_ERROR_CALL(ysDevice::UseIndexBuffer(buffer, offset));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::UseConstantBuffer(ysGPUBuffer *buffer, int slot) {
    YDS_ERROR_DECLARE("UseConstantBuffer");

    if (!CheckCompatibility(buffer))
        return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);

    if (buffer) {
        ysD3D11GPUBuffer *d3d11Buffer = static_cast<ysD3D11GPUBuffer *>(buffer);

        GetImmediateContext()->VSSetConstantBuffers(slot, 1,
                                                    &d3d11Buffer->m_buffer);
        GetImmediateContext()->PSSetConstantBuffers(slot, 1,
                                                    &d3d11Buffer->m_buffer);
    } else {
        GetImmediateContext()->VSSetConstantBuffers(slot, 0, nullptr);
        GetImmediateContext()->PSSetConstantBuffers(slot, 0, nullptr);
    }

    YDS_NESTED_ERROR_CALL(ysDevice::UseConstantBuffer(buffer, slot));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::EditBufferDataRange(ysGPUBuffer *buffer, char *data,
                                           int size, int offset) {
    YDS_ERROR_DECLARE("EditBufferDataRange");

    if (!CheckCompatibility(buffer)) {
        return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    }

    if (buffer == nullptr || data == nullptr) {
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    }

    if ((size + offset) > buffer->GetSize()) {
        return YDS_ERROR_RETURN(ysError::OutOfBounds);
    }

    if (size < 0 || offset < 0) {
        return YDS_ERROR_RETURN(ysError::OutOfBounds);
    }

    if (size == 0) { return YDS_ERROR_RETURN(ysError::None); }

    ysD3D11GPUBuffer *d3d11Buffer = static_cast<ysD3D11GPUBuffer *>(buffer);

    D3D11_BOX box;
    ZeroMemory(&box, sizeof(box));
    box.left = offset;
    box.right = offset + size;
    box.bottom = 1;
    box.back = 1;
    GetImmediateContext()->UpdateSubresource(d3d11Buffer->m_buffer, 0, &box,
                                             data, buffer->GetSize(), 0);

    YDS_NESTED_ERROR_CALL(
            ysDevice::EditBufferDataRange(buffer, data, size, offset));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::EditBufferData(ysGPUBuffer *buffer, char *data) {
    YDS_ERROR_DECLARE("EditBufferData");

    if (!CheckCompatibility(buffer))
        return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    if (buffer == nullptr || data == nullptr)
        return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ysD3D11GPUBuffer *d3d11Buffer = static_cast<ysD3D11GPUBuffer *>(buffer);

    GetImmediateContext()->UpdateSubresource(d3d11Buffer->m_buffer, 0, nullptr,
                                             data, 0, 0);

    YDS_NESTED_ERROR_CALL(ysDevice::EditBufferData(buffer, data));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::DestroyGPUBuffer(ysGPUBuffer *&buffer) {
    YDS_ERROR_DECLARE("DestroyGPUBuffer");

    if (buffer == nullptr) { return YDS_ERROR_RETURN(ysError::None); }

    if (!CheckCompatibility(buffer))
        return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    if (buffer == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ysD3D11GPUBuffer *d3d11Buffer = static_cast<ysD3D11GPUBuffer *>(buffer);

    switch (buffer->GetType()) {
        case ysGPUBuffer::GPU_CONSTANT_BUFFER: {
            if (buffer == GetActiveBuffer(ysGPUBuffer::GPU_CONSTANT_BUFFER)) {
                YDS_NESTED_ERROR_CALL(UseConstantBuffer(nullptr, 0));
            }

            break;
        }
        case ysGPUBuffer::GPU_DATA_BUFFER: {
            if (buffer == GetActiveBuffer(ysGPUBuffer::GPU_DATA_BUFFER)) {
                YDS_NESTED_ERROR_CALL(UseVertexBuffer(nullptr, 0, 0));
            }

            break;
        }
        case ysGPUBuffer::GPU_INDEX_BUFFER: {
            if (buffer == GetActiveBuffer(ysGPUBuffer::GPU_INDEX_BUFFER)) {
                YDS_NESTED_ERROR_CALL(UseIndexBuffer(nullptr, 0));
            }

            break;
        }
    }

    d3d11Buffer->m_buffer->Release();

    YDS_NESTED_ERROR_CALL(ysDevice::DestroyGPUBuffer(buffer));

    return YDS_ERROR_RETURN(ysError::None);
}

// Shaders
ysError ysD3D11Device::CreateVertexShader(ysShader **newShader,
                                          const wchar_t *shaderFilename,
                                          const wchar_t *compiledFilename,
                                          const char *shaderName,
                                          bool compile) {
    YDS_ERROR_DECLARE("CreateVertexShader");

    if (newShader == nullptr)
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newShader = nullptr;

    if (shaderFilename == nullptr)
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (shaderName == nullptr)
        return YDS_ERROR_RETURN(ysError::InvalidParameter);

    HRESULT result;
    void *shaderByteCode = nullptr;
    SIZE_T shaderSize = 0;

#if YDS_SUPPORTS_SHADER_COMPILATION
    if (compile) {
        ID3D10Blob *error = nullptr;
        ID3D10Blob *shaderBlob = nullptr;

        result = D3DCompileFromFile(
                shaderFilename, nullptr, nullptr, shaderName, "vs_4_0",
                D3DCOMPILE_ENABLE_STRICTNESS, 0, &shaderBlob, &error);

        if (FAILED(result) || shaderBlob == nullptr) {
            if (error != nullptr) {
                return YDS_ERROR_RETURN_MSG(
                        ysError::VertexShaderCompilationError,
                        (char *) error->GetBufferPointer());
            } else {
                return YDS_ERROR_RETURN_MSG(
                        ysError::VertexShaderCompilationError, "No error blob");
            }
        }

        std::fstream output(compiledFilename, std::ios::out | std::ios::binary);
        if (!output.is_open()) {
            return YDS_ERROR_RETURN(ysError::VertexShaderCompilationError);
        }

        output.write((char *) shaderBlob->GetBufferPointer(),
                     shaderBlob->GetBufferSize());
        output.close();

        shaderByteCode = malloc(shaderBlob->GetBufferSize());
        if (shaderByteCode == nullptr) {
            return YDS_ERROR_RETURN(ysError::VertexShaderCompilationError);
        }

        shaderSize = shaderBlob->GetBufferSize();
        memcpy(shaderByteCode, shaderBlob->GetBufferPointer(), shaderSize);

        shaderBlob->Release();
    }
#endif

    if (shaderByteCode == nullptr) {
        std::fstream input(compiledFilename, std::ios::in | std::ios::binary);
        if (!input.is_open()) {
            return YDS_ERROR_RETURN(ysError::CouldNotLoadCompiledShader);
        }

        input.ignore(std::numeric_limits<std::streamsize>::max());
        std::streamsize length = input.gcount();

        input.clear();
        input.seekg(0, std::ios_base::beg);

        shaderSize = size_t(length);
        shaderByteCode = malloc(shaderSize);
        input.read(static_cast<char *>(shaderByteCode), shaderSize);
    }

    ID3D11VertexShader *vertexShader = nullptr;
    result = m_device->CreateVertexShader(shaderByteCode, shaderSize, nullptr,
                                          &vertexShader);
    if (FAILED(result)) {
        free(shaderByteCode);
        return YDS_ERROR_RETURN(ysError::CouldNotCreateShader);
    }

    ysD3D11Shader *newD3D11Shader = m_shaders.NewGeneric<ysD3D11Shader>();
    newD3D11Shader->m_vertexShader = vertexShader;
    newD3D11Shader->m_shaderByteCode = shaderByteCode;
    newD3D11Shader->m_shaderSize = shaderSize;

    wcscpy_s(newD3D11Shader->m_filename, 256, shaderFilename);
    strcpy_s(newD3D11Shader->m_shaderName, 64, shaderName);
    newD3D11Shader->m_shaderType = ysShader::ShaderType::Vertex;

#ifdef _DEBUG
    D3D11SetDebugName(newD3D11Shader->m_vertexShader, "VERTEX_SHADER");
#endif

    *newShader = static_cast<ysShader *>(newD3D11Shader);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::CreatePixelShader(ysShader **newShader,
                                         const wchar_t *shaderFilename,
                                         const wchar_t *compiledFilename,
                                         const char *shaderName, bool compile) {
    YDS_ERROR_DECLARE("CreatePixelShader");

    if (newShader == nullptr) {
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    }

    *newShader = nullptr;

    if (shaderFilename == nullptr) {
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    }

    if (shaderName == nullptr) {
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    }

    void *shaderByteCode = nullptr;
    SIZE_T shaderSize = 0U;

    HRESULT result;
#if YDS_SUPPORTS_SHADER_COMPILATION
    if (compile) {
        ID3D10Blob *error = nullptr;
        ID3D10Blob *shaderBlob = nullptr;

        result = D3DCompileFromFile(
                shaderFilename, nullptr, nullptr, shaderName, "ps_4_0",
                D3DCOMPILE_ENABLE_STRICTNESS, 0, &shaderBlob, &error);

        if (FAILED(result) || shaderBlob == nullptr) {
            if (error != nullptr) {
                return YDS_ERROR_RETURN_MSG(
                        ysError::FragmentShaderCompilationError,
                        (char *) error->GetBufferPointer());
            } else {
                return YDS_ERROR_RETURN_MSG(
                        ysError::FragmentShaderCompilationError,
                        "No error blob");
            }
        }

        std::fstream output(compiledFilename, std::ios::out | std::ios::binary);
        if (!output.is_open()) {
            return YDS_ERROR_RETURN(ysError::VertexShaderCompilationError);
        }

        output.write((char *) shaderBlob->GetBufferPointer(),
                     shaderBlob->GetBufferSize());
        output.close();

        shaderByteCode = malloc(shaderBlob->GetBufferSize());
        if (shaderByteCode == nullptr) {
            return YDS_ERROR_RETURN(ysError::VertexShaderCompilationError);
        }

        shaderSize = shaderBlob->GetBufferSize();
        memcpy(shaderByteCode, shaderBlob->GetBufferPointer(), shaderSize);

        shaderBlob->Release();
    }
#endif

    if (shaderByteCode == nullptr) {
        std::fstream input(compiledFilename, std::ios::in | std::ios::binary);
        if (!input.is_open()) {
            return YDS_ERROR_RETURN(ysError::CouldNotLoadCompiledShader);
        }

        input.ignore(std::numeric_limits<std::streamsize>::max());
        std::streamsize length = input.gcount();

        input.clear();
        input.seekg(0, std::ios_base::beg);

        shaderSize = size_t(length);
        shaderByteCode = malloc(shaderSize);
        input.read(static_cast<char *>(shaderByteCode), shaderSize);
    }

    ID3D11PixelShader *pixelShader = nullptr;
    result = m_device->CreatePixelShader(shaderByteCode, shaderSize, nullptr,
                                         &pixelShader);
    if (FAILED(result)) {
        free(shaderByteCode);
        return YDS_ERROR_RETURN(ysError::CouldNotCreateShader);
    }

    ysD3D11Shader *newD3D11Shader = m_shaders.NewGeneric<ysD3D11Shader>();
    newD3D11Shader->m_shaderByteCode = shaderByteCode;
    newD3D11Shader->m_shaderSize = shaderSize;
    newD3D11Shader->m_pixelShader = pixelShader;

    wcscpy_s(newD3D11Shader->m_filename, 256, shaderFilename);
    strcpy_s(newD3D11Shader->m_shaderName, 64, shaderName);
    newD3D11Shader->m_shaderType = ysShader::ShaderType::Pixel;

    *newShader = newD3D11Shader;

    D3D11SetDebugName(newD3D11Shader->m_pixelShader, "FRAGMENT_SHADER");

    // TEMP ----------------------------------------------------
    GetImmediateContext()->PSSetShader(pixelShader, 0, 0);

    if (m_samplerState == nullptr) {
        // Create a sampler state (testing purposes)
        D3D11_SAMPLER_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        desc.MinLOD = 0.0f;
        desc.MaxLOD = FLT_MAX;
        desc.MaxAnisotropy = 16;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

        result = m_device->CreateSamplerState(&desc, &m_samplerState);
        if (FAILED(result)) {
            return YDS_ERROR_RETURN(ysError::CouldNotCreateSamplerState);
        }

        GetImmediateContext()->PSSetSamplers(0, 1, &m_samplerState);
    }

    // END TEMP ----------------------------------------------------

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::DestroyShader(ysShader *&shader) {
    YDS_ERROR_DECLARE("DestroyShader");

    if (!CheckCompatibility(shader))
        return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    if (shader == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ysD3D11Shader *d3d11Shader = static_cast<ysD3D11Shader *>(shader);
    bool active = (m_activeShaderProgram != nullptr) &&
                  (m_activeShaderProgram->GetShader(shader->GetShaderType()) ==
                   shader);

    free(d3d11Shader->m_shaderByteCode);

    switch (shader->GetShaderType()) {
        case ysShader::ShaderType::Vertex: {
            if (active) GetImmediateContext()->VSSetShader(nullptr, nullptr, 0);
            d3d11Shader->m_vertexShader->Release();
            break;
        }
        case ysShader::ShaderType::Pixel: {
            if (active) GetImmediateContext()->PSSetShader(nullptr, nullptr, 0);
            d3d11Shader->m_pixelShader->Release();
            break;
        }
    }

    YDS_NESTED_ERROR_CALL(ysDevice::DestroyShader(shader));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::DestroyShaderProgram(ysShaderProgram *&program,
                                            bool destroyShaders) {
    YDS_ERROR_DECLARE("DestroyShaderProgram");

    YDS_NESTED_ERROR_CALL(
            ysDevice::DestroyShaderProgram(program, destroyShaders));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::CreateShaderProgram(ysShaderProgram **newProgram) {
    YDS_ERROR_DECLARE("CreateShaderProgram");

    if (newProgram == nullptr)
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newProgram = nullptr;

    ysD3D11ShaderProgram *newD3D11Program =
            m_shaderPrograms.NewGeneric<ysD3D11ShaderProgram>();
    *newProgram = static_cast<ysShaderProgram *>(newD3D11Program);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::AttachShader(ysShaderProgram *program,
                                    ysShader *shader) {
    YDS_ERROR_DECLARE("AttachShader");

    YDS_NESTED_ERROR_CALL(ysDevice::AttachShader(program, shader));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::LinkProgram(ysShaderProgram *program) {
    YDS_ERROR_DECLARE("LinkProgram");

    YDS_NESTED_ERROR_CALL(ysDevice::LinkProgram(program));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::UseShaderProgram(ysShaderProgram *program) {
    YDS_ERROR_DECLARE("UseShaderProgram");

    if (!CheckCompatibility(program)) {
        return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    }

    ysD3D11ShaderProgram *d3d11Program =
            static_cast<ysD3D11ShaderProgram *>(program);
    ysD3D11ShaderProgram *currentProgram =
            static_cast<ysD3D11ShaderProgram *>(m_activeShaderProgram);
    ysD3D11Shader *vertexShader =
            (d3d11Program)
                    ? d3d11Program->GetShader(ysShader::ShaderType::Vertex)
                    : nullptr;
    ysD3D11Shader *fragmentShader =
            (d3d11Program)
                    ? d3d11Program->GetShader(ysShader::ShaderType::Pixel)
                    : nullptr;

    if (d3d11Program == currentProgram) return YDS_ERROR_RETURN(ysError::None);

    ysD3D11Shader *currentVertexShader =
            (currentProgram)
                    ? currentProgram->GetShader(ysShader::ShaderType::Vertex)
                    : nullptr;
    ysD3D11Shader *currentPixelShader =
            (currentProgram)
                    ? currentProgram->GetShader(ysShader::ShaderType::Pixel)
                    : nullptr;

    if (vertexShader != currentVertexShader) {
        GetImmediateContext()->VSSetShader(
                (vertexShader) ? vertexShader->m_vertexShader : nullptr,
                nullptr, 0);
    }

    if (fragmentShader != currentPixelShader) {
        GetImmediateContext()->PSSetShader(
                (fragmentShader) ? fragmentShader->m_pixelShader : nullptr,
                nullptr, 0);
    }

    YDS_NESTED_ERROR_CALL(ysDevice::UseShaderProgram(program));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError
ysD3D11Device::CreateInputLayout(ysInputLayout **newInputLayout,
                                 ysShader *shader,
                                 const ysRenderGeometryFormat *format,
                                 const ysRenderGeometryFormat *instanceFormat) {
    YDS_ERROR_DECLARE("CreateInputLayout");

    if (newInputLayout == nullptr) {
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    }
    *newInputLayout = nullptr;

    if (shader == nullptr || format == nullptr) {
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    }

    if (!CheckCompatibility(shader)) {
        return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    }

    ysD3D11Shader *d3d11Shader = static_cast<ysD3D11Shader *>(shader);

    const int nChannels = format->GetChannelCount();
    D3D11_INPUT_ELEMENT_DESC descArray[64];
    for (int i = 0; i < nChannels; ++i) {
        const ysRenderGeometryChannel *channel = format->GetChannel(i);
        descArray[i].SemanticName = channel->GetName();
        descArray[i].SemanticIndex = 0;
        descArray[i].Format = ConvertInputLayoutFormat(channel->GetFormat());
        descArray[i].InputSlot = 0;
        descArray[i].AlignedByteOffset = channel->GetOffset();
        descArray[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        descArray[i].InstanceDataStepRate = 0;
    }

    const int instanceDataChannels =
            (instanceFormat != nullptr) ? instanceFormat->GetChannelCount() : 0;
    if (instanceFormat != nullptr) {
        for (int i = nChannels; i < nChannels + instanceDataChannels; ++i) {
            const ysRenderGeometryChannel *channel =
                    instanceFormat->GetChannel(i - nChannels);
            descArray[i].SemanticName = channel->GetName();
            descArray[i].SemanticIndex = 0;
            descArray[i].Format =
                    ConvertInputLayoutFormat(channel->GetFormat());
            descArray[i].InputSlot = 1;
            descArray[i].AlignedByteOffset = channel->GetOffset();
            descArray[i].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
            descArray[i].InstanceDataStepRate = 1;
        }
    }

    ID3D11InputLayout *layout = nullptr;
    const HRESULT result = m_device->CreateInputLayout(
            descArray, nChannels + instanceDataChannels,
            d3d11Shader->m_shaderByteCode, d3d11Shader->m_shaderSize, &layout);

    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::IncompatibleInputFormat);
    }

    ysD3D11InputLayout *newD3D11Layout =
            m_inputLayouts.NewGeneric<ysD3D11InputLayout>();
    newD3D11Layout->m_layout = layout;

    *newInputLayout = static_cast<ysInputLayout *>(newD3D11Layout);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::UseInputLayout(ysInputLayout *layout) {
    YDS_ERROR_DECLARE("UseInputLayout");

    if (!CheckCompatibility(layout)) {
        return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    }

    ysD3D11InputLayout *d3d11Layout = static_cast<ysD3D11InputLayout *>(layout);

    if (layout != nullptr) {
        if (layout != m_activeInputLayout) {
            GetImmediateContext()->IASetInputLayout(d3d11Layout->m_layout);
        }
    } else {
        GetImmediateContext()->IASetInputLayout(nullptr);
    }

    YDS_NESTED_ERROR_CALL(ysDevice::UseInputLayout(layout));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::DestroyInputLayout(ysInputLayout *&layout) {
    YDS_ERROR_DECLARE("DestroyInputLayout");

    if (layout == nullptr) {
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    }

    if (!CheckCompatibility(layout)) {
        return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    }

    if (layout == m_activeInputLayout) { UseInputLayout(nullptr); }

    ysD3D11InputLayout *d3d11Layout = static_cast<ysD3D11InputLayout *>(layout);
    d3d11Layout->m_layout->Release();

    YDS_NESTED_ERROR_CALL(ysDevice::DestroyInputLayout(layout));

    return YDS_ERROR_RETURN(ysError::None);
}

// MARK: - Textures
ysError ysD3D11Device::CreateTexture(ysTexture **newTexture,
                                     const wchar_t *fname) {
    YDS_ERROR_DECLARE("CreateTexture");

    if (newTexture == nullptr) {
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    }

    if (fname == nullptr) {
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    }

    *newTexture = nullptr;

    ID3D11Texture2D *newD3DTexture = nullptr;
    ID3D11ShaderResourceView *resourceView;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    int width, height, channels;
    stbi_uc *pixels = stbi_load(utf8_conv.to_bytes(std::wstring(fname)).c_str(),
                                &width, &height, &channels, 4);
    if (pixels == nullptr) {
        return YDS_ERROR_RETURN(ysError::CouldNotOpenTexture);
    }

    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = pixels;
    data.SysMemPitch = desc.Width * sizeof(unsigned char) * 4;
    data.SysMemSlicePitch = 0;

    HRESULT result = m_device->CreateTexture2D(&desc, &data, &newD3DTexture);
    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotOpenTexture);
    }

    stbi_image_free(pixels);

    newD3DTexture->GetDesc(&desc);
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;

    result = m_device->CreateShaderResourceView(newD3DTexture, &srvDesc,
                                                &resourceView);
    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotMakeShaderResourceView);
    }

    ysD3D11Texture *newD3D11Texture = m_textures.NewGeneric<ysD3D11Texture>();
    newD3D11Texture->m_resourceView = resourceView;
    newD3D11Texture->m_width = desc.Width;
    newD3D11Texture->m_height = desc.Height;
    *newTexture = newD3D11Texture;

    newD3DTexture->Release();

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::CreateTexture(ysTexture **texture, int width, int height,
                                     const unsigned char *buffer) {
    YDS_ERROR_DECLARE("CreateTexture");

    D3D11_TEXTURE2D_DESC desc;
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = reinterpret_cast<const void *>(buffer);
    data.SysMemPitch = width * sizeof(unsigned char) * 4;
    data.SysMemSlicePitch = 0;

    ID3D11Texture2D *newD3DTexture = nullptr;
    HRESULT result = m_device->CreateTexture2D(&desc, &data, &newD3DTexture);

    if (FAILED(result)) { return YDS_ERROR_RETURN(ysError::InvalidOperation); }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;

    ID3D11ShaderResourceView *resourceView = nullptr;
    result = m_device->CreateShaderResourceView(newD3DTexture, &srvDesc,
                                                &resourceView);
    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotMakeShaderResourceView);
    }

    ysD3D11Texture *newD3D11Texture = m_textures.NewGeneric<ysD3D11Texture>();
    newD3D11Texture->m_resourceView = resourceView;
    newD3D11Texture->m_width = desc.Width;
    newD3D11Texture->m_height = desc.Height;
    newD3D11Texture->m_textureResource = newD3DTexture;
    *texture = newD3D11Texture;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::CreateAlphaTexture(ysTexture **texture, int width,
                                          int height,
                                          const unsigned char *buffer) {
    YDS_ERROR_DECLARE("CreateAlphaTexture");

    D3D11_TEXTURE2D_DESC desc;
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = reinterpret_cast<const void *>(buffer);
    data.SysMemPitch = width * sizeof(unsigned char);
    data.SysMemSlicePitch = 0;

    HRESULT result;

    ID3D11Texture2D *newD3DTexture = nullptr;
    result = m_device->CreateTexture2D(&desc, &data, &newD3DTexture);

    if (FAILED(result)) { return YDS_ERROR_RETURN(ysError::InvalidOperation); }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;

    ID3D11ShaderResourceView *resourceView = nullptr;
    result = m_device->CreateShaderResourceView(newD3DTexture, &srvDesc,
                                                &resourceView);
    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotMakeShaderResourceView);
    }

    ysD3D11Texture *newD3D11Texture = m_textures.NewGeneric<ysD3D11Texture>();
    newD3D11Texture->m_resourceView = resourceView;
    newD3D11Texture->m_width = desc.Width;
    newD3D11Texture->m_height = desc.Height;
    newD3D11Texture->m_textureResource = newD3DTexture;
    *texture = newD3D11Texture;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::UpdateTexture(ysTexture *texture,
                                     const unsigned char *buffer) {
    YDS_ERROR_DECLARE("UpdateTexture");

    ID3D11DeviceContext *deviceContext = m_deviceContext;

    ysD3D11Texture *d3d11Texture = static_cast<ysD3D11Texture *>(texture);
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result =
            deviceContext->Map(d3d11Texture->m_textureResource, 0,
                               D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (SUCCEEDED(result)) {
        for (size_t i = 0; i < d3d11Texture->GetHeight(); ++i) {
            memcpy((unsigned char *) mappedResource.pData +
                           (size_t) mappedResource.RowPitch * i,
                   buffer + i * 4 * sizeof(unsigned char) * texture->GetWidth(),
                   4 * sizeof(unsigned char) * texture->GetWidth());
        }

        deviceContext->Unmap(d3d11Texture->m_textureResource, 0);

        return YDS_ERROR_RETURN(ysError::None);
    } else {
        return YDS_ERROR_RETURN(ysError::ApiError);
    }
}

ysError ysD3D11Device::UseTexture(ysTexture *texture, int slot) {
    YDS_ERROR_DECLARE("UseTexture");

    if (slot >= m_maxTextureSlots)
        return YDS_ERROR_RETURN(ysError::OutOfBounds);

    ysD3D11Texture *d3d11Texture = static_cast<ysD3D11Texture *>(texture);

    if (texture != m_activeTextures[slot].Texture) {
        ID3D11ShaderResourceView *nullView = nullptr;
        GetImmediateContext()->PSSetShaderResources(
                slot, 1, (texture) ? &d3d11Texture->m_resourceView : &nullView);
    }

    YDS_NESTED_ERROR_CALL(ysDevice::UseTexture(texture, slot));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::UseRenderTargetAsTexture(ysRenderTarget *renderTarget,
                                                int slot) {
    YDS_ERROR_DECLARE("UseRenderTargetAsTexture");

    if (slot >= m_maxTextureSlots)
        return YDS_ERROR_RETURN(ysError::OutOfBounds);

    ysD3D11RenderTarget *d3d11RenderTarget =
            static_cast<ysD3D11RenderTarget *>(renderTarget);

    if (renderTarget != m_activeTextures[slot].RenderTarget) {
        ID3D11ShaderResourceView *nullView = nullptr;
        GetImmediateContext()->PSSetShaderResources(
                slot, 1,
                (d3d11RenderTarget != nullptr)
                        ? &d3d11RenderTarget->m_resourceView
                        : &nullView);
    }

    YDS_NESTED_ERROR_CALL(
            ysDevice::UseRenderTargetAsTexture(renderTarget, slot));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::DestroyTexture(ysTexture *&texture) {
    YDS_ERROR_DECLARE("DestroyTexture");

    if (texture == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ysD3D11Texture *d3d11Texture = static_cast<ysD3D11Texture *>(texture);

    for (int i = 0; i < m_maxTextureSlots; i++) {
        if (m_activeTextures[i].Texture == texture) {
            YDS_NESTED_ERROR_CALL(UseTexture((ysTexture *) nullptr, i));
        }
    }

    if (d3d11Texture->m_renderTargetView != nullptr)
        d3d11Texture->m_renderTargetView->Release();
    if (d3d11Texture->m_resourceView != nullptr)
        d3d11Texture->m_resourceView->Release();
    if (d3d11Texture->m_textureResource != nullptr)
        d3d11Texture->m_textureResource->Release();

    YDS_NESTED_ERROR_CALL(ysDevice::DestroyTexture(texture));

    return YDS_ERROR_RETURN(ysError::None);
}

void ysD3D11Device::DrawInstanced(int numFaces, int indexOffset,
                                  int vertexOffset, int instanceCount,
                                  int instanceOffset) {
    GetImmediateContext()->IASetPrimitiveTopology(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    GetImmediateContext()->DrawIndexedInstanced(numFaces * 3, instanceCount,
                                                indexOffset, vertexOffset,
                                                instanceOffset);
}

void ysD3D11Device::Draw(int numFaces, int indexOffset, int vertexOffset) {
    GetImmediateContext()->IASetPrimitiveTopology(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    GetImmediateContext()->DrawIndexed(numFaces * 3, indexOffset, vertexOffset);
}

void ysD3D11Device::DrawLines(int numIndices, int indexOffset,
                              int vertexOffset) {
    GetImmediateContext()->IASetPrimitiveTopology(
            D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    GetImmediateContext()->DrawIndexed(numIndices, indexOffset, vertexOffset);
}

// MARK: - Non-standard interface
DXGI_FORMAT ysD3D11Device::ConvertInputLayoutFormat(
        ysRenderGeometryChannel::ChannelFormat format) {
    switch (format) {
        case ysRenderGeometryChannel::ChannelFormat::R32_FLOAT:
            return DXGI_FORMAT_R32_FLOAT;
        case ysRenderGeometryChannel::ChannelFormat::R32_UINT:
            return DXGI_FORMAT_R32_UINT;
        case ysRenderGeometryChannel::ChannelFormat::R32G32B32A32_FLOAT:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case ysRenderGeometryChannel::ChannelFormat::R32G32B32_FLOAT:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case ysRenderGeometryChannel::ChannelFormat::R32G32_FLOAT:
            return DXGI_FORMAT_R32G32_FLOAT;
        case ysRenderGeometryChannel::ChannelFormat::R32G32B32A32_INT:
            return DXGI_FORMAT_R32G32B32A32_SINT;
        case ysRenderGeometryChannel::ChannelFormat::R32G32B32A32_UINT:
            return DXGI_FORMAT_R32G32B32A32_UINT;
        case ysRenderGeometryChannel::ChannelFormat::R32G32B32_UINT:
            return DXGI_FORMAT_R32G32B32_UINT;
        case ysRenderGeometryChannel::ChannelFormat::Undefined:
        default:
            return DXGI_FORMAT_UNKNOWN;
    }
}

ysError ysD3D11Device::CreateD3D11DepthStencilView(
        ID3D11DepthStencilView **newDepthStencil,
        ID3D11ShaderResourceView **shaderResourceView, int width, int height,
        int count, int quality, bool shaderResource) {
    YDS_ERROR_DECLARE("CreateD3D11DepthBuffer");

    if (newDepthStencil == nullptr)
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newDepthStencil = nullptr;

    HRESULT result;

    ID3D11Texture2D *depthBuffer;

    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    if (shaderResource) {
        descDepth.Format = DXGI_FORMAT_R32_TYPELESS;
    } else {
        descDepth.Format = DXGI_FORMAT_D32_FLOAT;
    }

    descDepth.SampleDesc.Count = count;
    descDepth.SampleDesc.Quality = quality;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    if (shaderResource) descDepth.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    result = m_device->CreateTexture2D(&descDepth, nullptr, &depthBuffer);

    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotCreateDepthBuffer);
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = DXGI_FORMAT_D32_FLOAT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;

    if (shaderResource) {
        result = m_device->CreateDepthStencilView(depthBuffer, &descDSV,
                                                  newDepthStencil);
    } else {
        result = m_device->CreateDepthStencilView(depthBuffer, nullptr,
                                                  newDepthStencil);
    }

    depthBuffer->Release();

    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotCreateDepthBuffer);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::DestroyD3D11DepthStencilView(
        ID3D11DepthStencilView *&depthStencil) {
    YDS_ERROR_DECLARE("DestroyD3D11DepthBuffer");

    if (depthStencil == nullptr)
        return YDS_ERROR_RETURN(ysError::InvalidParameter);

    depthStencil->Release();
    depthStencil = nullptr;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError
ysD3D11Device::CreateD3D11OnScreenRenderTarget(ysRenderTarget *newTarget,
                                               ysRenderingContext *context,
                                               bool depthBuffer) {
    YDS_ERROR_DECLARE("CreateD3D11OnScreenRenderTarget");

    ysD3D11Context *d3d11Context = static_cast<ysD3D11Context *>(context);

    ID3D11Texture2D *backBuffer;
    ID3D11RenderTargetView *newRenderTargetView;
    ID3D11DepthStencilView *newDepthStencilView = nullptr;
    HRESULT result;

    result = d3d11Context->m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                                                  (LPVOID *) (&backBuffer));
    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotGetBackBuffer);
    }

    result = m_device->CreateRenderTargetView(backBuffer, nullptr,
                                              &newRenderTargetView);

    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotCreateRenderTarget);
    }

    // Create Depth Buffer
    if (depthBuffer) {
        ysError depthResult = CreateD3D11DepthStencilView(
                &newDepthStencilView, nullptr,
                context->GetWindow()->GetGameWidth(),
                context->GetWindow()->GetGameHeight(), m_multisampleCount,
                m_multisampleQuality, false);

        if (depthResult != ysError::None) {
            newRenderTargetView->Release();
            return YDS_ERROR_RETURN(depthResult);
        }
    }

    ysD3D11RenderTarget *newRenderTarget =
            static_cast<ysD3D11RenderTarget *>(newTarget);
    d3d11Context->m_attachedRenderTarget = newRenderTarget;

    newRenderTarget->m_type = ysRenderTarget::Type::OnScreen;
    newRenderTarget->m_posX = 0;
    newRenderTarget->m_posY = 0;
    newRenderTarget->m_width = context->GetWindow()->GetGameWidth();
    newRenderTarget->m_height = context->GetWindow()->GetGameHeight();
    newRenderTarget->m_physicalWidth = context->GetWindow()->GetScreenWidth();
    newRenderTarget->m_physicalHeight = context->GetWindow()->GetScreenHeight();
    newRenderTarget->m_format = ysRenderTarget::Format::R8G8B8A8_UNORM;
    newRenderTarget->m_hasDepthBuffer = depthBuffer;
    newRenderTarget->m_hasColorData = true;
    newRenderTarget->m_associatedContext = context;

    newRenderTarget->m_renderTargetView = newRenderTargetView;
    newRenderTarget->m_depthStencilView = newDepthStencilView;
    newRenderTarget->m_resourceView = nullptr;
    newRenderTarget->m_texture = backBuffer;

#if YDS_D3D11_USE_FLIP_MODEL
    newRenderTarget->m_parent = d3d11Context->m_msaaRenderTarget;
#endif

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysD3D11Device::CreateD3D11OffScreenRenderTarget(
        ysRenderTarget *target, int width, int height,
        ysRenderTarget::Format format, int msaa, bool colorData,
        bool depthBuffer) {
    YDS_ERROR_DECLARE("CreateD3D11OffScreenRenderTarget");

    HRESULT result;

    ID3D11Texture2D *renderTarget = nullptr;
    ID3D11RenderTargetView *newRenderTargetView = nullptr;
    ID3D11ShaderResourceView *shaderResourceView = nullptr;
    ID3D11DepthStencilView *newDepthStencil = nullptr;

    if (colorData) {
        // Create the texture
        D3D11_TEXTURE2D_DESC descBuffer;
        ZeroMemory(&descBuffer, sizeof(descBuffer));
        descBuffer.Width = width;
        descBuffer.Height = height;
        descBuffer.MipLevels = 1;
        descBuffer.ArraySize = 1;

        if (format == ysRenderTarget::Format::R32G32B32_FLOAT) {
            descBuffer.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
        } else if (format == ysRenderTarget::Format::R8G8B8A8_UNORM) {
            descBuffer.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
        } else if (format == ysRenderTarget::Format::R32_FLOAT) {
            descBuffer.Format = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
        }

        descBuffer.SampleDesc.Count = msaa;
        descBuffer.SampleDesc.Quality = 0;
        descBuffer.Usage = D3D11_USAGE_DEFAULT;

        descBuffer.BindFlags = 0;
        descBuffer.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
        if (colorData) {
            descBuffer.BindFlags |= D3D11_BIND_RENDER_TARGET;
        } else if (depthBuffer) {
            descBuffer.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
        }

        descBuffer.CPUAccessFlags = 0;
        descBuffer.MiscFlags = 0;
        result = m_device->CreateTexture2D(&descBuffer, nullptr, &renderTarget);

        if (FAILED(result)) {
            return YDS_ERROR_RETURN(ysError::CouldNotCreateRenderTarget);
        }

        // Create the render target view
        D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
        ZeroMemory(&rtDesc, sizeof(rtDesc));
        rtDesc.Format = descBuffer.Format;
        rtDesc.ViewDimension = (msaa == 1) ? D3D11_RTV_DIMENSION_TEXTURE2D
                                           : D3D11_RTV_DIMENSION_TEXTURE2DMS;
        rtDesc.Texture2D.MipSlice = 0;

        result = m_device->CreateRenderTargetView(renderTarget, &rtDesc,
                                                  &newRenderTargetView);

        if (FAILED(result)) {
            return YDS_ERROR_RETURN(ysError::CouldNotCreateRenderTarget);
        }

        // Create the shader resource view
        D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
        ZeroMemory(&srDesc, sizeof(srDesc));
        srDesc.Format = descBuffer.Format;
        srDesc.ViewDimension = (msaa == 1) ? D3D11_SRV_DIMENSION_TEXTURE2D
                                           : D3D11_SRV_DIMENSION_TEXTURE2DMS;
        srDesc.Texture2D.MostDetailedMip = 0;
        srDesc.Texture2D.MipLevels = 1;

        result = m_device->CreateShaderResourceView(renderTarget, &srDesc,
                                                    &shaderResourceView);

        if (FAILED(result)) {
            return YDS_ERROR_RETURN(ysError::CouldNotCreateRenderTarget);
        }
    }

    // Create Depth Buffer
    if (depthBuffer) {
        ysError depthResult;
        if (!colorData) {
            depthResult = CreateD3D11DepthStencilView(
                    &newDepthStencil, &shaderResourceView, width, height, msaa,
                    0, true);
        } else {
            depthResult = CreateD3D11DepthStencilView(
                    &newDepthStencil, nullptr, width, height, msaa, 0, false);
        }

        if (depthResult != ysError::None) {
            if (newRenderTargetView != nullptr) {
                newRenderTargetView->Release();
            }
            return YDS_ERROR_RETURN(depthResult);
        }
    }

    // Create the render target

    ysD3D11RenderTarget *newRenderTarget =
            static_cast<ysD3D11RenderTarget *>(target);

    newRenderTarget->m_type = ysRenderTarget::Type::OffScreen;
    newRenderTarget->m_posX = 0;
    newRenderTarget->m_posY = 0;
    newRenderTarget->m_width = width;
    newRenderTarget->m_height = height;
    newRenderTarget->m_physicalWidth = width;
    newRenderTarget->m_physicalHeight = height;
    newRenderTarget->m_format = format;
    newRenderTarget->m_hasDepthBuffer = depthBuffer;
    newRenderTarget->m_hasColorData = colorData;
    newRenderTarget->m_associatedContext = nullptr;
    newRenderTarget->m_depthTestEnabled = depthBuffer;
    newRenderTarget->m_msaa = msaa;

    newRenderTarget->m_renderTargetView = newRenderTargetView;
    newRenderTarget->m_depthStencilView = newDepthStencil;
    newRenderTarget->m_resourceView = shaderResourceView;
    newRenderTarget->m_texture = renderTarget;

    return YDS_ERROR_RETURN(ysError::None);
}

#define RELEASE_RESOURCE(resource)                                             \
    if (resource != nullptr) {                                                 \
        resource->Release();                                                   \
        resource = nullptr;                                                    \
    }

ysError ysD3D11Device::DestroyD3D11RenderTarget(ysRenderTarget *target) {
    YDS_ERROR_DECLARE("DestroyD3D11RenderTarget");

    ysD3D11RenderTarget *d3d11Target =
            static_cast<ysD3D11RenderTarget *>(target);
    RELEASE_RESOURCE(d3d11Target->m_renderTargetView);
    RELEASE_RESOURCE(d3d11Target->m_depthStencilView);
    RELEASE_RESOURCE(d3d11Target->m_resourceView);
    RELEASE_RESOURCE(d3d11Target->m_texture);

    d3d11Target->m_renderTargetView = nullptr;
    d3d11Target->m_depthStencilView = nullptr;
    d3d11Target->m_resourceView = nullptr;
    d3d11Target->m_texture = nullptr;

#if YDS_D3D11_USE_FLIP_MODEL
    if (d3d11Target->GetType() == ysRenderTarget::Type::OnScreen) {
        DestroyD3D11RenderTarget(d3d11Target->GetParent());
    }
#endif

    return YDS_ERROR_RETURN(ysError::None);
}

#endif /* Windows */
