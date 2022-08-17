#include "../include/os_utilities.h"

#include "../include/path.h"

#include <filesystem>

dbasic::Path dbasic::GetModulePath()
{
    char path[256];
    std::filesystem::current_path(path);

    return path;
}
