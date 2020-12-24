#include "../include/yds_file_logger.h"

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
