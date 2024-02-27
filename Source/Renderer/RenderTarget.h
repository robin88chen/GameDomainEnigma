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
#include "Frameworks/EventSubscriber.h"
#include "GraphicKernel/TargetViewPort.h"
#include "GraphicKernel/RenderTextureUsage.h"
#include "GraphicKernel/BackSurfaceSpecification.h"
#include "GraphicKernel/DepthStencilSurfaceSpecification.h"
#include "MathLib/AlgebraBasicTypes.h"
#include "MathLib/ColorRGBA.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "GameEngine/TextureId.h"
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
        RenderTarget(const std::string& name, const Engine::TextureId& render_tex_id, const Graphics::BackSurfaceSpecification& back_specification, const Graphics::DepthStencilSurfaceSpecification& depth_specification, const std::vector<Graphics::RenderTextureUsage>& usages);
        RenderTarget(const std::string& name, const Engine::TextureId& render_tex_id, const Graphics::MultiBackSurfaceSpecification& back_specification, const std::string& depth_name, const Graphics::IDepthStencilSurfacePtr& depth_surface, const std::vector<Graphics::RenderTextureUsage>& usages);
        RenderTarget(const RenderTarget&) = delete;
        RenderTarget(RenderTarget&&) = delete;
        virtual ~RenderTarget();
        RenderTarget& operator=(const RenderTarget&) = delete;
        RenderTarget& operator=(RenderTarget&&) = delete;

        /** get back buffer interface */
        Graphics::IBackSurfacePtr getBackSurface() { return m_backSurface; };

        /** share DepthStencil Buffer */
        error shareDepthStencilSurface(const std::string& depth_name,
            const Graphics::IDepthStencilSurfacePtr& surface);

        /** get depth stencil buffer */
        Graphics::IDepthStencilSurfacePtr getDepthStencilSurface() { return m_depthStencilSurface; };

        void setViewPort(const Graphics::TargetViewPort& vp);
        /** get viewport */
        const Graphics::TargetViewPort& getViewPort();

        /** is primary render target?? */
        bool isPrimary() const { return m_isPrimary; };
        /** bind to device */
        error bind();
        /** bind viewport */
        error bindViewPort();
        /** clear render target */
        error clear();
        /** flip, only primary render target can flip */
        error flip() const;

        error changeClearingProperty(const RenderTargetClearChangingProperty& prop);
        /** get name */
        const std::string& getName() { return m_name; };

        /** get dimension. */
        const MathLib::Dimension<unsigned>& getDimension() const { return m_dimension; };

        /** get render target texture */
        Engine::TexturePtr getRenderTargetTexture() { return m_renderTargetTexture; };

        /** resize target */
        error resize(const MathLib::Dimension<unsigned>& dimension);

    protected:
        void subscribeHandler();
        void unsubscribeHandler();

        void createRenderTargetTexture(const Engine::TextureId& texture_id);
        void initViewPortSize();

        error clear(const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value,
            RenderTargetClearingBits flag = RenderTargetClear::BothBuffer) const;

        /** init Back-Buffer */
        error initBackSurface(const Graphics::BackSurfaceSpecification& specification);
        error initMultiBackSurface(const Graphics::MultiBackSurfaceSpecification& specification);
        /** init DepthStencil Buffer */
        error initDepthStencilSurface(const Graphics::DepthStencilSurfaceSpecification& specification);

        /** @name event handler */
        //@{
        void onPrimarySurfaceCreated(const Frameworks::IEventPtr& e);
        void onBackSurfaceCreated(const Frameworks::IEventPtr& e);
        void onDepthSurfaceCreated(const Frameworks::IEventPtr& e);
        void onBackSurfaceResized(const Frameworks::IEventPtr& e);
        void onDepthSurfaceResized(const Frameworks::IEventPtr& e);
        //@}
        void onTextureHydrated(const Frameworks::IEventPtr& e);
        void onHydrateTextureFailed(const Frameworks::IEventPtr& e);

        void completeSurfaceCreation();
        void completeRenderTextureHydration();

        std::string backSurfaceName() const;
        std::string depthSurfaceName() const;
    protected:
        bool m_isPrimary;

        std::string m_name;
        MathLib::Dimension<unsigned> m_dimension;
        std::vector<Graphics::RenderTextureUsage> m_usages;

        Graphics::IBackSurfacePtr m_backSurface;
        std::optional<Graphics::BackSurfaceSpecification> m_backSpecification;
        std::optional<Graphics::MultiBackSurfaceSpecification> m_multiBackSpecification;
        Graphics::IDepthStencilSurfacePtr m_depthStencilSurface;
        std::optional<Graphics::DepthStencilSurfaceSpecification> m_depthSpecification;
        Engine::TexturePtr m_renderTargetTexture;

        Graphics::TargetViewPort m_viewPort;
        RenderTargetClearingProperty m_clearingProperty;

        Frameworks::EventSubscriberPtr m_onPrimarySurfaceCreated;
        Frameworks::EventSubscriberPtr m_onBackSurfaceCreated;
        Frameworks::EventSubscriberPtr m_onDepthSurfaceCreated;
        Frameworks::EventSubscriberPtr m_onBackSurfaceResized;
        Frameworks::EventSubscriberPtr m_onDepthSurfaceResized;

        Frameworks::EventSubscriberPtr m_onTextureHydrated;
        Frameworks::EventSubscriberPtr m_onHydrateTextureFailed;

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
