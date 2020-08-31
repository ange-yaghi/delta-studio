#include <Windows.h>

#include "../include/delta_basic_demo_application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    (void)nCmdShow;
    (void)lpCmdLine;
    (void)hPrevInstance;

    dbasic_demo::DeltaBasicDemoApplication app; 
    app.Initialize((void *)&hInstance, ysContextObject::DeviceAPI::OpenGL4_0);
    app.Run();

    return 0;
}
