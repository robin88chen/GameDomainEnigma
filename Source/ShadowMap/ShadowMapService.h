/*********************************************************************
 * \file   ShadowMapService.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef _SHADOW_MAP_SERVICE_H
#define _SHADOW_MAP_SERVICE_H

#include "Frameworks/SystemService.h"
#include "GameCommon/GameSceneService.h"
#include "Renderer/Renderer.h"
#include "Renderer/RendererManager.h"

namespace Enigma::ShadowMap
{
    class SunLightCamera;
    class ShadowMapServiceConfiguration;

    class ShadowMapService : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        ShadowMapService(Frameworks::ServiceManager* manager, const std::shared_ptr<GameCommon::GameSceneService>& scene_service,
            const std::shared_ptr<GameCommon::GameCameraService>& camera_service,
            const std::shared_ptr<Renderer::RendererManager>& renderer_manager,
            std::unique_ptr<ShadowMapServiceConfiguration> configuration);
        ShadowMapService(const ShadowMapService& rhs) = delete;
        ShadowMapService(ShadowMapService&& rhs) = delete;
        virtual ~ShadowMapService() override;
        ShadowMapService& operator=(const ShadowMapService& rhs) = delete;
        ShadowMapService& operator=(ShadowMapService&& rhs) = delete;

        virtual Frameworks::ServiceResult OnInit() override;
        virtual Frameworks::ServiceResult OnTick() override;
        virtual Frameworks::ServiceResult OnTerm() override;

        virtual void CreateShadowRenderSystem(const std::string& renderer_name, const std::string& target_name);
        virtual void DestroyShadowRenderSystem(const std::string& renderer_name, const std::string& target_name);

        virtual void PrepareShadowScene();
        virtual void RenderShadowScene();

    protected:
        void OnLightInfoCreated(const Frameworks::IEventPtr& e);
        void OnLightInfoDeleted(const Frameworks::IEventPtr& e);
        void OnLightInfoUpdated(const Frameworks::IEventPtr& e);
        void OnPawnPrimitiveBuilt(const Frameworks::IEventPtr& e);

        void CreateSunLightCamera(const std::shared_ptr<SceneGraph::Light>& lit);

        void BindShadowMapToPawn(const std::shared_ptr<SceneGraph::Pawn>& pawn);
        void BindShadowMapToMesh(const std::shared_ptr<Renderer::MeshPrimitive>& mesh);

        static void AssignLightViewProjectionTransform(Engine::EffectVariable& var);

    protected:
        std::unique_ptr<ShadowMapServiceConfiguration> m_configuration;

        std::weak_ptr<GameCommon::GameSceneService> m_sceneService;
        std::weak_ptr<GameCommon::GameCameraService> m_cameraService;
        std::weak_ptr<Renderer::RendererManager> m_rendererManager;
        std::weak_ptr<Renderer::Renderer> m_renderer;
        std::weak_ptr<Renderer::RenderTarget> m_shadowMapRenderTarget;

        std::shared_ptr<SunLightCamera> m_sunLightCamera;
        static MathLib::Matrix4 m_lightViewProjectionTransform;

        Frameworks::EventSubscriberPtr m_onLightInfoCreated;
        Frameworks::EventSubscriberPtr m_onLightInfoDeleted;
        Frameworks::EventSubscriberPtr m_onLightInfoUpdated;
        Frameworks::EventSubscriberPtr m_onPawnPrimitiveBuilt;
    };
}

#endif // _SHADOW_MAP_SERVICE_H
