/*********************************************************************
 * \file   SceneRendererService.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _SCENE_RENDERER_SERVICE_H
#define _SCENE_RENDERER_SERVICE_H

#include "Frameworks/SystemService.h"
#include "Renderer/RendererManager.h"
#include "Renderer/Renderer.h"
#include "Frameworks/EventSubscriber.h"
#include "SceneRendererServiceConfiguration.h"
#include <memory>

namespace Enigma::GameCommon
{
    class GameSceneService;
    class GameCameraService;

    class SceneRendererService : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        SceneRendererService(Frameworks::ServiceManager* mngr, const std::shared_ptr<GameSceneService>& scene_service,
            const std::shared_ptr<GameCameraService>& camera_service,
            const std::shared_ptr<Renderer::RendererManager>& renderer_manager, const std::shared_ptr<SceneRendererServiceConfiguration>& config);
        SceneRendererService(const SceneRendererService&) = delete;
        SceneRendererService(SceneRendererService&&) = delete;
        virtual ~SceneRendererService() override;
        SceneRendererService& operator=(const SceneRendererService&) = delete;
        SceneRendererService& operator=(SceneRendererService&&) = delete;

        virtual Frameworks::ServiceResult onInit() override;
        virtual Frameworks::ServiceResult onTerm() override;

        virtual void CreateSceneRenderSystem(const std::string& renderer_name, const std::string& target_name);
        virtual void DestroySceneRenderSystem(const std::string& renderer_name, const std::string& target_name);

        virtual void PrepareGameScene();
        virtual void RenderGameScene();
        virtual void Flip();

    protected:
        void AttachCamera();

        void OnPrimaryCameraCreated(const Frameworks::IEventPtr& e);
        void OnPrimaryTargetCreated(const Frameworks::IEventPtr& e);

    private:
        std::shared_ptr<SceneRendererServiceConfiguration> m_config;

    protected:
        std::weak_ptr<GameSceneService> m_sceneService;
        std::weak_ptr<GameCameraService> m_cameraService;
        std::weak_ptr<Renderer::RendererManager> m_rendererManager;
        std::weak_ptr<Renderer::Renderer> m_renderer;

        Frameworks::EventSubscriberPtr m_onPrimaryCameraCreated;
        Frameworks::EventSubscriberPtr m_onPrimaryTargetCreated;
    };
}

#endif // _SCENE_RENDERER_SERVICE_H
