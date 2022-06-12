/********************************************************************
 * \file   IGraphicAPI.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef GRAPHIC_API_INTERFACE_H
#define GRAPHIC_API_INTERFACE_H

namespace Enigma::Graphics
{
    class IGraphicAPI
    {
    public:
        enum class APIVersion
        {
            API_Dx9,
            API_Dx11,
            API_EGL,
        };
    public:
        IGraphicAPI();
        IGraphicAPI(const IGraphicAPI&) = delete;
        virtual ~IGraphicAPI();
        IGraphicAPI& operator=(const IGraphicAPI&) = delete;

        static IGraphicAPI* Instance();
    };
}

#endif // GRAPHIC_API_INTERFACE_H
