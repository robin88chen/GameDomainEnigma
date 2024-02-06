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
#include "SceneGraph/SceneGraphEvents.h"
#include "SceneGraph/SceneGraphRepository.h"
#include "Frameworks/CommandSubscriber.h"
#include <memory>
#include <unordered_map>

namespace Enigma::GameCommon
{
    class LightingPawn;
    class LightVolumePawn;
    class DeferredRendererServiceConfiguration;

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

        void createAmbientLightQuad(const std::shared_ptr<SceneGraph::Light>& lit);
        void createSunLightQuad(const std::shared_ptr<SceneGraph::Light>& lit);
        void createPointLightVolume(const std::shared_ptr<SceneGraph::Light>& lit);
        void removeLightingPawn(const std::string& name);
        void updateAmbientLightQuad(const std::shared_ptr<SceneGraph::Light>& lit, SceneGraph::LightInfoUpdated::NotifyCode notify);
        void updateSunLightQuad(const std::shared_ptr<SceneGraph::Light>& lit, SceneGraph::LightInfoUpdated::NotifyCode notify);
        void updatePointLightVolume(const std::shared_ptr<SceneGraph::Light>& lit, SceneGraph::LightInfoUpdated::NotifyCode notify);

        void bindGBufferToLightingMesh(const std::shared_ptr<Renderables::MeshPrimitive>& mesh);
        void bindGBufferToLightingPawn(const std::shared_ptr<LightingPawn>& volume);

        void onPrimaryRenderTargetCreated(const Frameworks::IEventPtr& e);
        void onPrimaryRenderTargetResized(const Frameworks::IEventPtr& e);
        void onGameCameraUpdated(const Frameworks::IEventPtr& e);
        void onSceneGraphChanged(const Frameworks::IEventPtr& e);
        void onGBufferTextureCreated(const Frameworks::IEventPtr& e);
        void onGameLightCreated(const Frameworks::IEventPtr& e);
        void onLightInfoDeleted(const Frameworks::IEventPtr& e);
        void onLightInfoUpdated(const Frameworks::IEventPtr& e);
        void onSceneGraphBuilt(const Frameworks::IEventPtr& e);
        void onLightingPawnCreated(const Frameworks::IEventPtr& e);
        void onPawnPrimitiveBuilt(const Frameworks::IEventPtr& e);

        void bindGBuffer(const Frameworks::ICommandPtr& c);

        void onLightingPawnBuilt(const std::string& lit_name, const std::shared_ptr<LightingPawn>& lighting_pawn);

        std::shared_ptr<LightingPawn> findLightingPawn(const std::string& name);

        void checkLightVolumeBackfaceCulling(const std::string& lit_name);
        void checkLightVolumeBackfaceCulling(const std::shared_ptr<LightVolumePawn>& lit_vol, const std::shared_ptr<SceneGraph::Camera>& cam);

        void insertLightPawnBuildingMeta(const SceneGraph::SpatialId& pawn_id, const std::shared_ptr<SceneGraph::Light>& lit);

    private:
        std::shared_ptr<DeferredRendererServiceConfiguration> m_configuration;

        using LightingPawnMap = std::unordered_map<std::string, std::weak_ptr<LightingPawn>>;
        LightingPawnMap m_lightingPawns;

        struct SceneGraphLightPawnMeta
        {
            std::string m_parentNodeName;
            MathLib::Matrix4 m_localTransform;
        };
        using SceneGraphLightPawnMetaMap = std::unordered_map<SceneGraph::SpatialId, SceneGraphLightPawnMeta, SceneGraph::SpatialId::hash>;
        SceneGraphLightPawnMetaMap m_buildingLightPawns;

        std::weak_ptr<Renderer::RenderTarget> m_gBuffer;

        Frameworks::EventSubscriberPtr m_onPrimaryRenderTargetCreated;
        Frameworks::EventSubscriberPtr m_onPrimaryRenderTargetResized;
        Frameworks::EventSubscriberPtr m_onGameCameraUpdated;
        Frameworks::EventSubscriberPtr m_onSceneGraphChanged;
        Frameworks::EventSubscriberPtr m_onGBufferTextureCreated;
        Frameworks::EventSubscriberPtr m_onGameLightCreated;
        Frameworks::EventSubscriberPtr m_onLightInfoDeleted;
        Frameworks::EventSubscriberPtr m_onLightInfoUpdated;
        Frameworks::EventSubscriberPtr m_onSceneGraphBuilt;
        Frameworks::EventSubscriberPtr m_onLightingPawnCreated;
        Frameworks::EventSubscriberPtr m_onPawnPrimitiveBuilt;

        Frameworks::CommandSubscriberPtr m_bindGBuffer;
    };
}
#endif // DEFERRED_RENDERER_SERVICE_H
