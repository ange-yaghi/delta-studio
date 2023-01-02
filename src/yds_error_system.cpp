#include "../include/yds_error_system.h"

#include "../include/yds_error_handler.h"

#include <assert.h>

ysErrorSystem *ysErrorSystem::g_instance = nullptr;

ysErrorSystem::ysErrorSystem() {
    if (g_instance != nullptr) {
        YDS_ERROR_RAISE(ysError::MultipleErrorSystems);
    }

    g_instance = this;

    memset((void *)m_callStack, 0, sizeof(m_callStack));
    m_stackLevel = 0;
}

ysErrorSystem::~ysErrorSystem() {
    /* void */
}

ysErrorSystem *ysErrorSystem::GetInstance() {
    if (g_instance == nullptr) g_instance = new ysErrorSystem;
    return g_instance;
}

void ysErrorSystem::Destroy() {
    delete [] g_instance;
}

ysError ysErrorSystem::RaiseError(ysError error, unsigned int line, ysObject *object, const char *file, const char *msg, bool affectStack) {
    if (error != ysError::None) {
        for (int i = 0; i < m_errorHandlers.GetNumObjects(); i++) {
            m_errorHandlers.Get(i)->OnError(error, line, object, file);
        }
    }

    if (affectStack) {
        StackDescend();
    }

    assert(m_stackLevel >= 0);

    return error;
}

void ysErrorSystem::StackRaise(const char *callName) {
    assert(m_stackLevel >= 0);

    m_callStack[m_stackLevel] = callName;
    m_stackLevel++;
}

void ysErrorSystem::StackDescend() {
    --m_stackLevel;
}
