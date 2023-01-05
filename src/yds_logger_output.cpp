#include "../include/yds_logger_output.h"
#include "../include/yds_logger.h"

#include <time.h>

ysLoggerOutput::ysLoggerOutput() : ysObject("ysLoggerOutput") {
    m_level = -1;
    m_parentLogger = NULL;
    m_formatParameters = LOG_DEFAULT;
}

ysLoggerOutput::ysLoggerOutput(const char *typeID) : ysObject(typeID) {
    m_level = -1;
    m_parentLogger = NULL;
    m_formatParameters = LOG_DEFAULT;
}

ysLoggerOutput::~ysLoggerOutput() {
    /* void */
}

void ysLoggerOutput::SetLogger(ysLogger *logger) {
    m_parentLogger = logger;
}

void ysLoggerOutput::LogMessage(const char *message, const char *fname, int line, int level) {
    char buffer[256];

    time_t now = time(0);
    struct tm* tstruct = localtime(&now);

    if ((m_formatParameters & LOG_DATE) > 0) {
        size_t len = strftime(buffer, 256, "%Y-%m-%d", tstruct);
        Write(std::string_view(buffer, len), 15);
    }

    if ((m_formatParameters & LOG_TIME) > 0) {
        size_t len = strftime(buffer, 256, "%X", tstruct);
        Write(std::string_view(buffer, len), 15);
    }

    if ((m_formatParameters & LOG_LEVEL) > 0) {
        Write(m_parentLogger->GetLevelName(level), 15);
    }

    if ((m_formatParameters & LOG_MODULE) > 0) {
        Write(fname, 45);
    }

    if ((m_formatParameters & LOG_LINE) > 0) {
        size_t len = snprintf(buffer, 256, "%d", line);
        Write(std::string_view(buffer, len), 6);
    }

    Write(message);
    Write("\n");
}
