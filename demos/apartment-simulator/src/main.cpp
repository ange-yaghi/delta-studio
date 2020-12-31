#include <Windows.h>

#include "../include/ApartmentSimulator.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    (void)nCmdShow;
    (void)lpCmdLine;
    (void)hPrevInstance;

    // Top Stack Level
    YDS_ERROR_DECLARE("WinMain");

    ApartmentSimulator apartmentSimulator;

    apartmentSimulator.Initialize(ysDevice::DeviceAPI::DirectX11, (void *)&hInstance);
    apartmentSimulator.GameLoop();
    apartmentSimulator.Destroy();

    YDS_ERROR_RETURN_STATIC(ysError::None);

    return 0;
}
