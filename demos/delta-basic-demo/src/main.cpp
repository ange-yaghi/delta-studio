
#ifdef _WIN32
#include <Windows.h>
#endif

#include "../include/demo_list.h"

int main_inner(void* instance) {
    ysContextObject::DeviceAPI api = ysContextObject::DeviceAPI::OpenGL4_0;
    delta_demo::DemoApplication::Demo demo =
        delta_demo::DemoApplication::Demo::DeltaBasicDemo;

    delta_demo::DemoApplication *app = delta_demo::CreateApplication(demo);
    app->Initialize(instance, api);
    app->Run();
    app->Destroy();

    return 0;
}

#ifdef _WIN32
int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{
    (void)nCmdShow;
    (void)lpCmdLine;
    (void)hPrevInstance;

    return main_inner((void *)&hInstance);
}
#else
int main(int argc, char **argv) {
    return main_inner(nullptr);
}
#endif
