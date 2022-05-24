#include "PlatformLayerUtilities.h"
#if TARGET_PLATFORM == PLATFORM_WIN32
#include <Windows.h>
#include <stdio.h>

int _Win32_VDebugPrintf(const char* format, va_list argList)
{
    const unsigned int MAX_CHARS = 1023;
    static char s_buffer[MAX_CHARS + 1];
    int nWritten = vsnprintf_s(s_buffer, MAX_CHARS, format, argList);
    s_buffer[MAX_CHARS] = '\0';
    OutputDebugString(s_buffer);
    return nWritten;
}

namespace Enigma::Platforms
{
    int Debug::Printf(const char* format, ...)
    {
        va_list argList;
        va_start(argList, format);
        int nWritten = _Win32_VDebugPrintf(format, argList);
        va_end(argList);
        return nWritten;
    }
    int Debug::ErrorPrintf(const char* format, ...)
    {
        va_list argList;
        va_start(argList, format);
        int nWritten = _Win32_VDebugPrintf(format, argList);
        va_end(argList);
        return nWritten;
    }
}
#endif