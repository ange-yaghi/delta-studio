#include "../include/yds_depth_buffer.h"

ysDepthBuffer::ysDepthBuffer() : ysContextObject("DEPTH_BUFFER", API_UNKNOWN) {
    /* void */
}

ysDepthBuffer::ysDepthBuffer(DEVICE_API API) : ysContextObject("DEPTH_BUFFER", API) {
    /* void */
}

ysDepthBuffer::~ysDepthBuffer() {
    /* void */
}
