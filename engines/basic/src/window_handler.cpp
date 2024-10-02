#include "../include/window_handler.h"

#include "../include/delta_engine.h"

dbasic::WindowHandler::WindowHandler() {
    m_device = nullptr;
    m_context = nullptr;
    m_engine = nullptr;
    m_attachment = nullptr;
}

dbasic::WindowHandler::~WindowHandler() {}

void dbasic::WindowHandler::OnResizeWindow(int width, int height) {
    m_engine->SetWindowSize(width, height);
    m_device->UpdateRenderingContext(m_context);
    m_engine->GetConsole()->ResetScreenPosition();
}

void dbasic::WindowHandler::Initialize(ysDevice *device,
                                       ysRenderingContext *context,
                                       DeltaEngine *engine) {
    m_device = device;
    m_context = context;
    m_engine = engine;
}

void dbasic::WindowHandler::OnStartMoveResize() { m_engine->SetPaused(true); }
void dbasic::WindowHandler::OnEndMoveResize() { m_engine->SetPaused(false); }

void dbasic::WindowHandler::OnCloseWindow() {
    if (m_attachment == nullptr) { return; }
    m_attachment->OnClose();
}
