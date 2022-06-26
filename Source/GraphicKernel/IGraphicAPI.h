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
#include "TargetViewPort.h"
#include "GraphicAssetRepository.h"
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
    class TargetViewPort;
    class AssetRepository;

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

        virtual error Flip() = 0;
        virtual future_error AsyncFlip();

        /** @name back / depth surface */
        //@{
        virtual error CreatePrimaryBackSurface(const std::string& back_name, const std::string& depth_name) = 0;
        virtual error CreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension, 
            const GraphicFormat& fmt) = 0;
        virtual error CreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension, 
            unsigned int buff_count, const std::vector<GraphicFormat>& fmts) = 0;
        virtual error CreateDepthStencilSurface(const std::string& depth_name, const MathLib::Dimension& dimension, 
            const GraphicFormat& fmt) = 0;
        virtual error ShareDepthStencilSurface(const std::string& depth_name, const IDepthStencilSurfacePtr& from_depth) = 0;
        virtual error ClearSurface(const IBackSurfacePtr& back_surface, const IDepthStencilSurfacePtr& depth_surface,
            const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value) = 0;
        virtual future_error AsyncCreatePrimaryBackSurface(
            const std::string& back_name, const std::string& depth_name);
        virtual future_error AsyncCreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension,
            const GraphicFormat& fmt);
        virtual future_error AsyncCreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension, 
            unsigned int buff_count, const std::vector<GraphicFormat>& fmts);
        virtual future_error AsyncCreateDepthStencilSurface(const std::string& depth_name, const MathLib::Dimension& dimension, 
            const GraphicFormat& fmt);
        virtual future_error AsyncShareDepthStencilSurface(const std::string& depth_name, const IDepthStencilSurfacePtr& from_depth);
        virtual future_error AsyncClearSurface(
            const IBackSurfacePtr& back_surface, const IDepthStencilSurfacePtr& depth_surface,
            const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value);
        //@}

        virtual error BindBackSurface(
            const IBackSurfacePtr& back_surface, const IDepthStencilSurfacePtr& depth_surface) = 0;
        virtual future_error AsyncBindBackSurface(
            const IBackSurfacePtr& back_surface, const IDepthStencilSurfacePtr& depth_surface);
        virtual IBackSurfacePtr CurrentBoundBackSurface() { return m_boundBackSurface; }
        virtual IDepthStencilSurfacePtr CurrentBoundDepthSurface() { return m_boundDepthSurface; }
        virtual error BindViewPort(const TargetViewPort& vp) = 0;
        virtual future_error AsyncBindViewPort(const TargetViewPort& vp);


        virtual void TerminateGraphicThread();
        virtual GraphicThread* GetGraphicThread();

        /** Get graphic asset, assert if key not found */
        template <class T> T GetGraphicAsset(const std::string& asset_key)
        {
            return m_repository->Get<T>(asset_key);
        }

        /** Try get asset, return nullopt if key not found.
         *  return value is the copy.
         **/
        template <class T> std::optional<T> TryGetGraphicAsset(const std::string& asset_key)
        {
            return m_repository->TryGetValue<T>(asset_key);
        }

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
        AssetRepository* m_repository;

            IBackSurfacePtr m_boundBackSurface;
        IDepthStencilSurfacePtr m_boundDepthSurface;
        TargetViewPort m_boundViewPort;
    };
}

#endif // GRAPHIC_API_INTERFACE_H
