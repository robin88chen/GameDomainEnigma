#include "ViewerAppDelegate.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayerUtilities.h"
#include "FileSystem/FileSystem.h"
#include "GraphicAPIDx11/GraphicAPIDx11.h"
#include "GameEngine/DeviceCreatingPolicy.h"
#include "GameEngine/EngineInstallingPolicy.h"
#include "Renderer/RendererInstallingPolicy.h"
#include "Animators/AnimatorInstallingPolicy.h"
#include "SceneGraph/SceneGraphInstallingPolicy.h"
#include "InputHandlers/InputHandlerInstallingPolicy.h"
#include "GameCommon/GameCommonInstallingPolicies.h"
#include "GameCommon/SceneRendererInstallingPolicy.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "Gateways/JsonFileEffectProfileDeserializer.h"
#include "CameraDtoMaker.h"
#include "FileSystem/StdMountPath.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "SceneGraph/Pawn.h"
#include "GameCommon/GameSceneService.h"
#include "ViewerCommands.h"
#include "Animators/AnimatorCommands.h"
#include "Animators/ModelPrimitiveAnimator.h"
#include <memory>

using namespace EnigmaViewer;
using namespace Enigma::Graphics;
using namespace Enigma::Platforms;
using namespace Enigma::FileSystem;
using namespace Enigma::Controllers;
using namespace Enigma::Devices;
using namespace Enigma::Engine;
using namespace Enigma::Renderer;
using namespace Enigma::Animators;
using namespace Enigma::SceneGraph;
using namespace Enigma::GameCommon;
using namespace Enigma::Gateways;
using namespace Enigma::Frameworks;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";
std::string SceneRootName = "_SceneRoot_";
std::string PortalManagementName = "_PortalManagement_";

ViewerAppDelegate::ViewerAppDelegate()
{
    m_hasLogFile = false;
    m_graphicMain = nullptr;
    m_hwnd = nullptr;
}

ViewerAppDelegate::~ViewerAppDelegate()
{
}

void ViewerAppDelegate::Initialize(IGraphicAPI::APIVersion api_ver, IGraphicAPI::AsyncType useAsyncDevice,
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

void ViewerAppDelegate::Finalize()
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
    delete FileSystem::Instance();

    CoUninitialize();
}

void ViewerAppDelegate::InitializeMountPaths()
{
    if (FileSystem::Instance())
    {
        auto path = std::filesystem::current_path();
        auto mediaPath = path / "../../../Media/";
        FileSystem::Instance()->AddMountPath(std::make_shared<StdMountPath>(mediaPath.string(), "APK_PATH"));
        FileSystem::Instance()->AddMountPath(std::make_shared<StdMountPath>(path.string(), "DataPath"));
    }
}

void ViewerAppDelegate::InstallEngine()
{
    m_onPawnPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnPawnPrimitiveBuilt(e); });
    EventPublisher::Subscribe(typeid(PawnPrimitiveBuilt), m_onPawnPrimitiveBuilt);

    assert(m_graphicMain);

    auto creating_policy = std::make_shared<DeviceCreatingPolicy>(DeviceRequiredBits(), m_hwnd);
    auto engine_policy = std::make_shared<EngineInstallingPolicy>(std::make_shared<JsonFileEffectProfileDeserializer>());
    auto render_sys_policy = std::make_shared<RenderSystemInstallingPolicy>();
    auto animator_policy = std::make_shared<AnimatorInstallingPolicy>();
    auto scene_graph_policy = std::make_shared<SceneGraphInstallingPolicy>(
        std::make_shared<JsonFileDtoDeserializer>());
    auto input_handler_policy = std::make_shared<Enigma::InputHandlers::InputHandlerInstallingPolicy>();
    auto game_camera_policy = std::make_shared<GameCameraInstallingPolicy>(CameraDtoMaker::MakeCameraDto());
    auto scene_renderer_policy = std::make_shared<SceneRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName, true);
    auto game_scene_policy = std::make_shared<GameSceneInstallingPolicy>(SceneRootName, PortalManagementName);
    auto animated_pawn = std::make_shared<AnimatedPawnInstallingPolicy>();
    m_graphicMain->InstallRenderEngine({ creating_policy, engine_policy, render_sys_policy, animator_policy, scene_graph_policy,
        input_handler_policy, game_camera_policy, scene_renderer_policy, game_scene_policy, animated_pawn });
    m_inputHandler = input_handler_policy->GetInputHandler();
    m_sceneRenderer = m_graphicMain->GetSystemServiceAs<SceneRendererService>();
}

void ViewerAppDelegate::RegisterMediaMountPaths(const std::string& media_path)
{
}

void ViewerAppDelegate::ShutdownEngine()
{
    m_pawn = nullptr;

    EventPublisher::Unsubscribe(typeid(PawnPrimitiveBuilt), m_onPawnPrimitiveBuilt);
    m_onPawnPrimitiveBuilt = nullptr;

    m_graphicMain->ShutdownRenderEngine();
}

void ViewerAppDelegate::FrameUpdate()
{
    if (m_graphicMain) m_graphicMain->FrameUpdate();
}

void ViewerAppDelegate::PrepareRender()
{
    if (!m_sceneRenderer.expired()) m_sceneRenderer.lock()->PrepareGameScene();
}

void ViewerAppDelegate::RenderFrame()
{
    if (!m_sceneRenderer.expired())
    {
        m_sceneRenderer.lock()->RenderGameScene();
        m_sceneRenderer.lock()->Flip();
    }
}

void ViewerAppDelegate::OnTimerElapsed()
{
    if (!m_graphicMain) return;

    FrameUpdate();

    PrepareRender();
    RenderFrame();
}

void ViewerAppDelegate::LoadPawn(const AnimatedPawnDto& pawn_dto)
{
    CommandBus::Post(std::make_shared<OutputMessage>("Load Pawn " + pawn_dto.Name()));
    CommandBus::Post(std::make_shared<BuildSceneGraph>("viewing_pawn", std::vector{ pawn_dto.ToGenericDto() }));
}

void ViewerAppDelegate::OnPawnPrimitiveBuilt(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<PawnPrimitiveBuilt, IEvent>(e);
    if (!ev) return;
    m_pawn = std::dynamic_pointer_cast<AnimatedPawn, Pawn>(ev->GetPawn());
    if (!m_pawn) return;
    auto scene_service = m_graphicMain->GetSystemServiceAs<GameSceneService>();
    if (!scene_service) return;
    Enigma::MathLib::Matrix4 mx = Enigma::MathLib::Matrix4::MakeRotationXTransform(-Enigma::MathLib::Math::HALF_PI);
    error er = scene_service->GetSceneRoot()->AttachChild(m_pawn, mx);
    auto prim = m_pawn->GetPrimitive();
    if (prim)
    {
        auto model = std::dynamic_pointer_cast<ModelPrimitive, Primitive>(prim);
        if (!model) return;
        CommandBus::Post(std::make_shared<RefreshModelNodeTree>(model));
        if (auto ani = model->GetAnimator())
        {
            ani->Reset();
            CommandBus::Post(std::make_shared<AddListeningAnimator>(ani));
            if (auto model_ani = std::dynamic_pointer_cast<ModelPrimitiveAnimator, Animator>(ani))
            {
                model_ani->PlayAnimation(AnimationClip{ 0.0f, 20.0f, AnimationClip::WarpMode::Loop, 0 });
            }
        }
    }
}
