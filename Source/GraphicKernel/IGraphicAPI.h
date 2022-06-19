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
#include "Frameworks/ExtentTypesDefine.h"
#include "MathLib/AlgebraBasicTypes.h"
#include <system_error>

namespace Enigma::MathLib
{
    class ColorRGBA;
}

namespace Enigma::Graphics
{
    using error = std::error_code;

    class GraphicThread;
    class IBackSurface;
    using IBackSurfacePtr = std::shared_ptr<IBackSurface>;
    class IDepthStencilSurface;
    using IDepthStencilSurfacePtr = std::shared_ptr<IDepthStencilSurface>;

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
        virtual error CreateDevice(const DeviceRequiredBits& rqb, void* hwnd) = 0;
        virtual error CleanupDevice() = 0;
        virtual future_error AsyncCreateDevice(const DeviceRequiredBits& rqb, void* hwnd);
        virtual future_error AsyncCleanupDevice();
        //@}

        /** @name back / depth surface */
        //@{
        virtual error GetPrimaryBackSurface(IBackSurfacePtr* back_surface, IDepthStencilSurfacePtr* depth_surface) = 0;
        virtual error CreateBackSurface(const MathLib::Dimension& dimension, const GraphicFormat& fmt,
            IBackSurfacePtr* back_surface) = 0;
        virtual error CreateBackSurface(const MathLib::Dimension& dimension, unsigned int buff_count,
            const std::vector<GraphicFormat>& fmts, IBackSurfacePtr* back_surface) = 0;
        virtual error CreateDepthStencilSurface(const MathLib::Dimension& dimension, const GraphicFormat& fmt,
            IDepthStencilSurfacePtr* depth_surface) = 0;
        virtual error ShareDepthStencilSurface(const IDepthStencilSurfacePtr& from_depth,
            IDepthStencilSurfacePtr* depth_surface) = 0;
        virtual error ClearSurface(const IBackSurfacePtr& back_surface, const IDepthStencilSurfacePtr& depth_surface,
            const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value) = 0;
        virtual future_error AsyncGetPrimaryBackSurface(
            IBackSurfacePtr* back_surface, IDepthStencilSurfacePtr* depth_surface);
        virtual future_error AsyncCreateBackSurface(const MathLib::Dimension& dimension, const GraphicFormat& fmt,
            IBackSurfacePtr* back_surface);
        virtual future_error AsyncCreateBackSurface(const MathLib::Dimension& dimension, unsigned int buff_count,
            const std::vector<GraphicFormat>& fmts, IBackSurfacePtr* back_surface);
        virtual future_error AsyncCreateDepthStencilSurface(const MathLib::Dimension& dimension, const GraphicFormat& fmt,
            IDepthStencilSurfacePtr* depth_surface);
        virtual future_error AsyncShareDepthStencilSurface(const IDepthStencilSurfacePtr& from_depth,
            IDepthStencilSurfacePtr* depth_surface);
        virtual future_error AsyncClearSurface(
            const IBackSurfacePtr& back_surface, const IDepthStencilSurfacePtr& depth_surface,
            const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value);
        //@}


        virtual void TerminateGraphicThread();
        virtual GraphicThread* GetGraphicThread();

    protected:
        /** @name surface format */
        //@{
        void SetPrimaryBackSurfaceFormat(const GraphicFormat& fmt) { m_fmtBackSurface = fmt; };
        void SetDepthSurfaceFormat(const GraphicFormat& fmt) { m_fmtDepthSurface = fmt; };
        //@}

    protected:
        static IGraphicAPI* m_instance;
        void* m_wnd;
        DeviceRequiredBits m_deviceRequiredBits;
        APIVersion m_apiVersion;

        GraphicFormat m_fmtBackSurface;
        GraphicFormat m_fmtDepthSurface;

        GraphicThread* m_workerThread;
    };
}

#endif // GRAPHIC_API_INTERFACE_H
