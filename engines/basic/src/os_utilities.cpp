#include "../include/os_utilities.h"

#include "../include/path.h"

#include <Windows.h>

dbasic::Path dbasic::GetModulePath() {
    // Windows only implementation for now
    wchar_t path[MAX_PATH];
    DWORD result = GetModuleFileName(NULL, path, MAX_PATH);

    Path fullPath = Path(path);
    Path parentPath;

    fullPath.GetParentPath(&parentPath);

    return parentPath;
}
