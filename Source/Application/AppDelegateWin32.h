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
        AppDelegate(const std::string& app_name);
        virtual ~AppDelegate();

        static AppDelegate* instance() { return m_instance; };

        virtual void initialize(Graphics::IGraphicAPI::APIVersion api_ver, Graphics::IGraphicAPI::AsyncType useAsyncDevice,
            const std::string& log_filename = "");
        virtual void initializeMountPaths() {};

        virtual void installEngine() = 0;
        virtual void registerMediaMountPaths(const std::string& media_path);
        virtual void shutdownEngine() = 0;
        virtual void finalize();

        virtual void run();
        virtual void frameUpdate();
        virtual void prepareRender() {};
        virtual void renderFrame() {};

        virtual void onFrameSizeChanged(int, int);

        HWND getWnd() { return m_hwnd; };
    private:
        /// The event handler.
        static LRESULT CALLBACK WindowProcedure(HWND handle, UINT message,
            WPARAM wParam, LPARAM lParam);

        void registerAppClass();
        void create();
        void destroy();

        /// Processing Message
        void onMouseMove(WPARAM wParam, LPARAM lParam);
        void onMouseLeftDown(WPARAM wParam, LPARAM lParam);
        void onMouseLeftUp(WPARAM wParam, LPARAM lParam);
        void onMouseLeftDblClick(WPARAM wParam, LPARAM lParam);
        void onMouseRightDown(WPARAM wParam, LPARAM lParam);
        void onMouseRightUp(WPARAM wParam, LPARAM lParam);
        void onMouseRightDblClick(WPARAM wParam, LPARAM lParam);
        void onMouseMiddleDown(WPARAM wParam, LPARAM lParam);
        void onMouseMiddleUp(WPARAM wParam, LPARAM lParam);
        void onMouseMiddleDblClick(WPARAM wParam, LPARAM lParam);
        void onMouseWheel(WPARAM wParam, LPARAM lParam);
        void onKeyChar(WPARAM wParam, LPARAM lParam);
        void onKeyDown(WPARAM wParam, LPARAM lParam);
        void onKeyUp(WPARAM wParam, LPARAM lParam);
        void onClose(WPARAM wParam, LPARAM lParam);
        void onDestroy(WPARAM wParam, LPARAM lParam);

        void onIdle();
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
