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
    public:
        RenderTarget(const std::string& name, PrimaryType primary, Graphics::IGraphicAPI::AsyncType async);
        RenderTarget(const std::string& name,
            const Graphics::IBackSurfacePtr& back_surface,
            Graphics::IDepthStencilSurfacePtr& depth_surface, bool primary);
        RenderTarget(const RenderTarget&) = delete;
        virtual ~RenderTarget();
        RenderTarget& operator=(const RenderTarget&) = delete;

        error Initialize();
        future_error AsyncInitialize();

        /** init Back-Buffer */
        error InitBackSurface(const MathLib::Dimension& dimension, const Graphics::GraphicFormat& fmt);
        future_error AsyncInitBackSurface(const MathLib::Dimension& dimension, const Graphics::GraphicFormat& fmt);
        error InitMultiBackSurface(const MathLib::Dimension& dimension, unsigned int surface_count,
            const std::vector<Graphics::GraphicFormat>& fmts);
        future_error AsyncInitMultiBackSurface(const MathLib::Dimension& dimension, unsigned int surface_count,
            const std::vector<Graphics::GraphicFormat>& fmts);

        /** get back buffer interface */
        Graphics::IBackSurfacePtr GetBackSurface() { return m_backSurface; };

        /** init DepthStencil Buffer */
        error InitDepthStencilSurface(const MathLib::Dimension& dimension, const Graphics::GraphicFormat& fmt);
        future_error AsyncInitDepthStencilSurface(const MathLib::Dimension& dimension, const Graphics::GraphicFormat& fmt);
        /** share DepthStencil Buffer */
        error ShareDepthStencilSurface(const Graphics::IDepthStencilSurfacePtr& surface);
        future_error AsyncShareDepthStencilSurface(const Graphics::IDepthStencilSurfacePtr& surface);

        /** get depth stencil buffer */
        Graphics::IDepthStencilSurfacePtr GetDepthStencilSurface() { return m_depthStencilSurface; };

        /** set viewport */
        void SetViewPort(const Graphics::TargetViewPort& vp);
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
        error Clear(const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value,
            BufferClearFlag flag = BufferClearFlag::BothBuffer);
        future_error AsyncClear(const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value,
            BufferClearFlag flag = BufferClearFlag::BothBuffer);
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
        void CreateRenderTargetTexture();
        void InitViewPortSize();

    protected:
        bool m_isPrimary;

        std::string m_name;
        MathLib::Dimension m_dimension;

        Graphics::IBackSurfacePtr m_backSurface;
        Graphics::IDepthStencilSurfacePtr m_depthStencilSurface;
        TexturePtr m_renderTargetTexture;

        Graphics::TargetViewPort m_viewPort;

        unsigned int m_gbufferDepthMapIndex;
    };
    using RenderTargetPtr = std::shared_ptr<RenderTarget>;
};

#endif // RENDER_TARGET_H
