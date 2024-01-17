#include "AppDelegateAndroid.h"
#include "ApplicationBridge.h"
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

void AppDelegate::initialize(Graphics::IGraphicAPI::APIVersion api_ver, Graphics::IGraphicAPI::AsyncType useAsyncDevice,
    const std::string& log_filename)
{
    if (log_filename.length() > 0)
    {
        Platforms::Logger::InitLoggerFile(log_filename);
        m_hasLogFile = true;
    }

    FileSystem::FileSystem::create();
    initializeMountPaths();

    m_graphicMain = menew Controllers::GraphicMain();
    m_graphicMain->installFrameworks();

    menew Devices::GraphicAPIEgl();

    installEngine();
}

void AppDelegate::finalize()
{
    shutdownEngine();

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

void AppDelegate::run()
{

}

void AppDelegate::frameUpdate()
{
    if (m_graphicMain) m_graphicMain->frameUpdate();
}

void AppDelegate::registerMediaMountPaths(const std::string& media_path)
{

}

void AppDelegate::onFrameSizeChanged(int w, int h)
{
    Frameworks::CommandBus::post(std::make_shared<Renderer::ResizePrimaryRenderTarget>(
        MathLib::Dimension<unsigned>{(unsigned)w, (unsigned)h}));
}

void AppDelegate::initBridgeCallback()
{
    ApplicationBridge::m_initializeGraphicDevice = []() { instance()->initialize(Graphics::IGraphicAPI::APIVersion::API_EGL, Graphics::IGraphicAPI::AsyncType::NotAsyncDevice, ""); };
    ApplicationBridge::m_finalizeGraphicDevice = []() { instance()->finalize(); };
    ApplicationBridge::m_onFrameUpdate = []() { instance()->frameUpdate(); };
    ApplicationBridge::m_onPrepareFrame = []() { instance()->prepareRender(); };
    ApplicationBridge::m_onDrawFrame = []() { instance()->renderFrame(); };
    ApplicationBridge::m_onRendererSurfaceSizeChanged = [](int w, int h) { instance()->onFrameSizeChanged(w, h); };
}

#endif
