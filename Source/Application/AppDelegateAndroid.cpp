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
    m_inputHandler = nullptr;
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

    FileSystem::FileSystem::Create();
    InitializeMountPaths();

    m_graphicMain = menew Controllers::GraphicMain();
    m_graphicMain->InstallFrameworks();

    menew Devices::GraphicAPIEgl();

    InstallEngine();
}

void AppDelegate::Finalize()
{
    ShutdownEngine();

    std::this_thread::sleep_for(std::chrono::seconds(1)); // 放一點時間給thread 執行 cleanup
    Graphics::IGraphicAPI::Instance()->TerminateGraphicThread(); // 先跳出thread
    delete Graphics::IGraphicAPI::Instance();

    m_graphicMain->ShutdownFrameworks();
    SAFE_DELETE(m_graphicMain);

    if (m_hasLogFile)
    {
        Platforms::Logger::CloseLoggerFile();
    }
    delete FileSystem::FileSystem::Instance();
}

void AppDelegate::Run()
{

}

void AppDelegate::FrameUpdate()
{
    if (m_graphicMain) m_graphicMain->FrameUpdate();
}

void AppDelegate::RegisterMediaMountPaths(const std::string& media_path)
{

}

void AppDelegate::OnFrameSizeChanged(int w, int h)
{
    Frameworks::CommandBus::Post(std::make_shared<Renderer::ResizePrimaryRenderTarget>(
        MathLib::Dimension{(unsigned)w, (unsigned)h}));
}

void AppDelegate::InitBridgeCallback()
{
    ApplicationBridge::m_initializeGraphicDevice = []() { Instance()->Initialize(Graphics::IGraphicAPI::APIVersion::API_EGL, Graphics::IGraphicAPI::AsyncType::NotAsyncDevice, ""); };
    ApplicationBridge::m_finalizeGraphicDevice = []() { Instance()->Finalize(); };
    ApplicationBridge::m_onFrameUpdate = []() { Instance()->FrameUpdate(); };
    ApplicationBridge::m_onPrepareFrame = []() { Instance()->PrepareRender(); };
    ApplicationBridge::m_onDrawFrame = []() { Instance()->RenderFrame(); };
    ApplicationBridge::m_onRendererSurfaceSizeChanged = [](int w, int h) { Instance()->OnFrameSizeChanged(w, h); };
}

#endif
