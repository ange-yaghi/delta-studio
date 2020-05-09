#include "../include/yds_input_layout.h"

ysInputLayout::ysInputLayout() : ysContextObject("INPUT_LAYOUT", API_UNKNOWN) {
	m_format = nullptr;
	m_shader = nullptr;
}

ysInputLayout::ysInputLayout(DEVICE_API API) : ysContextObject("INPUT_LAYOUT", API) {
	m_format = nullptr;
	m_shader = nullptr;
}

ysInputLayout::~ysInputLayout() {
    /* void */
}
