/*********************************************************************
 * \file   SceneRendering.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _SCENE_RENDERING_H
#define _SCENE_RENDERING_H

#include "Frameworks/SystemService.h"
#include "Renderer/RendererManager.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderingConfiguration.h"
#include "Frameworks/EventSubscriber.h"
#include <memory>

namespace Enigma::GameCommon
{
    class GameSceneService;
    class GameCameraService;
};

namespace Enigma::Rendering
{
    class SceneRenderingConfiguration;

    class SceneRendering : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        SceneRendering(Frameworks::ServiceManager* mngr, const std::shared_ptr<GameCommon::GameSceneService>& scene_service,
            const std::shared_ptr<GameCommon::GameCameraService>& camera_service,
            const std::shared_ptr<Renderer::RendererManager>& renderer_manager, const std::shared_ptr<Renderer::IRenderingConfiguration>& config);
        SceneRendering(const SceneRendering&) = delete;
        SceneRendering(SceneRendering&&) = delete;
        virtual ~SceneRendering() override;
        SceneRendering& operator=(const SceneRendering&) = delete;
        SceneRendering& operator=(SceneRendering&&) = delete;

        virtual Frameworks::ServiceResult onInit() override;
        virtual Frameworks::ServiceResult onTerm() override;

        virtual void createSceneRenderSystem(const std::string& renderer_name, const std::string& target_name);
        virtual void destroySceneRenderSystem(const std::string& renderer_name, const std::string& target_name);

        virtual void prepareGameScene();
        virtual void renderGameScene();
        virtual void flip();

    protected:
        void attachCamera();

        void onPrimaryCameraCreated(const Frameworks::IEventPtr& e);
        void onPrimaryTargetCreated(const Frameworks::IEventPtr& e);

    private:
        std::shared_ptr<SceneRenderingConfiguration> m_config;

    protected:
        std::weak_ptr<GameCommon::GameSceneService> m_sceneService;
        std::weak_ptr<GameCommon::GameCameraService> m_cameraService;
        std::weak_ptr<Renderer::RendererManager> m_rendererManager;
        std::weak_ptr<Renderer::Renderer> m_renderer;

        Frameworks::EventSubscriberPtr m_onPrimaryCameraCreated;
        Frameworks::EventSubscriberPtr m_onPrimaryTargetCreated;
    };
}

#endif // _SCENE_RENDERING_H
