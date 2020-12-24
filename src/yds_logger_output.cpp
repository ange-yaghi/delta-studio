#include "../include/yds_logger_output.h"
#include "../include/yds_logger.h"

#include <time.h>

ysLoggerOutput::ysLoggerOutput() : ysObject("ysLoggerOutput") {
    m_level = -1;
    m_parentLogger = NULL;
    m_formatParameters = LOG_DEFAULT;

    ClearBuffer();
}

ysLoggerOutput::ysLoggerOutput(const char *typeID) : ysObject(typeID) {
    m_level = -1;
    m_parentLogger = NULL;
    m_formatParameters = LOG_DEFAULT;

    ClearBuffer();
}

ysLoggerOutput::~ysLoggerOutput() {
    /* void */
}

void ysLoggerOutput::SetLogger(ysLogger *logger) {
    m_parentLogger = logger;
}

void ysLoggerOutput::LogMessage(const char *message, const char *fname, int line, int level) {
    char buffer[256];

    ClearBuffer();

    time_t now = time(0);
    struct tm tstruct;
    localtime_s(&tstruct, &now);

    if ((m_formatParameters & LOG_DATE) > 0) {
        strftime(buffer, 256, "%Y-%m-%d", &tstruct);
        WriteToBuffer(buffer, 15);
    }

    if ((m_formatParameters & LOG_TIME) > 0) {
        strftime(buffer, 256, "%X", &tstruct);
        WriteToBuffer(buffer, 15);
    }

    if ((m_formatParameters & LOG_LEVEL) > 0) {
        WriteToBuffer(m_parentLogger->GetLevelName(level), 15);
    }

    if ((m_formatParameters & LOG_MODULE) > 0) {
        WriteToBuffer(fname, 45);
    }

    if ((m_formatParameters & LOG_LINE) > 0) {
        sprintf_s(buffer, 256, "%d", line);
        WriteToBuffer(buffer, 6);
    }

    WriteToBuffer(message);
    WriteToBuffer("\n");

    // Submit the entire message
    Write(m_buffer);
}

void ysLoggerOutput::ClearBuffer() {
    m_buffer[0] = '\0';
    m_bufferLength = 0;
}

void ysLoggerOutput::WriteToBuffer(const char *data, int width) {
    int dataLength = (int)strlen(data);
    int padding = 0;

    if (width > 0) {
        padding = width - dataLength;
    }

    strcat_s(m_buffer, 1024, data);
    m_bufferLength += dataLength;
    
    if (padding > 0) {
        for (int i = 0; i < padding; i++) {
            m_buffer[m_bufferLength] = ' ';
            m_bufferLength++;
        }

        m_buffer[m_bufferLength] = '\0';
    }
}
