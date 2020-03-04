#include "../include/window_handler.h"

#include "../include/delta_engine.h"

dbasic::WindowHandler::WindowHandler() {
    m_device = NULL;
    m_context = NULL;
}

dbasic::WindowHandler::~WindowHandler() {
    /* void */
}

void dbasic::WindowHandler::OnResizeWindow(int width, int height) {
    m_device->UpdateRenderingContext(m_context);
    m_engine->SetWindowSize(width, height);
}

void dbasic::WindowHandler::Initialize(ysDevice *device, ysRenderingContext *context, DeltaEngine *engine) {
    m_device = device;
    m_context = context;
    m_engine = engine;
}
