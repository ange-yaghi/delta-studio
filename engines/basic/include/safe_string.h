#pragma once

#include <string.h>

#ifndef __STDC_LIB_EXT1__

// Portable versions of _s functions for non-windows platforms

// This is probably a horrible idea, my original plan was to replace all uses
// of _s functions with std::string where possible.
//
// But they are everywhere in the codebase, and we need to do this port in smaller
// steps. So this is the more sane solution in the short term.
//
// Ideally we will remove these later

int strcpy_s(char *__restrict dest, int destsz, const char *__restrict src);
int strcat_s(char *__restrict dest, int destsz, const char *__restrict src);
int sprintf_s(char *__restrict buffer, int bufsz, const char *__restrict format, ...);

#endif