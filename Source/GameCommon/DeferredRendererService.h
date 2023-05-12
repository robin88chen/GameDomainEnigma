/*********************************************************************
 * \file   DeferredRendererService.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef DEFERRED_RENDERER_SERVICE_H
#define DEFERRED_RENDERER_SERVICE_H

#include "SceneRendererService.h"
#include "Renderer/MeshPrimitive.h"
#include "SceneGraph/SpatialRenderState.h"
#include <memory>
#include <unordered_map>

namespace Enigma::GameCommon
{
    class LightVolumePawn;
    class DeferredRendererServiceConfiguration;

    class DeferredRendererService : public SceneRendererService
    {
        DECLARE_EN_RTTI
    public:
        DeferredRendererService(Frameworks::ServiceManager* mngr, const std::shared_ptr<GameSceneService>& scene_service,
            const std::shared_ptr<GameCameraService>& camera_service,
            const std::shared_ptr<Renderer::RendererManager>& renderer_manager,
            std::unique_ptr<DeferredRendererServiceConfiguration> configuration);
        DeferredRendererService(const DeferredRendererService&) = delete;
        DeferredRendererService(DeferredRendererService&&) = delete;
        virtual ~DeferredRendererService() override;
        DeferredRendererService& operator=(DeferredRendererService&) = delete;
        DeferredRendererService& operator=(DeferredRendererService&&) = delete;

        virtual Frameworks::ServiceResult OnInit() override;
        virtual Frameworks::ServiceResult OnTerm() override;

        virtual void CreateSceneRenderSystem(const std::string& renderer_name, const std::string& target_name, bool is_primary) override;
        virtual void DestroySceneRenderSystem(const std::string& renderer_name, const std::string& target_name) override;

    private:
        Renderer::RenderTargetPtr CreateGBuffer(unsigned int width, unsigned int height, const Graphics::IDepthStencilSurfacePtr& depth) const;

    private:
        std::unique_ptr<DeferredRendererServiceConfiguration> m_configuration;

        Renderer::MeshPrimitivePtr m_ambientLightQuad;
        Renderer::MeshPrimitivePtr m_sunLightQuad;
        SceneGraph::SpatialRenderState m_ambientQuadRenderState;
        SceneGraph::SpatialRenderState m_sunLightQuadRenderState;

        using LightVolumeMap = std::unordered_map<std::string, std::shared_ptr<LightVolumePawn>>;
        LightVolumeMap m_lightVolumes;
    };
}
#endif // DEFERRED_RENDERER_SERVICE_H
