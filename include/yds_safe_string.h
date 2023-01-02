#pragma once

#include <cstring>
#include <utility>

// __STDC_LIB_EXT1__ isn't defined by MSVC
#ifndef _MSC_VER

// Portable versions of _s functions for non-windows platforms

// This is probably a horrible idea, my original plan was to replace all uses
// of _s functions with std::string where possible.
//
// But they are everywhere in the codebase, and we need to do this port in smaller
// steps. So this is the more sane solution in the short term.
//
// Ideally we will remove these later

int strcpy_s(char *__restrict dest, size_t destsz, const char *__restrict src);
int strcat_s(char *__restrict dest, size_t destsz, const char *__restrict src);
int sprintf_s(char *__restrict buffer, size_t bufsz, const char *__restrict format, ...);

template <size_t size>
int strcpy_s(char (&buffer)[size], const char *src) {
    return strcpy_s(buffer, size, src);
}
template <size_t size, typename ...Args>
int sprintf_s(char (&buffer)[size], const char *__restrict format, Args&&...args) {
    return sprintf_s(buffer, size, format, std::forward<Args>(args)...);
}

#endif
