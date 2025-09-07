#include <string>
#include <windows.h>

std::string wstring_to_utf8(const std::wstring& wstr)
{
    if (wstr.empty()) return {};

    int size_needed = WideCharToMultiByte(
        CP_UTF8, 0, wstr.data(), (int)wstr.size(),
        nullptr, 0, nullptr, nullptr
    );

    std::string str(size_needed, 0);
    WideCharToMultiByte(
        CP_UTF8, 0, wstr.data(), (int)wstr.size(),
        str.data(), size_needed, nullptr, nullptr
    );

    return str;
}