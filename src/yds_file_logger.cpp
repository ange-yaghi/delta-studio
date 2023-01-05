#include "../include/yds_file_logger.h"

#include <iomanip>

ysFileLogger::ysFileLogger() : ysLoggerOutput("ysFileLogger") {
    /* void */
}

ysFileLogger::~ysFileLogger() {
    /* void */
}

void ysFileLogger::OpenFile(std::string fname) {
    m_fname = fname;
}

void ysFileLogger::Initialize() {
    m_stream.open(m_fname.c_str(), std::ios::out);
}

void ysFileLogger::Close() {
    m_stream.close();
}

void ysFileLogger::Write(std::string_view data, int padding) {
    m_stream << std::setw(padding) << data;
}
