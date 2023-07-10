/*********************************************************************
 * \file   LevelEditorAppDelegate.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _LEVEL_EDITOR_APP_DELEGATE_H
#define _LEVEL_EDITOR_APP_DELEGATE_H

#include <windows.h>
#include "GraphicKernel/IGraphicAPI.h"
#include "Controllers/GraphicMain.h"
#include "GameCommon/SceneRendererService.h"
#include "InputHandlers/InputHandlerService.h"
#include "Frameworks/EventSubscriber.h"

namespace LevelEditor
{
    class AppConfiguration;

    class EditorAppDelegate
    {
    public:
        EditorAppDelegate();
        ~EditorAppDelegate();

        void Initialize(Enigma::Graphics::IGraphicAPI::APIVersion api_ver, Enigma::Graphics::IGraphicAPI::AsyncType useAsyncDevice,
            const std::string& log_filename, HWND hwnd);
        virtual void InitializeMountPaths();
        void InstallEngine();
        void RegisterMediaMountPaths(const std::string& media_path);
        void ShutdownEngine();
        void Finalize();

        void FrameUpdate();
        void PrepareRender();
        void RenderFrame();

        void OnTimerElapsed();

        const std::unique_ptr<AppConfiguration>& GetAppConfig() const { return m_appConfig; };
    protected:
        void OnSceneGraphChanged(const Enigma::Frameworks::IEventPtr& e);
        void OnSceneRootCreated(const Enigma::Frameworks::IEventPtr& e);
        void OnWorldMapCreated(const Enigma::Frameworks::IEventPtr& e);

    protected:
        HWND m_hwnd;
        bool m_hasLogFile;

        std::string m_mediaRootPath;

        std::unique_ptr<AppConfiguration> m_appConfig;

        Enigma::Controllers::GraphicMain* m_graphicMain;
        std::weak_ptr<Enigma::InputHandlers::InputHandlerService> m_inputHandler;
        std::weak_ptr<Enigma::GameCommon::SceneRendererService> m_sceneRenderer;

        std::weak_ptr<Enigma::SceneGraph::Node> m_sceneRoot;

        Enigma::Frameworks::EventSubscriberPtr m_onSceneGraphChanged;
        Enigma::Frameworks::EventSubscriberPtr m_onSceneRootCreated;
        Enigma::Frameworks::EventSubscriberPtr m_onWorldMapCreated;
    };
}


#endif // _LEVEL_EDITOR_APP_DELEGATE_H
