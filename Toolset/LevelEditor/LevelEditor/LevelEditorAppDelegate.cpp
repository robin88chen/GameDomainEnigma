#include "LevelEditorAppDelegate.h"
#include "Platforms/PlatformLayerUtilities.h"
#include "FileSystem/FileSystem.h"
#include "Platforms/MemoryMacro.h"
#include "GraphicAPIDx11/GraphicAPIDx11.h"
#include "Controllers/InstallingPolicies.h"
#include <memory>

using namespace LevelEditor;
using namespace Enigma::Graphics;
using namespace Enigma::Platforms;
using namespace Enigma::Controllers;
using namespace Enigma::FileSystem;
using namespace Enigma::Devices;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";

EditorAppDelegate::EditorAppDelegate()
{
    m_hasLogFile = false;
    m_graphicMain = nullptr;
    m_hwnd = nullptr;
}

EditorAppDelegate::~EditorAppDelegate()
{
}

void EditorAppDelegate::Initialize(IGraphicAPI::APIVersion api_ver, IGraphicAPI::AsyncType useAsyncDevice,
                                   const std::string& log_filename, HWND hwnd)
{
    m_hwnd = hwnd;
    if (log_filename.length() > 0)
    {
        Logger::InitLoggerFile(log_filename);
        m_hasLogFile = true;
    }

    FileSystem::Create();
    InitializeMountPaths();

    m_graphicMain = menew GraphicMain();
    m_graphicMain->InstallFrameworks();

    menew GraphicAPIDx11(useAsyncDevice);

    CoInitializeEx(NULL, COINIT_MULTITHREADED);  // for WIC Texture Loader

    InstallEngine();
}

void EditorAppDelegate::Finalize()
{
    ShutdownEngine();

    std::this_thread::sleep_for(std::chrono::seconds(1)); // 放一點時間給thread 執行 cleanup
    IGraphicAPI::Instance()->TerminateGraphicThread(); // 先跳出thread
    delete IGraphicAPI::Instance();

    m_graphicMain->ShutdownFrameworks();
    SAFE_DELETE(m_graphicMain);

    if (m_hasLogFile)
    {
        Logger::CloseLoggerFile();
    }
    delete FileSystem::FileSystem::Instance();

    CoUninitialize();
}

void EditorAppDelegate::InitializeMountPaths()
{
}

void EditorAppDelegate::RegisterMediaMountPaths(const std::string& media_path)
{
}

void EditorAppDelegate::InstallEngine()
{
    assert(m_graphicMain);

    auto creating_policy = std::make_unique<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto policy = std::make_unique<InstallingDefaultRendererPolicy>(std::move(creating_policy), DefaultRendererName, PrimaryTargetName);
    m_graphicMain->InstallRenderEngine(std::move(policy));
}

void EditorAppDelegate::ShutdownEngine()
{
    assert(m_graphicMain);
    m_graphicMain->ShutdownRenderEngine();
}

void EditorAppDelegate::FrameUpdate()
{
    if (m_graphicMain) m_graphicMain->FrameUpdate();
}

void EditorAppDelegate::PrepareRender()
{
}

void EditorAppDelegate::RenderFrame()
{
}

void EditorAppDelegate::OnTimerElapsed()
{
    if (!m_graphicMain) return;

    FrameUpdate();

    PrepareRender();
    RenderFrame();
}
