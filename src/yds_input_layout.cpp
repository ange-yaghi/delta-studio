#include "../include/yds_input_layout.h"

ysInputLayout::ysInputLayout() : ysContextObject("INPUT_LAYOUT", DeviceAPI::Unknown) {
    m_format = nullptr;
    m_shader = nullptr;
}

ysInputLayout::ysInputLayout(DeviceAPI API) : ysContextObject("INPUT_LAYOUT", API) {
    m_format = nullptr;
    m_shader = nullptr;
}

ysInputLayout::~ysInputLayout() {
    /* void */
}
