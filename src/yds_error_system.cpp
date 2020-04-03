// TEMP
#include <Windows.h>
#include <WinBase.h>

#include "../include/yds_error_system.h"

#include "../include/yds_error_handler.h"

#include <assert.h>

ysErrorSystem *ysErrorSystem::g_instance = NULL;

ysErrorSystem::ysErrorSystem() {
	if (g_instance != NULL) 
		YDS_ERROR_RAISE(ysError::YDS_MULTIPLE_ERROR_SYSTEMS);

	g_instance = this;

    memset((void *)m_callStack, 0, sizeof(m_callStack));
	m_stackLevel = 0;
}

ysErrorSystem::~ysErrorSystem() {
    /* void */
}

ysErrorSystem *ysErrorSystem::GetInstance() {
	if (g_instance == NULL) g_instance = new ysErrorSystem;
	return g_instance;
}

void ysErrorSystem::Destroy() {
	delete [] g_instance;
}

ysError ysErrorSystem::RaiseError(ysError error, unsigned int line, ysObject *object, const char *file, const char *msg, bool affectStack) {
	if (error != ysError::YDS_NO_ERROR) {
		for(int i = 0; i < m_errorHandlers.GetNumObjects(); i++) {
			m_errorHandlers.Get(i)->OnError(error, line, object, file);
		}
	}

	if (affectStack) {
		m_stackLevel--;
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
    m_stackLevel--;
}
