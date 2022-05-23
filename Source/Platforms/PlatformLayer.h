/*********************************************************************
 * \file   PlatformLayer.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _PLATFORM_LAYER_H
#define _PLATFORM_LAYER_H

namespace Enigma::Platforms
{
    class Debug
    {
    public:
        static int Printf(const char* format, ...);
        static int ErrorPrintf(const char* format, ...);
    };
}

#endif // !_PLATFORM_LAYER_H
