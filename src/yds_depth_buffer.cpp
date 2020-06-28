#include "../include/yds_depth_buffer.h"

ysDepthBuffer::ysDepthBuffer() : ysContextObject("DEPTH_BUFFER", DeviceAPI::Unknown) {
    /* void */
}

ysDepthBuffer::ysDepthBuffer(DeviceAPI API) : ysContextObject("DEPTH_BUFFER", API) {
    /* void */
}

ysDepthBuffer::~ysDepthBuffer() {
    /* void */
}
