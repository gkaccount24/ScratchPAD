#ifndef DEFS_H
#define DEFS_H

#define PLATFORM_WIN64

#ifdef PLATFORM_WIN64

#include "win32_defs.h"

#elif PLATFORM_LINUX64

#include "posix_defs.h"

#endif

#include "common_defs.h"

struct geometry 
{
    int X;     int Y;
    int Width; int Height;
};

#endif