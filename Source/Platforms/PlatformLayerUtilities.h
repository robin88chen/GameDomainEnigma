/*********************************************************************
 * \file   PlatformLayerUtilities.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _PLATFORM_LAYER_UTILITIES_H
#define _PLATFORM_LAYER_UTILITIES_H

#include "PlatformConfig.h"

#if TARGET_PLATFORM == PLATFORM_WIN32
#include "PlatformLayerWin32.h"
#endif

#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "PlatformLayerAndroid.h"
#endif

#endif // !_PLATFORM_LAYER_UTILITIES_H
