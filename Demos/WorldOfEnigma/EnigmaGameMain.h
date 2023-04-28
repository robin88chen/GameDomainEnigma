/*********************************************************************
 * \file   EnigmaGameMain.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _ENIGMA_GAME_MAIN_H
#define _ENIGMA_GAME_MAIN_H

#include <string>
#include "Application/AppDelegateWin32.h"
#include "GameCommon/SceneRendererService.h"

namespace EnigmaGame
{
    class EnigmaGameMain : public Enigma::Application::AppDelegate
    {
    public:
        static inline std::string MediaPathName = "APK_PATH";
    public:
        EnigmaGameMain(const std::string app_name);
        ~EnigmaGameMain() override;

        virtual void InitializeMountPaths() override;
        virtual void InstallEngine() override final;
        virtual void ShutdownEngine() override final;
        virtual void FrameUpdate() override;
        virtual void RenderFrame() override;

    protected:
        std::weak_ptr<Enigma::GameCommon::SceneRendererService> m_sceneRendererService;
    };
}

#endif // _ENIGMA_GAME_MAIN_H
