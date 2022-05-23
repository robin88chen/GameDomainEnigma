#include "PlatformLayerUtilities.h"

#if TARGET_PLATFORM == PLATFORM_ANDROID
#include <android/log.h>

namespace Enigma::Platforms
{
    int Debug::Printf(const char* format, ...)
    {
        va_list argList;
        va_start(argList, format);
        int nWritten = __android_log_vprint(ANDROID_LOG_DEBUG, "EnigmaAndroid", format, argList);
        va_end(argList);
        return nWritten;
    }
    int Debug::ErrorPrintf(const char* format, ...)
    {
        va_list argList;
        va_start(argList, format);
        int nWritten = __android_log_vprint(ANDROID_LOG_ERROR, "EnigmaAndroid", format, argList);
            va_end(argList);
        return nWritten;
    }
}

#endif