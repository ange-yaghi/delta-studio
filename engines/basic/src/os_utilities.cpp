#include "../include/os_utilities.h"

#include "../include/path.h"

dbasic::Path dbasic::GetModulePath()
{
    return dbasic::filesystem::current_path();
}
