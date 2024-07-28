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
#include "SceneGraph/SpatialId.h"
#include "Renderables/MeshPrimitive.h"
#include "Frameworks/EventSubscriber.h"
#include "SceneGraph/Light.h"
#include "SceneGraph/LightEvents.h"
#include "SceneGraph/SceneGraphRepository.h"
#include "Frameworks/CommandSubscriber.h"
#include "GameEngine/EffectTextureMapAssembler.h"
#include <memory>
#include <unordered_map>

namespace Enigma::GameCommon
{
    class LightingPawn;
    class LightVolumePawn;
    class DeferredRendererServiceConfiguration;
    class LightingPawnRepository;

    class DeferredRendererService : public SceneRendererService
    {
        DECLARE_EN_RTTI
    public:
        DeferredRendererService(Frameworks::ServiceManager* mngr, const std::shared_ptr<GameSceneService>& scene_service,
            const std::shared_ptr<GameCameraService>& camera_service,
            const std::shared_ptr<Renderer::RendererManager>& renderer_manager,
            const std::shared_ptr<DeferredRendererServiceConfiguration>& configuration);
        DeferredRendererService(const DeferredRendererService&) = delete;
        DeferredRendererService(DeferredRendererService&&) = delete;
        virtual ~DeferredRendererService() override;
        DeferredRendererService& operator=(DeferredRendererService&) = delete;
        DeferredRendererService& operator=(DeferredRendererService&&) = delete;

        virtual Frameworks::ServiceResult onInit() override;
        virtual Frameworks::ServiceResult onTerm() override;

        virtual void createSceneRenderSystem(const std::string& renderer_name, const std::string& target_name) override;
        virtual void destroySceneRenderSystem(const std::string& renderer_name, const std::string& target_name) override;

        virtual void prepareGameScene() override;

    private:
        void createGBuffer(const Renderer::RenderTargetPtr& primary_target);

        //void createAmbientLightQuad(const std::shared_ptr<SceneGraph::Light>& lit);
        //void createSunLightQuad(const std::shared_ptr<SceneGraph::Light>& lit);
        //void createPointLightVolume(const std::shared_ptr<SceneGraph::Light>& lit);
        //void removeLightingPawn(const SceneGraph::SpatialId& lit_id);
        //void updateAmbientLightQuad(const std::shared_ptr<SceneGraph::Light>& lit, SceneGraph::LightInfoUpdated::NotifyCode notify);
        //void updateSunLightQuad(const std::shared_ptr<SceneGraph::Light>& lit, SceneGraph::LightInfoUpdated::NotifyCode notify);
        //void updatePointLightVolume(const std::shared_ptr<SceneGraph::Light>& lit, SceneGraph::LightInfoUpdated::NotifyCode notify);

        void bindGBufferToPendingLights();

        void onPrimaryRenderTargetCreated(const Frameworks::IEventPtr& e);
        void onPrimaryRenderTargetResized(const Frameworks::IEventPtr& e);
        //void onGameCameraUpdated(const Frameworks::IEventPtr& e);
        void onSceneGraphChanged(const Frameworks::IEventPtr& e);

        void onLightCreatedOrConstituted(const Frameworks::IEventPtr& e);
        void onLightInfoDeleted(const Frameworks::IEventPtr& e);
        //void onLightInfoUpdated(const Frameworks::IEventPtr& e);

        void requestAmbientLightMeshAssembly(const Frameworks::IQueryPtr& q);
        void requestSunLightMeshAssembly(const Frameworks::IQueryPtr& q);
        void requestPointLightMeshAssembly(const Frameworks::IQueryPtr& q);

        void completeLightingPawnBuilt(const SceneGraph::SpatialId& lit_id, const std::shared_ptr<LightingPawn>& lighting_pawn);

        std::shared_ptr<Renderables::MeshPrimitive> assembleAmbientLightMesh(const Primitives::PrimitiveId& mesh_id);
        std::shared_ptr<Renderables::MeshPrimitive> assembleSunLightMesh(const Primitives::PrimitiveId& mesh_id);
        std::shared_ptr<Renderables::MeshPrimitive> assemblePointLightMesh(const Primitives::PrimitiveId& mesh_id, float sphere_radius);

        std::shared_ptr<LightingPawn> findLightingPawn(const SceneGraph::SpatialId& lit_id);

        //void checkLightVolumeBackfaceCulling(const SceneGraph::SpatialId& lit_id);
        //void checkLightVolumeBackfaceCulling(const std::shared_ptr<LightVolumePawn>& lit_vol, const std::shared_ptr<SceneGraph::Camera>& cam);

        Engine::EffectTextureMapAssembler getGBufferTextureSemantics();

    private:
        std::shared_ptr<DeferredRendererServiceConfiguration> m_configuration;

        std::shared_ptr<LightingPawnRepository> m_lightingPawns;

        std::weak_ptr<Renderer::RenderTarget> m_gBuffer;

        std::vector<std::shared_ptr<SceneGraph::Light>> m_pendingLightsOfGBufferBind;

        Frameworks::EventSubscriberPtr m_onPrimaryRenderTargetCreated;
        Frameworks::EventSubscriberPtr m_onPrimaryRenderTargetResized;
        //Frameworks::EventSubscriberPtr m_onGameCameraUpdated;
        Frameworks::EventSubscriberPtr m_onNodeChildAttached;
        Frameworks::EventSubscriberPtr m_onNodeChildAttachmentFailed;;
        Frameworks::EventSubscriberPtr m_onNodeChildDetached;
        Frameworks::EventSubscriberPtr m_onNodeChildDetachmentFailed;
        Frameworks::EventSubscriberPtr m_onSceneSpatialDeleted;

        Frameworks::EventSubscriberPtr m_onLightCreated;
        Frameworks::EventSubscriberPtr m_onLightConstituted;
        Frameworks::EventSubscriberPtr m_onLightInfoDeleted;
        //Frameworks::EventSubscriberPtr m_onLightInfoUpdated;

        Frameworks::QuerySubscriberPtr m_requestAmbientLightMeshAssembly;
        Frameworks::QuerySubscriberPtr m_requestSunLightMeshAssembly;
        Frameworks::QuerySubscriberPtr m_requestPointLightMeshAssembly;
    };
}
#endif // DEFERRED_RENDERER_SERVICE_H
