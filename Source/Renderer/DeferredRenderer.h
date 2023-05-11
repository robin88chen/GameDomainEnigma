/*********************************************************************
 * \file   DeferredRenderer.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef DEFERRED_RENDERER_H
#define DEFERRED_RENDERER_H

#include "Renderer.h"

namespace Enigma::Renderer
{
    class DeferredRenderer : public Renderer
    {
    public:
        DeferredRenderer(const std::string& name);
        DeferredRenderer(const DeferredRenderer&) = delete;
        DeferredRenderer(DeferredRenderer&&) = delete;
        virtual ~DeferredRenderer() override;
        DeferredRenderer& operator=(const DeferredRenderer&) = delete;
        DeferredRenderer& operator=(DeferredRenderer&&) = delete;

        virtual error ClearRenderTarget() override;
        virtual error ChangeClearingProperty(const RenderTargetClearChangingProperty& prop) override;

        virtual error BeginScene() override;
        virtual error BeginScene(const MathLib::Vector3& camera_loc, const MathLib::Matrix4& mxView, const MathLib::Matrix4& mxProj) override;
        virtual error DrawScene() override;

        error AttachGBufferTarget(const std::shared_ptr<RenderTarget>& gbuffer);

        /// GBuffer render target 與 scene render target 共用一個depth buffer, 當 scene render target被改掉後, 需要重新設定分享
        void ReShareDepthStencilSurface();

    private:
        std::weak_ptr<RenderTarget> m_gbufferTarget;
        MathLib::ColorRGBA m_gbufferClearColor;
    };
}
#endif // DEFERRED_RENDERER_H
