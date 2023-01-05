#include "../include/yds_logger.h"

#include <stdarg.h>

ysLogger *ysLogger::g_instance = NULL;

ysLogger::ysLogger() {
    /* void */
}

ysLogger::~ysLogger() {
    /* void */
}

void ysLogger::LogMessage(const char *message, const char *fname, int line, int level, ...) {
    char buffer[1024];

    va_list argptr;
    va_start(argptr, level);
    vsnprintf(buffer, 1024, message, argptr);
    va_end(argptr);

    int nTargets = m_loggerOutputs.GetNumObjects();

    for (int i = 0; i < nTargets; i++) {
        m_loggerOutputs.Get(i)->LogMessage(buffer, fname, line, level);
    }
}

void ysLogger::AddMessageLevel(int level, const char *name) {
    m_messageLevels[level].m_level = level;
    m_messageLevels[level].m_name = name;
    m_messageLevels[level].m_valid = true;
}

void ysLogger::Initialize() {
    // Add default levels
    AddMessageLevel(0,    "DEBUG");
    AddMessageLevel(10, "INFO");
    AddMessageLevel(20, "WARNING");
    AddMessageLevel(30, "ERROR");
    AddMessageLevel(40, "CRITICAL");
}

void ysLogger::Start() {
    int nTargets = m_loggerOutputs.GetNumObjects();

    for (int i = 0; i < nTargets; i++) {
        m_loggerOutputs.Get(i)->Initialize();
    }
}

void ysLogger::End() {
    int nTargets = m_loggerOutputs.GetNumObjects();

    for (int i = 0; i < nTargets; i++) {
        m_loggerOutputs.Get(i)->Close();
    }
}

ysLoggerMessageLevel::ysLoggerMessageLevel() : ysLoggerMessageLevel("", -1) {
    m_valid = false;
}

ysLoggerMessageLevel::ysLoggerMessageLevel(std::string name, int level) {
    m_name = name;
    m_level = level;
    m_valid = true;
}

ysLoggerMessageLevel::~ysLoggerMessageLevel() {
    /* void */
}
