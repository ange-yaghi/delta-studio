#include "../include/yds_input_layout.h"

ysInputLayout::ysInputLayout() : ysContextObject("INPUT_LAYOUT", API_UNKNOWN) {
	m_format = NULL;
	m_shader = NULL;
}

ysInputLayout::ysInputLayout(DEVICE_API API) : ysContextObject("INPUT_LAYOUT", API) {
	m_format = NULL;
	m_shader = NULL;
}

ysInputLayout::~ysInputLayout() {
    /* void */
}
