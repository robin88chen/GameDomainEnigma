/*********************************************************************
 * \file   AppDelegateAndroid.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef APP_DELEGATE_ANDROID_H
#define APP_DELEGATE_ANDROID_H

#include "Platforms/PlatformLayerUtilities.h"
#include "Controllers/GraphicMain.h"

#if TARGET_PLATFORM == PLATFORM_ANDROID

namespace Enigma::Application
{
    class AppDelegate
    {
    public:
        AppDelegate(const std::string& app_name);
        virtual ~AppDelegate();

        static AppDelegate* Instance() { return m_instance; };

        virtual void Initialize(Graphics::IGraphicAPI::APIVersion api_ver, Graphics::IGraphicAPI::AsyncType useAsyncDevice,
            const std::string& log_filename = "");
        virtual void InitializeMountPaths() {};
        virtual void InstallEngine() = 0;
        virtual void RegisterMediaMountPaths(const std::string& media_path);
        virtual void ShutdownEngine() = 0;
        virtual void Finalize();

        virtual void Run();
        virtual void FrameUpdate();
        virtual void PrepareRender() {};
        virtual void RenderFrame() {};

        virtual void OnFrameSizeChanged(int, int);

        /** for android graphic bridge callback */
        void InitBridgeCallback();

    private:
        static AppDelegate* m_instance;
    protected:
        std::string m_appName;
        /** for compatibility with win32 only */
        void* m_hwnd;
        bool m_hasLogFile;

        Controllers::GraphicMain* m_graphicMain;
    };
}
#endif

#endif // APP_DELEGATE_ANDROID_H
