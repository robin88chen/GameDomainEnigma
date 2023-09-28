/*********************************************************************
 * \file   BattleArenaMain.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef BATTLE_ARENA_MAIN_H
#define BATTLE_ARENA_MAIN_H

#include <string>
#include <memory>
#include "Platforms/PlatformConfig.h"
#if TARGET_PLATFORM == PLATFORM_WIN32
#include "Application/AppDelegateWin32.h"
#elif TARGET_PLATFORM == PLATFORM_ANDROID
#include "Application/AppDelegateAndroid.h"
#endif
#include "ShadowMap/ShadowMapService.h"
#include "GameCommon/SceneRendererService.h"

namespace BattleArena
{
    class AppConfiguration;
    class BattleArenaMain : public Enigma::Application::AppDelegate
    {
    public:
        BattleArenaMain(const std::string& app_name);
        BattleArenaMain(const BattleArenaMain&) = delete;
        BattleArenaMain(BattleArenaMain&&) = delete;
        ~BattleArenaMain() override = default;
        BattleArenaMain& operator=(const BattleArenaMain&) = delete;
        BattleArenaMain& operator=(BattleArenaMain&&) = delete;

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

#endif // BATTLE_ARENA_MAIN_H
