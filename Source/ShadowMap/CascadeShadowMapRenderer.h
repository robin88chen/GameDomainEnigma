/*********************************************************************
 * \file   CascadeShadowMapRenderer.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef CASCADE_SHADOW_MAP_RENDERER_H
#define CASCADE_SHADOW_MAP_RENDERER_H

#include "GraphicKernel/TargetViewPort.h"
#include "Renderer/Renderer.h"

namespace Enigma::ShadowMap
{
    class CSMSunLightCamera;

    using error = std::error_code;

    class CascadeShadowMapRenderer : public Renderer::Renderer
    {
    public:
        CascadeShadowMapRenderer(const std::string& name);
        virtual ~CascadeShadowMapRenderer() override;
        CascadeShadowMapRenderer(const CascadeShadowMapRenderer& other) = delete;
        CascadeShadowMapRenderer(CascadeShadowMapRenderer&& other) noexcept = delete;
        CascadeShadowMapRenderer& operator=(const CascadeShadowMapRenderer& other) = delete;
        CascadeShadowMapRenderer& operator=(CascadeShadowMapRenderer&& other) noexcept = delete;

        void setRenderTargetViewPorts(const std::vector<Graphics::TargetViewPort>& view_ports);
        void setSunLightCamera(const std::shared_ptr<CSMSunLightCamera>& sun_light_camera);

        virtual error drawScene() override;

    protected:
        std::vector<Graphics::TargetViewPort> m_targetViewPorts;
        std::weak_ptr<CSMSunLightCamera> m_sunLightCamera;
    };
}

#endif // CASCADE_SHADOW_MAP_RENDERER_H
