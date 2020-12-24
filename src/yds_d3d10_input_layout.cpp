#include "../include/yds_d3d10_input_layout.h"

ysD3D10InputLayout::ysD3D10InputLayout() : ysInputLayout(DeviceAPI::DirectX10) {
    m_layout = NULL;
}

ysD3D10InputLayout::~ysD3D10InputLayout() {
    /* void */
}
