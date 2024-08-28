#define NOMINMAX

#include "../include/demo_list.h"

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)

#include <stdio.h>

#define GLAD_GL_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION

#include "glad/glad.h"

// Without this gl.h gets included instead of gl3.h
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// For includes related to OpenGL, make sure their are included after glfw3.h
#import <OpenGL/OpenGL.h>
 
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

// TODO: - 
int main(void) {
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    
//    glfwDestroyWindow(window);
 
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

#elif defined(_WIN64) // -------------------------------------------------------

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
