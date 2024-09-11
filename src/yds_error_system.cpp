// TEMP
#define NOMINMAX

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)
    #include "win32/windows_modular.h"
#elif defined(_WIN64)
    #include <Windows.h>
#endif


#include "../include/yds_error_system.h"

#include "../include/yds_error_handler.h"

#include <assert.h>

ysErrorSystem *ysErrorSystem::g_instance = nullptr;

ysErrorSystem::ysErrorSystem() {
    if (g_instance != nullptr) {
        YDS_ERROR_RAISE(ysError::MultipleErrorSystems);
    }

    g_instance = this;
}

ysErrorSystem::~ysErrorSystem() {}

ysErrorSystem *ysErrorSystem::GetInstance() {
    if (g_instance == nullptr) { g_instance = new ysErrorSystem; }
    return g_instance;
}

void ysErrorSystem::Destroy() { delete[] g_instance; }

ysError ysErrorSystem::RaiseError(ysError error, unsigned int line,
                                  ysObject *object, const char *file,
                                  const char *msg, bool affectStack) {
    if (error != ysError::None) {
        std::lock_guard<std::mutex> lk(m_lock);
        for (int i = 0; i < m_errorHandlers.GetNumObjects(); i++) {
            m_errorHandlers.Get(i)->OnError(error, line, object, file);
        }
    }

    return error;
}
