#include "AppDelegateAndroid.h"
#include "ApplicationBridge.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Controllers/GraphicMain.h"
#include "FileSystem/FileSystem.h"
#include "Platforms/MemoryMacro.h"
#include "Renderer/RendererCommands.h"
#include "Frameworks/CommandBus.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "../GraphicAPIEgl/GraphicAPIEgl.h"

using namespace Enigma::Application;
AppDelegate* AppDelegate::m_instance = nullptr;

AppDelegate::AppDelegate(const std::string& app_name) : m_appName(app_name)
{
    m_instance = this;
    m_hasLogFile = false;
    m_hwnd = nullptr;
    m_graphicMain = nullptr;
}

AppDelegate::~AppDelegate()
{

}

void AppDelegate::Initialize(Graphics::IGraphicAPI::APIVersion api_ver, Graphics::IGraphicAPI::AsyncType useAsyncDevice,
    const std::string& log_filename)
{
    if (log_filename.length() > 0)
    {
        Platforms::Logger::InitLoggerFile(log_filename);
        m_hasLogFile = true;
    }

    FileSystem::FileSystem::create();
    InitializeMountPaths();

    m_graphicMain = menew Controllers::GraphicMain();
    m_graphicMain->installFrameworks();

    menew Devices::GraphicAPIEgl();

    InstallEngine();
}

void AppDelegate::Finalize()
{
    ShutdownEngine();

    std::this_thread::sleep_for(std::chrono::seconds(1)); // 放一點時間給thread 執行 cleanup
    Graphics::IGraphicAPI::instance()->TerminateGraphicThread(); // 先跳出thread
    delete Graphics::IGraphicAPI::instance();

    m_graphicMain->shutdownFrameworks();
    SAFE_DELETE(m_graphicMain);

    if (m_hasLogFile)
    {
        Platforms::Logger::CloseLoggerFile();
    }
    delete FileSystem::FileSystem::instance();
}

void AppDelegate::Run()
{

}

void AppDelegate::frameUpdate()
{
    if (m_graphicMain) m_graphicMain->frameUpdate();
}

void AppDelegate::RegisterMediaMountPaths(const std::string& media_path)
{

}

void AppDelegate::OnFrameSizeChanged(int w, int h)
{
    Frameworks::CommandBus::post(std::make_shared<Renderer::ResizePrimaryRenderTarget>(
        MathLib::Dimension<unsigned>{(unsigned)w, (unsigned)h}));
}

void AppDelegate::InitBridgeCallback()
{
    ApplicationBridge::m_initializeGraphicDevice = []() { instance()->Initialize(Graphics::IGraphicAPI::APIVersion::API_EGL, Graphics::IGraphicAPI::AsyncType::NotAsyncDevice, ""); };
    ApplicationBridge::m_finalizeGraphicDevice = []() { instance()->Finalize(); };
    ApplicationBridge::m_onFrameUpdate = []() { instance()->frameUpdate(); };
    ApplicationBridge::m_onPrepareFrame = []() { instance()->PrepareRender(); };
    ApplicationBridge::m_onDrawFrame = []() { instance()->RenderFrame(); };
    ApplicationBridge::m_onRendererSurfaceSizeChanged = [](int w, int h) { instance()->OnFrameSizeChanged(w, h); };
}

#endif
