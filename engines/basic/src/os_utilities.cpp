#include "../include/os_utilities.h"

#include "../include/path.h"

#include <Windows.h>

dbasic::Path dbasic::GetModulePath() {
    // Windows only implementation for now
    char path[256];
    DWORD result = GetModuleFileName(NULL, path, 256);

    Path fullPath = Path(path);
    Path parentPath;

    fullPath.GetParentPath(&parentPath);

    return parentPath;
}
