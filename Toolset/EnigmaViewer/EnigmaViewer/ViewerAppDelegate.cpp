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
#include "GameCommon/AvatarRecipes.h"
#include <memory>
#include <Gateways/DtoJsonGateway.h>
#include "GameCommon/GameLightCommands.h"
#include "GameCommon/GameSceneEvents.h"
#include "SceneGraph/SceneGraphDtoHelper.h"
#include "ShadowMap/ShadowMapServiceConfiguration.h"
#include "ShadowMap/ShadowMapInstallingPolicies.h"
#include "ShadowMap/SpatialShadowFlags.h"
#include "GameEngine/StandardGeometryDtoHelper.h"
#include "GameEngine/EffectDtoHelper.h"

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
using namespace Enigma::ShadowMap;
using namespace Enigma::MathLib;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";
std::string SceneRootName = "_SceneRoot_";
std::string PortalManagementName = "_PortalManagement_";
std::string ViewingPawnName = "_ViewingPawn_";
std::string FloorReceiverName = "_Floor_";

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
    m_onSceneGraphRootCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnSceneGraphRootCreated(e); });
    EventPublisher::Subscribe(typeid(SceneRootCreated), m_onSceneGraphRootCreated);
    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnSceneGraphBuilt(e); });
    EventPublisher::Subscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);

    m_doChangingMeshTexture = std::make_shared<CommandSubscriber>([=](auto c) { this->DoChangingMeshTexture(c); });
    CommandBus::Subscribe(typeid(ChangeMeshTexture), m_doChangingMeshTexture);
    m_doAddingAnimationClip = std::make_shared<CommandSubscriber>([=](auto c) { this->DoAddingAnimationClip(c); });
    CommandBus::Subscribe(typeid(AddAnimationClip), m_doAddingAnimationClip);
    m_doDeletingAnimationClip = std::make_shared<CommandSubscriber>([=](auto c) { this->DoDeletingAnimationClip(c); });
    CommandBus::Subscribe(typeid(DeleteAnimationClip), m_doDeletingAnimationClip);
    m_doPlayingAnimationClip = std::make_shared<CommandSubscriber>([=](auto c) { this->DoPlayingAnimationClip(c); });
    CommandBus::Subscribe(typeid(PlayAnimationClip), m_doPlayingAnimationClip);
    m_doChangingAnimationTimeValue = std::make_shared<CommandSubscriber>([=](auto c) { this->DoChangingAnimationTimeValue(c); });
    CommandBus::Subscribe(typeid(ChangeAnimationTimeValue), m_doChangingAnimationTimeValue);

    assert(m_graphicMain);

    auto creating_policy = std::make_shared<DeviceCreatingPolicy>(DeviceRequiredBits(), m_hwnd);
    auto engine_policy = std::make_shared<EngineInstallingPolicy>(std::make_shared<JsonFileEffectProfileDeserializer>());
    auto render_sys_policy = std::make_shared<RenderSystemInstallingPolicy>();
    auto animator_policy = std::make_shared<AnimatorInstallingPolicy>();
    auto scene_graph_policy = std::make_shared<SceneGraphInstallingPolicy>(
        std::make_shared<JsonFileDtoDeserializer>());
    auto input_handler_policy = std::make_shared<Enigma::InputHandlers::InputHandlerInstallingPolicy>();
    auto game_camera_policy = std::make_shared<GameCameraInstallingPolicy>(
        CameraDtoHelper("camera").EyePosition(Enigma::MathLib::Vector3(-5.0f, 5.0f, -5.0f)).LookAt(Enigma::MathLib::Vector3(1.0f, -1.0f, 1.0f)).UpDirection(Enigma::MathLib::Vector3::UNIT_Y)
        .Frustum("frustum", Frustum::ProjectionType::Perspective).FrustumFov(Enigma::MathLib::Math::PI / 4.0f).FrustumFrontBackZ(0.1f, 100.0f)
        .FrustumNearPlaneDimension(40.0f, 30.0f).ToCameraDto());
    auto deferred_config = std::make_shared<DeferredRendererServiceConfiguration>();
    deferred_config->SunLightEffectName() = "DeferredShadingWithShadowSunLightPass";
    deferred_config->SunLightPassFxFileName() = "fx/DeferredShadingWithShadowSunLightPass.efx@APK_PATH";
    deferred_config->SunLightSpatialFlags() |= SpatialShadowFlags::Spatial_ShadowReceiver;
    auto deferred_renderer_policy = std::make_shared<DeferredRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName, deferred_config);
    //auto scene_render_config = std::make_shared<SceneRendererServiceConfiguration>();
    //auto scene_renderer_policy = std::make_shared<SceneRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName, scene_render_config);
    auto game_scene_policy = std::make_shared<GameSceneInstallingPolicy>(SceneRootName, PortalManagementName);
    auto animated_pawn = std::make_shared<AnimatedPawnInstallingPolicy>();
    auto game_light_policy = std::make_shared<GameLightInstallingPolicy>();
    auto shadow_map_config = std::make_shared<ShadowMapServiceConfiguration>();
    auto shadow_map_policy = std::make_shared<ShadowMapInstallingPolicy>("shadowmap_renderer", "shadowmap_target", shadow_map_config);
    m_graphicMain->InstallRenderEngine({ creating_policy, engine_policy, render_sys_policy, animator_policy, scene_graph_policy,
        input_handler_policy, game_camera_policy, deferred_renderer_policy, game_scene_policy, animated_pawn, game_light_policy, shadow_map_policy });
    m_inputHandler = input_handler_policy->GetInputHandler();
    m_sceneRenderer = m_graphicMain->GetSystemServiceAs<SceneRendererService>();
    m_shadowMapService = m_graphicMain->GetSystemServiceAs<ShadowMapService>();
}

void ViewerAppDelegate::RegisterMediaMountPaths(const std::string& media_path)
{
}

void ViewerAppDelegate::ShutdownEngine()
{
    m_pawn = nullptr;
    m_sceneRoot = nullptr;
    m_floor = nullptr;

    EventPublisher::Unsubscribe(typeid(PawnPrimitiveBuilt), m_onPawnPrimitiveBuilt);
    m_onPawnPrimitiveBuilt = nullptr;
    EventPublisher::Unsubscribe(typeid(SceneRootCreated), m_onSceneGraphRootCreated);
    m_onSceneGraphRootCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphBuilt = nullptr;

    CommandBus::Unsubscribe(typeid(ChangeMeshTexture), m_doChangingMeshTexture);
    m_doChangingMeshTexture = nullptr;
    CommandBus::Unsubscribe(typeid(AddAnimationClip), m_doAddingAnimationClip);
    m_doAddingAnimationClip = nullptr;
    CommandBus::Unsubscribe(typeid(DeleteAnimationClip), m_doDeletingAnimationClip);
    m_doDeletingAnimationClip = nullptr;
    CommandBus::Unsubscribe(typeid(PlayAnimationClip), m_doPlayingAnimationClip);
    m_doPlayingAnimationClip = nullptr;
    CommandBus::Unsubscribe(typeid(ChangeAnimationTimeValue), m_doChangingAnimationTimeValue);
    m_doChangingAnimationTimeValue = nullptr;

    m_graphicMain->ShutdownRenderEngine();
}

void ViewerAppDelegate::FrameUpdate()
{
    if (m_graphicMain) m_graphicMain->FrameUpdate();
}

void ViewerAppDelegate::PrepareRender()
{
    if (!m_shadowMapService.expired()) m_shadowMapService.lock()->PrepareShadowScene();
    if (!m_sceneRenderer.expired()) m_sceneRenderer.lock()->PrepareGameScene();
}

void ViewerAppDelegate::RenderFrame()
{
    if (!m_shadowMapService.expired()) m_shadowMapService.lock()->RenderShadowScene();
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
    CommandBus::Post(std::make_shared<BuildSceneGraph>(ViewingPawnName, std::vector{ pawn_dto.ToGenericDto() }));
}

void ViewerAppDelegate::SavePawnFile(const std::filesystem::path& filepath)
{
    if (!m_pawn) return;
    auto pawn_dto = m_pawn->SerializeDto();
    pawn_dto.AsTopLevel(true);
    std::string json = DtoJsonGateway::Serialize(std::vector<GenericDto>{pawn_dto});
    IFilePtr iFile = FileSystem::Instance()->OpenFile(filepath.generic_string(), "w+b");
    iFile->Write(0, convert_to_buffer(json));
    FileSystem::Instance()->CloseFile(iFile);
}

void ViewerAppDelegate::LoadPawnFile(const std::filesystem::path& filepath)
{
    if (m_pawn)
    {
        m_pawn->DetachFromParent();
        m_pawn = nullptr;
    }
    IFilePtr iFile = FileSystem::Instance()->OpenFile(filepath.generic_string(), "rb");
    size_t file_size = iFile->Size();

    auto read_buf = iFile->Read(0, file_size);
    FileSystem::Instance()->CloseFile(iFile);
    auto dtos = DtoJsonGateway::Deserialize(convert_to_string(read_buf.value(), file_size));
    CommandBus::Post(std::make_shared<BuildSceneGraph>(ViewingPawnName, dtos));
}

void ViewerAppDelegate::OnPawnPrimitiveBuilt(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<PawnPrimitiveBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetPawn() == m_pawn)
    {
        OnViewingPawnPrimitiveBuilt();
    }
    else if (ev->GetPawn() == m_floor)
    {
        OnFloorPrimitiveBuilt();
    }
}

void ViewerAppDelegate::OnViewingPawnPrimitiveBuilt()
{
    m_pawn->GetPrimitive()->SelectVisualTechnique("Default");
    m_pawn->BakeAvatarRecipes();
    CommandBus::Post(std::make_shared<RefreshAnimationClipList>(m_pawn->TheAnimationClipMap()));
    auto scene_service = m_graphicMain->GetSystemServiceAs<GameSceneService>();
    if (!scene_service) return;
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

void ViewerAppDelegate::OnFloorPrimitiveBuilt()
{
    m_floor->GetPrimitive()->SelectVisualTechnique("ShadowMapReceiver");
}

void ViewerAppDelegate::OnSceneGraphRootCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneRootCreated, IEvent>(e);
    if (!ev) return;
    m_sceneRoot = ev->GetSceneRoot();
    CommandBus::Post(std::make_shared<CreateAmbientLight>(m_sceneRoot, "amb_lit", Enigma::MathLib::ColorRGBA(0.2f, 0.2f, 0.2f, 1.0f)));
    CommandBus::Post(std::make_shared<CreateSunLight>(m_sceneRoot, "sun_lit", Enigma::MathLib::Vector3(-1.0, -1.0, -1.0), Enigma::MathLib::ColorRGBA(0.6f, 0.6f, 0.6f, 1.0f)));
    auto mx = Enigma::MathLib::Matrix4::MakeTranslateTransform(2.0f, 2.0f, 2.0f);
    CommandBus::Post(std::make_shared<CreatePointLight>(m_sceneRoot, mx, "point_lit", Enigma::MathLib::Vector3(2.0f, 2.0f, 2.0f), Enigma::MathLib::ColorRGBA(3.0f, 0.0f, 3.0f, 1.0f), 3.50f));
    CreateFloorReceiver();
}

void ViewerAppDelegate::OnSceneGraphBuilt(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt, IEvent>(e);
    if (!ev) return;
    auto top_spatials = ev->GetTopLevelSpatial();
    if (top_spatials.empty()) return;
    if (ev->GetSceneGraphId() == FloorReceiverName)
    {
        m_floor = std::dynamic_pointer_cast<Pawn, Spatial>(top_spatials[0]);
        if (m_sceneRoot) m_sceneRoot->AttachChild(m_floor, Matrix4::IDENTITY);
    }
    else if (ev->GetSceneGraphId() == ViewingPawnName)
    {
        m_pawn = std::dynamic_pointer_cast<AnimatedPawn, Spatial>(top_spatials[0]);
        Enigma::MathLib::Matrix4 mx = Enigma::MathLib::Matrix4::MakeRotationXTransform(-Enigma::MathLib::Math::HALF_PI);
        if (m_sceneRoot) m_sceneRoot->AttachChild(m_pawn, mx);
    }
}

void ViewerAppDelegate::DoChangingMeshTexture(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<ChangeMeshTexture, ICommand>(c);
    if (!cmd) return;
    if (!m_pawn) return;
    TextureMappingDto tex_dto;
    tex_dto.Filename() = "image/" + cmd->GetTextureFilename();
    tex_dto.TextureName() = cmd->GetTextureFilename();
    tex_dto.PathId() = "APK_PATH";
    tex_dto.Semantic() = "DiffuseMap";
    auto recipe = std::make_shared<ChangeAvatarTexture>(cmd->GetMeshName(), tex_dto);
    m_pawn->AddAvatarRecipe(recipe);
    m_pawn->BakeAvatarRecipes();
}

void ViewerAppDelegate::DoAddingAnimationClip(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<AddAnimationClip, ICommand>(c);
    if (!cmd) return;
    if (!m_pawn) return;
    if (auto act_clip = m_pawn->TheAnimationClipMap().FindAnimationClip(cmd->GetName()); !act_clip)
    {
        AnimationClipMap::AnimClip act_clip_new(cmd->GetName(), cmd->GetClip());
        m_pawn->TheAnimationClipMap().InsertClip(act_clip_new);
    }
    else
    {
        act_clip.value().get().ChangeClip(cmd->GetClip());
    }
}

void ViewerAppDelegate::DoDeletingAnimationClip(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<DeleteAnimationClip, ICommand>(c);
    if (!cmd) return;
    if (!m_pawn) return;
    m_pawn->TheAnimationClipMap().RemoveClip(cmd->GetName());
}

void ViewerAppDelegate::DoPlayingAnimationClip(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<PlayAnimationClip, ICommand>(c);
    if (!cmd) return;
    if (!m_pawn) return;
    m_pawn->PlayAnimation(cmd->GetName());
}

void ViewerAppDelegate::DoChangingAnimationTimeValue(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<ChangeAnimationTimeValue, ICommand>(c);
    if (!cmd) return;
    if (!m_pawn) return;
    bool isNameChanged = false;
    if ((m_pawn->TheAnimationClipMap().FindAnimationClip(cmd->GetOldName()))
        && (cmd->GetOldName() != cmd->GetNewName()))
    {
        isNameChanged = true;
    }

    if (!isNameChanged)
    {
        if (auto act_clip = m_pawn->TheAnimationClipMap().FindAnimationClip(cmd->GetNewName()); act_clip)
        {
            act_clip.value().get().ChangeClip(cmd->GetClip());
        }
    }
    else
    {
        m_pawn->TheAnimationClipMap().RemoveClip(cmd->GetOldName());
        Enigma::GameCommon::AnimationClipMap::AnimClip act_clip_new(cmd->GetNewName(), cmd->GetClip());
        m_pawn->TheAnimationClipMap().InsertClip(act_clip_new);
    }
}

void ViewerAppDelegate::CreateFloorReceiver()
{
    PawnDtoHelper pawn_dto(FloorReceiverName);
    MeshPrimitiveDto mesh_dto;
    SquareQuadDtoHelper floor_dto("floor");
    floor_dto.XZQuad(Vector3(-5.0f, 0.0f, -5.0f), Vector3(5.0f, 0.0f, 5.0f)).Normal().TextureCoord(Vector2(0.0f, 1.0f), Vector2(1.0f, 0.0f));
    EffectMaterialDtoHelper mat_dto("default_textured_mesh_effect");
    mat_dto.FilenameAtPath("fx/default_textured_mesh_effect.efx@APK_PATH");
    EffectTextureMapDtoHelper tex_dto;
    tex_dto.TextureMapping("image/du011.png", "APK_PATH", "du011", std::nullopt, "DiffuseMap");
    mesh_dto.Name() = "floor_mesh";
    mesh_dto.Effects().emplace_back(mat_dto.ToGenericDto());
    mesh_dto.TextureMaps().emplace_back(tex_dto.ToGenericDto());
    mesh_dto.GeometryName() = "floor";
    mesh_dto.TheGeometry() = floor_dto.ToGenericDto();

    pawn_dto.MeshPrimitive(mesh_dto).LocalTransform(Matrix4::IDENTITY).TopLevel(true).SpatialFlags(Spatial::Spatial_BelongToParent | SpatialShadowFlags::Spatial_ShadowReceiver);
    auto dtos = { pawn_dto.ToGenericDto() };
    CommandBus::Post(std::make_shared<BuildSceneGraph>(FloorReceiverName, dtos));
}
