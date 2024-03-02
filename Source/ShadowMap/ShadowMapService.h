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
#include "GameEngine/EffectVariable.h"
#include "Renderables/MeshPrimitive.h"

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
            const std::shared_ptr<ShadowMapServiceConfiguration>& configuration);
        ShadowMapService(const ShadowMapService& rhs) = delete;
        ShadowMapService(ShadowMapService&& rhs) = delete;
        virtual ~ShadowMapService() override;
        ShadowMapService& operator=(const ShadowMapService& rhs) = delete;
        ShadowMapService& operator=(ShadowMapService&& rhs) = delete;

        virtual Frameworks::ServiceResult onInit() override;
        virtual Frameworks::ServiceResult onTick() override;
        virtual Frameworks::ServiceResult onTerm() override;

        virtual void createShadowRenderSystem(const std::string& renderer_name, const std::string& target_name);
        virtual void destroyShadowRenderSystem(const std::string& renderer_name, const std::string& target_name);

        virtual void prepareShadowScene();
        virtual void renderShadowScene();

    protected:
        void subscribeEvents();
        void unsubscribeEvents();

        void onLightInfoCreated(const Frameworks::IEventPtr& e);
        void onLightInfoDeleted(const Frameworks::IEventPtr& e);
        void onLightInfoUpdated(const Frameworks::IEventPtr& e);
        void onPawnConstituted(const Frameworks::IEventPtr& e);

        virtual void createSunLightCamera(const std::shared_ptr<SceneGraph::Light>& lit);
        virtual void deleteSunLightCamera();
        virtual void updateSunLightDirection(const MathLib::Vector3& dir);

        void bindShadowMapToPawn(const std::shared_ptr<SceneGraph::Pawn>& pawn);
        void bindShadowMapToMesh(const std::shared_ptr<Renderables::MeshPrimitive>& mesh);

        static void assignShadowMapDimension(Engine::EffectVariable& var);

    private:
        static void assignLightViewProjectionTransform(Engine::EffectVariable& var);

    private:
        std::shared_ptr<ShadowMapServiceConfiguration> m_configuration;
        std::shared_ptr<SunLightCamera> m_sunLightCamera;
        static inline MathLib::Matrix4 m_lightViewProjectionTransform;

    protected:
        std::weak_ptr<GameCommon::GameSceneService> m_sceneService;
        std::weak_ptr<GameCommon::GameCameraService> m_cameraService;
        std::weak_ptr<Renderer::RendererManager> m_rendererManager;
        std::weak_ptr<Renderer::Renderer> m_renderer;
        std::weak_ptr<Renderer::RenderTarget> m_shadowMapRenderTarget;

        static inline MathLib::Vector4 m_shadowMapDimensionBiasDensity =
        { 512.0f, 512.0f, 0.005f, 0.75f };      //< width, height, depth bias, shadow density; 靜態變數不能用 union

        Frameworks::EventSubscriberPtr m_onLightInfoCreated;
        Frameworks::EventSubscriberPtr m_onLightInfoDeleted;
        Frameworks::EventSubscriberPtr m_onLightInfoUpdated;
        Frameworks::EventSubscriberPtr m_onPawnConstituted;
    };
}

#endif // _SHADOW_MAP_SERVICE_H
