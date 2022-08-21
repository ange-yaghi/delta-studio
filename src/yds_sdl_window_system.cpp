#include "../include/yds_sdl_window_system.h"

#include "../include/yds_sdl_input_system.h"
#include "../include/yds_sdl_monitor.h"
#include "../include/yds_sdl_window.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>

ysSdlWindowSystem::ysSdlWindowSystem() : ysWindowSystem(Platform::Sdl) {
    SDL_Init(SDL_INIT_VIDEO);
}

ysSdlWindowSystem::~ysSdlWindowSystem() {}

ysError ysSdlWindowSystem::NewWindow(ysWindow **newWindow) {
    YDS_ERROR_DECLARE("NewWindow");

    if (newWindow == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);


    ysSdlWindow *sdlWindow = m_windowArray.NewGeneric<ysSdlWindow>();

    *newWindow = static_cast<ysWindow *>(sdlWindow);
    return YDS_ERROR_RETURN(ysError::None);
}

ysMonitor *ysSdlWindowSystem::NewMonitor() {
    return m_monitorArray.NewGeneric<ysSdlMonitor>();
}

void ysSdlWindowSystem::SurveyMonitors() {
    ysWindowSystem::SurveyMonitors();

    int numMonitors = SDL_GetNumVideoDisplays();

    for (int i = 0; i < numMonitors; i++) {
        ysMonitor *monitor = NewMonitor();

        SDL_Rect rect;
        SDL_GetDisplayBounds(i, &rect);
        monitor->SetOrigin(rect.x, rect.y);

        // FIXME: One of these two sizes is not right for HiDPI displays
        //        But i'm not sure which. Maybe both?
        //
        // SDL documentation recommends using SDL_GetWindowSize to find logical size
        // and SDL_GL_GetDrawableSize() (or Vulkan, or Metal) to find physical size
        // But that requires both a window and a context to be created first
        // They promise to fix it in SDL 3
        monitor->SetLogicalSize(rect.w, rect.h);
        monitor->SetPhysicalSize(rect.w, rect.h);

        // Which means this is going to be wrong too
        monitor->CalculateScaling();
    }
}

void ysSdlWindowSystem::ProcessMessages() {
    auto *inputSystem = static_cast<ysSdlInputSystem *>(GetInputSystem());

    // Grab all the queued events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                // TODO: quit
                break;

            // Input events
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEWHEEL:
                inputSystem->ProcessEvent(event);
                break;

            default:
                // Ignore it
                break;
        }
    }
}
