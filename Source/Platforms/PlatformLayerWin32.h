/*********************************************************************
 * \file   PlatformLayerWin32.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _PLATFORM_LAYER_WIN32_H
#define _PLATFORM_LAYER_WIN32_H

#include <stdio.h>

namespace Enigma::Platforms
{
    class Debug
    {
    public:
        static int Printf(const char* format, ...);
        static int ErrorPrintf(const char* format, ...);
    };
}

#endif // !_PLATFORM_LAYER_WIN32_H
