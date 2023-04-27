/*********************************************************************
 * \file   ViewerAppDelegate.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef VIEWER_APP_DELEGATE_H
#define VIEWER_APP_DELEGATE_H

#include <windows.h>
#include "GraphicKernel/IGraphicAPI.h"
#include "Controllers/GraphicMain.h"
#include "InputHandlers/InputHandlerService.h"
#include "GameCommon/SceneRendererService.h"
#include "GameCommon/AnimatedPawnDto.h"
#include "Frameworks/EventSubscriber.h"
#include "GameCommon/AnimatedPawn.h"
#include "Frameworks/CommandSubscriber.h"
#include <filesystem>

namespace EnigmaViewer
{
    class ViewerAppDelegate
    {
    public:
        ViewerAppDelegate();
        ~ViewerAppDelegate();

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

        void LoadPawn(const Enigma::GameCommon::AnimatedPawnDto& pawn_dto);

        void SavePawnFile(const std::filesystem::path& filepath);
        void LoadPawnFile(const std::filesystem::path& filepath);

    protected:
        void OnPawnPrimitiveBuilt(const Enigma::Frameworks::IEventPtr& e);
        void DoChangingMeshTexture(const Enigma::Frameworks::ICommandPtr& c);
        void DoAddingAnimationClip(const Enigma::Frameworks::ICommandPtr& c);
        void DoDeletingAnimationClip(const Enigma::Frameworks::ICommandPtr& c);
        void DoPlayingAnimationClip(const Enigma::Frameworks::ICommandPtr& c);
        void DoChangingAnimationTimeValue(const Enigma::Frameworks::ICommandPtr& c);

    protected:
        HWND m_hwnd;
        bool m_hasLogFile;

        std::string m_mediaRootPath;

        Enigma::Controllers::GraphicMain* m_graphicMain;

        std::weak_ptr<Enigma::InputHandlers::InputHandlerService> m_inputHandler;
        std::weak_ptr<Enigma::GameCommon::SceneRendererService> m_sceneRenderer;

        Enigma::Frameworks::EventSubscriberPtr m_onPawnPrimitiveBuilt;

        Enigma::Frameworks::CommandSubscriberPtr m_doChangingMeshTexture;
        Enigma::Frameworks::CommandSubscriberPtr m_doAddingAnimationClip;
        Enigma::Frameworks::CommandSubscriberPtr m_doDeletingAnimationClip;
        Enigma::Frameworks::CommandSubscriberPtr m_doPlayingAnimationClip;
        Enigma::Frameworks::CommandSubscriberPtr m_doChangingAnimationTimeValue;

        std::shared_ptr<Enigma::GameCommon::AnimatedPawn> m_pawn;
    };
}

#endif // VIEWER_APP_DELEGATE_H
