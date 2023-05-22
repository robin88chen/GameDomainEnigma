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
#include "Frameworks/EventSubscriber.h"
#include "SceneGraph/LightInfo.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "Frameworks/ResponseSubscriber.h"
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

        virtual void PrepareGameScene() override;

    private:
        void CreateGBuffer(const Renderer::RenderTargetPtr& primary_target);

        void CreateAmbientLightQuad(const SceneGraph::LightInfo& lit);
        void CreateSunLightQuad(const SceneGraph::LightInfo& lit);
        void CreatePointLightVolume(const SceneGraph::LightInfo& lit);
        void DeletePointLightVolume(const std::string& name);
        void UpdateAmbientLightQuad(const SceneGraph::LightInfo& lit, SceneGraph::LightInfoUpdated::NotifyCode notify);
        void UpdateSunLightQuad(const SceneGraph::LightInfo& lit, SceneGraph::LightInfoUpdated::NotifyCode notify);
        void UpdatePointLightVolume(const SceneGraph::LightInfo& lit, SceneGraph::LightInfoUpdated::NotifyCode notify);

        void BindGBufferToLightingQuadMesh(const Renderer::MeshPrimitivePtr& mesh);

        void OnPrimaryRenderTargetCreated(const Frameworks::IEventPtr& e);
        void OnGBufferTextureCreated(const Frameworks::IEventPtr& e);
        void OnLightInfoCreated(const Frameworks::IEventPtr& e);
        void OnLightInfoDeleted(const Frameworks::IEventPtr& e);
        void OnLightInfoUpdated(const Frameworks::IEventPtr& e);

        void OnBuildPrimitiveResponse(const Frameworks::IResponsePtr& response);

    private:
        std::unique_ptr<DeferredRendererServiceConfiguration> m_configuration;

        Renderer::MeshPrimitivePtr m_ambientLightQuad;
        Renderer::MeshPrimitivePtr m_sunLightQuad;
        Engine::RenderLightingState m_ambientQuadLightingState;
        SceneGraph::SpatialRenderState m_sunLightQuadRenderState;

        using LightVolumeMap = std::unordered_map<std::string, std::shared_ptr<LightVolumePawn>>;
        LightVolumeMap m_lightVolumes;

        Renderer::RenderTargetPtr m_gBuffer;

        Frameworks::EventSubscriberPtr m_onPrimaryRenderTargetCreated;
        Frameworks::EventSubscriberPtr m_onGBufferTextureCreated;
        Frameworks::EventSubscriberPtr m_onLightInfoCreated;
        Frameworks::EventSubscriberPtr m_onLightInfoDeleted;
        Frameworks::EventSubscriberPtr m_onLightInfoUpdated;

        Frameworks::ResponseSubscriberPtr m_onBuildPrimitiveResponse;

        Frameworks::Ruid m_ambientQuadRequester;
    };
}
#endif // DEFERRED_RENDERER_SERVICE_H
