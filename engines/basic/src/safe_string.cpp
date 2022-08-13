
#include "../include/safe_string.h"

#ifndef __STDC_LIB_EXT1__

#include <cstdlib>
#include <limits>
#include <stdarg.h>
#include <stdio.h>

int strcpy_s(char *__restrict dest, size_t destsz, const char *__restrict src) {
    if (destsz > std::numeric_limits<int>::max() || destsz == 0 || dest == nullptr || src == nullptr)
        std::abort();

    int i;
    for (i = 0; i < (destsz - 1); i++) {
        dest[i] = src[i];
        if (src[i] == '\0')
            return 0;
    }
    dest[destsz] = '\0';
    return 0;
}


int strcat_s(char *__restrict dest, size_t destsz, const char *__restrict src) {
    if (destsz > std::numeric_limits<int>::max() || destsz == 0 || dest == nullptr || src == nullptr)
        std::abort();

    int i;
    for (i = 0; i < (destsz - 1); i++) {
        if (dest[i] == '\0')
            goto found;
    }
    std::abort();

found:
    for (int j=0; i < (destsz - 1); i++, j++) {
        dest[i] = src[j];
        if (src[j] == '\0')
            return 0;
    }
    dest[destsz] = '\0';
    return 0;
}

int sprintf_s(char *__restrict buffer, size_t bufsz, const char *__restrict format, ...) {
    if (bufsz > std::numeric_limits<int>::max() || bufsz == 0 || buffer == nullptr || format == nullptr)
        std::abort();

    // This is nowhere near as safe as the real sprintf_s. It doesn't do sanity checking on the
    // format string and args

    va_list args;
    va_start(args, format);
    int ret = vsnprintf(buffer, bufsz, format, args);
    va_end(args);

    if (ret < 0 || ret >= bufsz)
        std::abort();

    return ret;
}

#endif
