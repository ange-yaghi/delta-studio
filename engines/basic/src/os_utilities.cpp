#include "../include/os_utilities.h"

#include "../include/path.h"

#define NOMINMAX

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)
    #include "win32/windows_modular.h"

    #define MAX_PATH 260
    typedef unsigned long       DWORD;
#elif defined(_WIN64)
    #include <Windows.h>
#endif

dbasic::Path dbasic::GetModulePath() {
    // Windows only implementation for now
    wchar_t path[MAX_PATH];
    
    #if defined(_WIN64)
        DWORD result = GetModuleFileName(NULL, path, MAX_PATH);
    #endif /* Windows */

    Path fullPath = Path(path);
    Path parentPath;

    fullPath.GetParentPath(&parentPath);

    return parentPath;
}
