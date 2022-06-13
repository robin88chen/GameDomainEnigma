/********************************************************************
 * \file   IGraphicAPI.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef GRAPHIC_API_INTERFACE_H
#define GRAPHIC_API_INTERFACE_H

#include "DeviceRequiredBits.h"
#include "ExtentTypesDefine.h"
#include <system_error>

namespace Enigma::Graphics
{
    using error = std::system_error;

    class IGraphicAPI
    {
    public:
        enum class APIVersion
        {
            API_Dx9,
            API_Dx11,
            API_EGL,
        };
        enum class AsyncType : bool
        {
            UseAsyncDevice = true,
            NotAsyncDevice = false
        };
    public:
        IGraphicAPI();
        IGraphicAPI(const IGraphicAPI&) = delete;
        virtual ~IGraphicAPI();
        IGraphicAPI& operator=(const IGraphicAPI&) = delete;

        static IGraphicAPI* Instance();

        /** @name create / cleanup device */
        //@{
        virtual error CreateDevice(const DeviceRequiredBits& rqb, AsyncType use_async, void* hwnd) = 0;
        virtual error CleanupDevice() = 0;
        virtual future_error AsyncCreateDevice(const DeviceRequiredBits& rqb);
        virtual future_error AsyncCleanupDevice();
        //@}
    };
}

#endif // GRAPHIC_API_INTERFACE_H
