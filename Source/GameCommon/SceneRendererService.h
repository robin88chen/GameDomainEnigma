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
#include <memory>

namespace Enigma::GameCommon
{
    class GameSceneService;

    class SceneRendererService : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        SceneRendererService(Frameworks::ServiceManager* mngr, const std::shared_ptr<GameSceneService>& scene_service,
            const std::shared_ptr<Renderer::RendererManager>& renderer_manager, const std::string& renderer_name);
        SceneRendererService(const SceneRendererService&) = delete;
        SceneRendererService(SceneRendererService&&) = delete;
        virtual ~SceneRendererService() override;
        SceneRendererService& operator=(const SceneRendererService&) = delete;
        SceneRendererService& operator=(SceneRendererService&&) = delete;

        virtual Frameworks::ServiceResult OnInit() override;
        virtual Frameworks::ServiceResult OnTerm() override;

    protected:
        std::weak_ptr<GameSceneService> m_sceneService;
        std::weak_ptr<Renderer::RendererManager> m_rendererManager;
        std::weak_ptr<Engine::IRenderer> m_renderer;
        std::string m_rendererName;
    };
}

#endif // _SCENE_RENDERER_SERVICE_H
