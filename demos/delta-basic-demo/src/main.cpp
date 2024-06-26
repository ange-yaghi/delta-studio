#define NOMINMAX

#include "../include/demo_list.h"

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)

// TODO: - 

#elif defined(_WIN64)

#include <Windows.h>

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{
    (void)nCmdShow;
    (void)lpCmdLine;
    (void)hPrevInstance;

    ysContextObject::DeviceAPI api = ysContextObject::DeviceAPI::DirectX11;
    delta_demo::DemoApplication::Demo demo =
        delta_demo::DemoApplication::Demo::DeltaBasicDemo;

    delta_demo::DemoApplication *app = delta_demo::CreateApplication(demo);
    app->Initialize((void *)&hInstance, api);
    app->Run();
    app->Destroy();

    return 0;
}

#endif /* Windows */
