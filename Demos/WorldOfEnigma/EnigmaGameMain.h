/*********************************************************************
 * \file   EnigmaGameMain.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef ENIGMA_GAME_MAIN_H
#define ENIGMA_GAME_MAIN_H

#include <string>
#include "Application/AppDelegateWin32.h"
#include "GameCommon/SceneRendererService.h"
#include "ShadowMap/ShadowMapService.h"

namespace EnigmaGame
{
    class AppConfiguration;

    class EnigmaGameMain : public Enigma::Application::AppDelegate
    {
    public:
        static inline std::string MediaPathName = "APK_PATH";
    public:
        EnigmaGameMain(const std::string& app_name);
        EnigmaGameMain(const EnigmaGameMain&) = delete;
        EnigmaGameMain(EnigmaGameMain&&) = delete;
        ~EnigmaGameMain() override = default;
        EnigmaGameMain& operator=(const EnigmaGameMain&) = delete;
        EnigmaGameMain& operator=(EnigmaGameMain&&) = delete;

        void InitializeMountPaths() override;
        void InstallEngine() final;
        void ShutdownEngine() final;
        void FrameUpdate() override;
        void RenderFrame() override;

    private:
        std::weak_ptr<Enigma::GameCommon::SceneRendererService> m_sceneRendererService;
        std::weak_ptr<Enigma::ShadowMap::ShadowMapService> m_shadowMapService;

        std::unique_ptr<AppConfiguration> m_appConfig;
    };
}

#endif // ENIGMA_GAME_MAIN_H
