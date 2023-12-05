/*********************************************************************
 * \file   AppDelegateWin32.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef _APPLICATION_DELEGATE_WIN32_H
#define _APPLICATION_DELEGATE_WIN32_H

#include "Platforms/PlatformLayerUtilities.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "Controllers/GraphicMain.h"
#include "InputHandlers/InputHandlerService.h"

#if TARGET_PLATFORM == PLATFORM_WIN32
#include <string>
#include <Windows.h>

namespace Enigma::Application
{
    class AppDelegate
    {
    public:
        AppDelegate(const std::string app_name);
        virtual ~AppDelegate();

        static AppDelegate* instance() { return m_instance; };

        virtual void Initialize(Graphics::IGraphicAPI::APIVersion api_ver, Graphics::IGraphicAPI::AsyncType useAsyncDevice,
                                const std::string & log_filename = "");
        virtual void InitializeMountPaths() {};

        virtual void InstallEngine() = 0;
        virtual void RegisterMediaMountPaths(const std::string& media_path);
        virtual void ShutdownEngine() = 0;
        virtual void Finalize();

        virtual void Run();
        virtual void FrameUpdate();
        virtual void PrepareRender() {};
        virtual void RenderFrame() {};

        virtual void OnFrameSizeChanged(int , int );

        HWND GetWnd() { return m_hwnd; };
    private:
        /// The event handler.
        static LRESULT CALLBACK WindowProcedure(HWND handle, UINT message,
            WPARAM wParam, LPARAM lParam);

        void RegisterAppClass();
        void create();
        void Destroy();

        /// Processing Message
        void OnMouseMove(WPARAM wParam, LPARAM lParam);
        void OnMouseLeftDown(WPARAM wParam, LPARAM lParam);
        void OnMouseLeftUp(WPARAM wParam, LPARAM lParam);
        void OnMouseLeftDblClick(WPARAM wParam, LPARAM lParam);
        void OnMouseRightDown(WPARAM wParam, LPARAM lParam);
        void OnMouseRightUp(WPARAM wParam, LPARAM lParam);
        void OnMouseRightDblClick(WPARAM wParam, LPARAM lParam);
        void OnMouseMiddleDown(WPARAM wParam, LPARAM lParam);
        void OnMouseMiddleUp(WPARAM wParam, LPARAM lParam);
        void OnMouseMiddleDblClick(WPARAM wParam, LPARAM lParam);
        void OnMouseWheel(WPARAM wParam, LPARAM lParam);
        void OnKeyChar(WPARAM wParam, LPARAM lParam);
        void OnKeyDown(WPARAM wParam, LPARAM lParam);
        void OnKeyUp(WPARAM wParam, LPARAM lParam);
        void OnClose(WPARAM wParam, LPARAM lParam);
        void OnDestroy(WPARAM wParam, LPARAM lParam);

        void OnIdle();
    private:
        static AppDelegate* m_instance;

    protected:
        std::string m_appName;
        ATOM m_atom;
        HWND m_hwnd;
        bool m_hasLogFile;

        std::string m_mediaRootPath;

        Controllers::GraphicMain* m_graphicMain;
        std::weak_ptr<InputHandlers::InputHandlerService> m_inputHandler;
    };
}
#endif


#endif // !_APPLICATION_DELEGATE_WIN32_H
