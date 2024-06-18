#include "../include/yds_file_logger.h"

// safestringlib is available in windows by default & there's no need to include
#if !defined(_WIN64)
    #include "include/safe_lib.h"
#endif

ysFileLogger::ysFileLogger() : ysLoggerOutput("ysFileLogger") {
    /* void */
}

ysFileLogger::~ysFileLogger() {
    /* void */
}

void ysFileLogger::OpenFile(const char *fname) {
    strcpy_s(m_fname, 256, fname);
}

void ysFileLogger::Initialize() {
    m_stream.open(m_fname, std::ios::out);
}

void ysFileLogger::Close() {
    m_stream.close();
}

void ysFileLogger::Write(const char *data) {
    m_stream.write(data, strlen(data));
}
