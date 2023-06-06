/********************************************************************
 * \file   RenderTarget.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include "RenderTargetClearingProperties.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "Frameworks/CommandSubscriber.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/ResponseSubscriber.h"
#include "GraphicKernel/TargetViewPort.h"
#include "GraphicKernel/RenderTextureUsage.h"
#include "MathLib/AlgebraBasicTypes.h"
#include "MathLib/ColorRGBA.h"
#include "GraphicKernel/IGraphicAPI.h"
#include <string>
#include <memory>
#include <system_error>
#include <bitset>

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
    class Texture;
    using TexturePtr = std::shared_ptr<Texture>;
}

namespace Enigma::Renderer
{
    using error = std::error_code;
    /** Render Target class */
    class RenderTarget : public std::enable_shared_from_this<RenderTarget>
    {
    public:
        enum class PrimaryType : bool
        {
            IsPrimary = true,
            NotPrimary = false
        };
    protected:
        enum class ResizingBitIndex
        {
            BackSurface,
            DepthSurface,
            Count
        };
    public:
        RenderTarget(const std::string& name, PrimaryType primary, const std::vector<Graphics::RenderTextureUsage>& usages);
        RenderTarget(const std::string& name, const std::vector<Graphics::RenderTextureUsage>& usages);
        RenderTarget(const RenderTarget&) = delete;
        RenderTarget(RenderTarget&&) = delete;
        virtual ~RenderTarget();
        RenderTarget& operator=(const RenderTarget&) = delete;
        RenderTarget& operator=(RenderTarget&&) = delete;

        /** init Back-Buffer */
        error InitBackSurface(const std::string& back_name, const MathLib::Dimension& dimension,
            const Graphics::GraphicFormat& fmt);
        error InitMultiBackSurface(const std::string& back_name, const MathLib::Dimension& dimension,
            unsigned int surface_count, const std::vector<Graphics::GraphicFormat>& fmts);

        /** get back buffer interface */
        Graphics::IBackSurfacePtr GetBackSurface() { return m_backSurface; };

        /** init DepthStencil Buffer */
        error InitDepthStencilSurface(const std::string& depth_name, const MathLib::Dimension& dimension,
            const Graphics::GraphicFormat& fmt);
        /** share DepthStencil Buffer */
        error ShareDepthStencilSurface(const std::string& depth_name,
            const Graphics::IDepthStencilSurfacePtr& surface);

        /** get depth stencil buffer */
        Graphics::IDepthStencilSurfacePtr GetDepthStencilSurface() { return m_depthStencilSurface; };

        void SetViewPort(const Graphics::TargetViewPort& vp);
        /** get viewport */
        const Graphics::TargetViewPort& GetViewPort();

        /** is primary render target?? */
        bool IsPrimary() const { return m_isPrimary; };
        /** bind to device */
        error Bind();
        /** bind viewport */
        error BindViewPort();
        /** clear render target */
        error Clear();
        /** flip, only primary render target can flip */
        error Flip() const;

        error ChangeClearingProperty(const RenderTargetClearChangingProperty& prop);
        /** get name */
        const std::string& GetName() { return m_name; };

        /** get dimension. */
        const MathLib::Dimension& GetDimension() const { return m_dimension; };

        /** get render target texture */
        Engine::TexturePtr GetRenderTargetTexture() { return m_renderTargetTexture; };

        /** resize target */
        error Resize(const MathLib::Dimension& dimension);

        std::optional<unsigned> FindRenderTextureUsageIndex(Graphics::RenderTextureUsage usage) const;

    protected:
        void SubscribeHandler();
        void UnsubscribeHandler();

        error Initialize();
        future_error AsyncInitialize();

        void CreateRenderTargetTexture();
        void InitViewPortSize();

        error Clear(const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value,
            RenderTargetClearingBits flag = RenderTargetClear::BothBuffer) const;

        /** @name event handler */
        //@{
        void OnPrimarySurfaceCreated(const Frameworks::IEventPtr& e);
        void OnBackSurfaceCreated(const Frameworks::IEventPtr& e);
        void OnDepthSurfaceCreated(const Frameworks::IEventPtr& e);
        void OnBackSurfaceResized(const Frameworks::IEventPtr& e);
        void OnDepthSurfaceResized(const Frameworks::IEventPtr& e);
        //@}
        void OnCreateTextureResponse(const Frameworks::IResponsePtr& r);

    protected:
        bool m_isPrimary;

        std::string m_name;
        MathLib::Dimension m_dimension;
        std::vector<Graphics::RenderTextureUsage> m_usages;

        Graphics::IBackSurfacePtr m_backSurface;
        std::string m_backSurfaceName;
        Graphics::IDepthStencilSurfacePtr m_depthStencilSurface;
        std::string m_depthSurfaceName;
        Engine::TexturePtr m_renderTargetTexture;

        Graphics::TargetViewPort m_viewPort;
        RenderTargetClearingProperty m_clearingProperty;

        Frameworks::EventSubscriberPtr m_onPrimarySurfaceCreated;
        Frameworks::EventSubscriberPtr m_onBackSurfaceCreated;
        Frameworks::EventSubscriberPtr m_onDepthSurfaceCreated;
        Frameworks::EventSubscriberPtr m_onBackSurfaceResized;
        Frameworks::EventSubscriberPtr m_onDepthSurfaceResized;

        Frameworks::ResponseSubscriberPtr m_onCreateTextureResponse;

        enum Resizing  //! 不能用 enum class, bitsets 操作會有問題
        {
            BackSurfaceBit = 0x01,
            DepthSurfaceBit = 0x02
        };
        using ResizingBits = std::bitset<static_cast<size_t>(ResizingBitIndex::Count)>;
        ResizingBits m_resizingBits;
        //const ResizingBits ResizingBackSurfaceBit{ "01" };
        //const ResizingBits ResizingDepthSurfaceBit{ "10" };
    };
    using RenderTargetPtr = std::shared_ptr<RenderTarget>;
};

#endif // RENDER_TARGET_H
