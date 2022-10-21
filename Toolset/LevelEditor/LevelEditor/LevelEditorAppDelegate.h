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

namespace LevelEditor
{
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

    protected:
        HWND m_hwnd;
        bool m_hasLogFile;

        std::string m_mediaRootPath;

        Enigma::Controllers::GraphicMain* m_graphicMain;
    };
}


#endif // _LEVEL_EDITOR_APP_DELEGATE_H
