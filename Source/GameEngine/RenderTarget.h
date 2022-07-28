/********************************************************************
 * \file   RenderTarget.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include "Frameworks/ExtentTypesDefine.h"
#include "Frameworks/CommandSubscriber.h"
#include "Frameworks/EventSubscriber.h"
#include "GraphicKernel/TargetViewPort.h"
#include "MathLib/AlgebraBasicTypes.h"
#include "MathLib/ColorRGBA.h"
#include "GraphicKernel/IGraphicAPI.h"
#include <string>
#include <memory>
#include <system_error>


namespace Enigma::Graphics
{
    struct GraphicFormat;
    class IBackSurface;
    using IBackSurfacePtr = std::shared_ptr<IBackSurface>;
    class IDepthStencilSurface;
    using IDepthStencilSurfacePtr = std::shared_ptr<IDepthStencilSurface>;
}

namespace Enigma::Engine
{
    using error = std::error_code;
    class Texture;
    using TexturePtr = std::shared_ptr<Texture>;
    /** Render Target class */
    class RenderTarget : public std::enable_shared_from_this<RenderTarget>
    {
    public:
        enum class BufferClearFlag
        {
            ColorBuffer = 0x01,
            DepthBuffer = 0x02,
            BothBuffer = 0x03,
        };
        enum class PrimaryType : bool
        {
            IsPrimary = true,
            NotPrimary = false
        };
        struct ClearingProperty
        {
            MathLib::ColorRGBA m_color;
            float m_depth;
            unsigned int m_stencil;
            BufferClearFlag m_flag;
        };
    public:
        RenderTarget(const std::string& name, PrimaryType primary);
        RenderTarget(const std::string& name);
        RenderTarget(const RenderTarget&) = delete;
        virtual ~RenderTarget();
        RenderTarget& operator=(const RenderTarget&) = delete;

        /** init Back-Buffer */
        error InitBackSurface(const std::string& back_name, const MathLib::Dimension& dimension,
            const Graphics::GraphicFormat& fmt);
        future_error AsyncInitBackSurface(const std::string& back_name, const MathLib::Dimension& dimension, 
            const Graphics::GraphicFormat& fmt);
        error InitMultiBackSurface(const std::string& back_name, const MathLib::Dimension& dimension, 
            unsigned int surface_count, const std::vector<Graphics::GraphicFormat>& fmts);
        future_error AsyncInitMultiBackSurface(const std::string& back_name, const MathLib::Dimension& dimension, 
            unsigned int surface_count, const std::vector<Graphics::GraphicFormat>& fmts);

        /** get back buffer interface */
        Graphics::IBackSurfacePtr GetBackSurface() { return m_backSurface; };

        /** init DepthStencil Buffer */
        error InitDepthStencilSurface(const std::string& depth_name, const MathLib::Dimension& dimension,
            const Graphics::GraphicFormat& fmt);
        future_error AsyncInitDepthStencilSurface(const std::string& depth_name, const MathLib::Dimension& dimension,
            const Graphics::GraphicFormat& fmt);
        /** share DepthStencil Buffer */
        error ShareDepthStencilSurface(const std::string& depth_name, 
            const Graphics::IDepthStencilSurfacePtr& surface);
        future_error AsyncShareDepthStencilSurface(const std::string& depth_name, 
            const Graphics::IDepthStencilSurfacePtr& surface);

        /** get depth stencil buffer */
        Graphics::IDepthStencilSurfacePtr GetDepthStencilSurface() { return m_depthStencilSurface; };

        /** get viewport */
        const Graphics::TargetViewPort& GetViewPort();

        /** is primary render target?? */
        bool IsPrimary() { return m_isPrimary; };
        /** bind to device */
        error Bind();
        future_error AsyncBind();
        /** bind viewport */
        error BindViewPort();
        future_error AsyncBindViewPort();
        /** clear render target */
        error Clear();
        future_error AsyncClear();
        /** flip, only primary render target can flip */
        error Flip();
        future_error AsyncFlip();

        /** get name */
        const std::string& GetName() { return m_name; };

        /** get dimension. */
        const MathLib::Dimension& GetDimension() const { return m_dimension; };

        /** get render target texture */
        TexturePtr GetRenderTargetTexture() { return m_renderTargetTexture; };

        /** resize target */
        error Resize(const MathLib::Dimension& dimension);
        future_error AsyncResize(const MathLib::Dimension& dimension);

        /** @name depth map info */
        //@{
        bool HasGBufferDepthMap();
        unsigned int GetGBufferDepthMapIndex();
        void SetGBufferDepthMapIndex(unsigned int index);
        //@}

    protected:
        void SubscribeHandler();
        void UnsubscribeHandler();

        error Initialize();
        future_error AsyncInitialize();

        void CreateRenderTargetTexture();
        void InitViewPortSize();

        error Clear(const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value,
            BufferClearFlag flag = BufferClearFlag::BothBuffer);

        void SetViewPort(const Graphics::TargetViewPort& vp);

        /** @name event handler */
        //@{
        void OnPrimarySurfaceCreated(const Frameworks::IEventPtr& e);
        void OnBackSurfaceCreated(const Frameworks::IEventPtr& e);
        void OnDepthSurfaceCreated(const Frameworks::IEventPtr& e);
        //@}
        /** @name command handler */
        //@{
        void HandleChangingViewPort(const Frameworks::ICommandPtr& c);
        void HandleChangingClearingProperty(const Frameworks::ICommandPtr& c);
        //@}

    protected:
        Graphics::IGraphicAPI::AsyncType m_async;
        bool m_isPrimary;

        std::string m_name;
        MathLib::Dimension m_dimension;

        Graphics::IBackSurfacePtr m_backSurface;
        std::string m_backSurfaceName;
        Graphics::IDepthStencilSurfacePtr m_depthStencilSurface;
        std::string m_depthSurfaceName;
        TexturePtr m_renderTargetTexture;

        Graphics::TargetViewPort m_viewPort;
        ClearingProperty m_clearingProperty;

        unsigned int m_gbufferDepthMapIndex;

        Frameworks::EventSubscriberPtr m_onPrimarySurfaceCreated;
        Frameworks::EventSubscriberPtr m_onBackSurfaceCreated;
        Frameworks::EventSubscriberPtr m_onDepthSurfaceCreated;
        Frameworks::CommandSubscriberPtr m_handleChangingViewPort;
        Frameworks::CommandSubscriberPtr m_handleChangingClearingProperty;
    };
    using RenderTargetPtr = std::shared_ptr<RenderTarget>;
};

#endif // RENDER_TARGET_H
