#include "../include/yds_d3d11_input_layout.h"

ysD3D11InputLayout::ysD3D11InputLayout() : ysInputLayout(DeviceAPI::DirectX11) {
    /* void */
}

ysD3D11InputLayout::~ysD3D11InputLayout() {
    /* void */
}

void ysD3D11InputLayout::SetDebugName(const std::string &debugName) {
    ysInputLayout::SetDebugName(debugName);

    if (m_layout != nullptr) {
        m_layout->SetPrivateData(WKPDID_D3DDebugObjectName, debugName.size(), debugName.c_str());
    }
}
