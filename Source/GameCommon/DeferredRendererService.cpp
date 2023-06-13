#include "DeferredRendererService.h"
#include "DeferredRendererServiceConfiguration.h"
#include "GameCameraEvents.h"
#include "GameSceneService.h"
#include "LightVolumePawn.h"
#include "Controllers/GraphicMain.h"
#include "Frameworks/CommandBus.h"
#include "Platforms/PlatformLayer.h"
#include "Renderer/DeferredRenderer.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "SceneGraph/Light.h"
#include "GameEngine/EffectDtoHelper.h"
#include "GameEngine/StandardGeometryDtoHelper.h"
#include "Renderer/RenderablePrimitiveDtos.h"
#include "Renderer/RenderablePrimitiveRequests.h"
#include "Renderer/RenderablePrimitiveResponses.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/RequestBus.h"
#include "Frameworks/ResponseBus.h"
#include "Renderer/RendererEvents.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphDtoHelper.h"
#include "GameCommon/GameCameraService.h"

using namespace Enigma::GameCommon;
using namespace Enigma::Frameworks;
using namespace Enigma::Renderer;
using namespace Enigma::Engine;
using namespace Enigma::SceneGraph;

using error = std::error_code;

DEFINE_RTTI(GameCommon, DeferredRendererService, SceneRendererService);

//!  samples 的數量太少，光球的 poly 太粗，會導致算出的受光亮面偏移
#define SPHERE_STACKS 40
#define SPHERE_SLICES 80

DeferredRendererService::DeferredRendererService(ServiceManager* mngr,
    const std::shared_ptr<GameSceneService>& scene_service, const std::shared_ptr<GameCameraService>& camera_service,
    const std::shared_ptr<Renderer::RendererManager>& renderer_manager,
    const std::shared_ptr<SceneGraph::SceneGraphRepository>& scene_graph_repository,
    std::unique_ptr<DeferredRendererServiceConfiguration> configuration) : SceneRendererService(mngr, scene_service, camera_service, renderer_manager, nullptr)
{
    m_configuration = std::move(configuration);
    m_sceneGraphRepository = scene_graph_repository;
    m_ambientLightQuad = nullptr;
    m_sunLightQuad = nullptr;
    LightVolumePawn::SetDefaultVisualTech(m_configuration->VisualTechniqueNameForCameraDefault());
    LightVolumePawn::SetInsideVisualTech(m_configuration->VisualTechniqueNameForCameraInside());
}

DeferredRendererService::~DeferredRendererService()
{
    m_configuration = nullptr;
    m_ambientLightQuad = nullptr;
    m_sunLightQuad = nullptr;
    m_gBuffer = nullptr;
    m_lightVolumes.clear();
}

ServiceResult DeferredRendererService::OnInit()
{
    m_onPrimaryRenderTargetCreated = std::make_shared<EventSubscriber>([=](auto e) { OnPrimaryRenderTargetCreated(e); });
    EventPublisher::Subscribe(typeid(Renderer::PrimaryRenderTargetCreated), m_onPrimaryRenderTargetCreated);
    m_onGameCameraUpdated = std::make_shared<EventSubscriber>([=](auto e) { OnGameCameraUpdated(e); });
    EventPublisher::Subscribe(typeid(GameCameraUpdated), m_onGameCameraUpdated);
    m_onSceneGraphChanged = std::make_shared<EventSubscriber>([=](auto e) { OnSceneGraphChanged(e); });
    EventPublisher::Subscribe(typeid(SceneGraph::SceneGraphChanged), m_onSceneGraphChanged);
    m_onGBufferTextureCreated = std::make_shared<EventSubscriber>([=](auto e) { OnGBufferTextureCreated(e); });
    EventPublisher::Subscribe(typeid(Renderer::RenderTargetTextureCreated), m_onGBufferTextureCreated);
    m_onLightInfoCreated = std::make_shared<EventSubscriber>([=](auto e) { OnLightInfoCreated(e); });
    EventPublisher::Subscribe(typeid(SceneGraph::LightInfoCreated), m_onLightInfoCreated);
    m_onLightInfoDeleted = std::make_shared<EventSubscriber>([=](auto e) { OnLightInfoDeleted(e); });
    EventPublisher::Subscribe(typeid(SceneGraph::LightInfoDeleted), m_onLightInfoDeleted);
    m_onLightInfoUpdated = std::make_shared<EventSubscriber>([=](auto e) { OnLightInfoUpdated(e); });
    EventPublisher::Subscribe(typeid(SceneGraph::LightInfoUpdated), m_onLightInfoUpdated);
    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { OnSceneGraphBuilt(e); });
    EventPublisher::Subscribe(typeid(SceneGraph::FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onPawnPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { OnPawnPrimitiveBuilt(e); });
    EventPublisher::Subscribe(typeid(SceneGraph::PawnPrimitiveBuilt), m_onPawnPrimitiveBuilt);

    m_onBuildPrimitiveResponse = std::make_shared<ResponseSubscriber>([=](auto e) { OnBuildPrimitiveResponse(e); });
    ResponseBus::Subscribe(typeid(BuildRenderablePrimitiveResponse), m_onBuildPrimitiveResponse);

    return SceneRendererService::OnInit();
}

ServiceResult DeferredRendererService::OnTerm()
{
    EventPublisher::Unsubscribe(typeid(Renderer::PrimaryRenderTargetCreated), m_onPrimaryRenderTargetCreated);
    m_onPrimaryRenderTargetCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(GameCameraUpdated), m_onGameCameraUpdated);
    m_onGameCameraUpdated = nullptr;
    EventPublisher::Unsubscribe(typeid(SceneGraph::SceneGraphChanged), m_onSceneGraphChanged);
    m_onSceneGraphChanged = nullptr;
    EventPublisher::Unsubscribe(typeid(Renderer::RenderTargetTextureCreated), m_onGBufferTextureCreated);
    m_onGBufferTextureCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(SceneGraph::LightInfoCreated), m_onLightInfoCreated);
    m_onLightInfoCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(SceneGraph::LightInfoDeleted), m_onLightInfoDeleted);
    m_onLightInfoDeleted = nullptr;
    EventPublisher::Unsubscribe(typeid(SceneGraph::LightInfoUpdated), m_onLightInfoUpdated);
    m_onLightInfoUpdated = nullptr;
    EventPublisher::Unsubscribe(typeid(SceneGraph::FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphBuilt = nullptr;
    EventPublisher::Unsubscribe(typeid(SceneGraph::PawnPrimitiveBuilt), m_onPawnPrimitiveBuilt);
    m_onPawnPrimitiveBuilt = nullptr;

    ResponseBus::Unsubscribe(typeid(BuildRenderablePrimitiveResponse), m_onBuildPrimitiveResponse);
    m_onBuildPrimitiveResponse = nullptr;

    m_ambientLightQuad = nullptr;
    m_sunLightQuad = nullptr;
    m_gBuffer = nullptr;
    m_lightVolumes.clear();

    return SceneRendererService::OnTerm();
}

void DeferredRendererService::CreateSceneRenderSystem(const std::string& renderer_name, const std::string& target_name)
{
    assert(!m_rendererManager.expired());
    assert(m_configuration);
    auto rendererManager = m_rendererManager.lock();
    Engine::IRendererPtr renderer = std::make_shared<Renderer::DeferredRenderer>(renderer_name);
    error er = rendererManager->InsertRenderer(renderer_name, renderer);
    if (er) return;

    RenderTargetPtr primaryTarget = rendererManager->GetPrimaryRenderTarget();
    if (!primaryTarget)
    {
        rendererManager->CreateRenderTarget(target_name, RenderTarget::PrimaryType::IsPrimary, { Graphics::RenderTextureUsage::Default });
        primaryTarget = rendererManager->GetPrimaryRenderTarget();
    }
    if (FATAL_LOG_EXPR(!primaryTarget)) return;

    m_renderer = std::dynamic_pointer_cast<Renderer::Renderer, Engine::IRenderer>(rendererManager->GetRenderer(renderer_name));
    m_renderer.lock()->SelectRendererTechnique(m_configuration->DeferredRendererTechniqueName());
    m_renderer.lock()->SetRenderTarget(primaryTarget);
    if (primaryTarget->GetBackSurface() && primaryTarget->GetDepthStencilSurface())
    {
        CreateGBuffer(primaryTarget);
    }
}

void DeferredRendererService::DestroySceneRenderSystem(const std::string& renderer_name, const std::string& target_name)
{
    assert(!m_rendererManager.expired());
    assert(m_configuration);
    m_rendererManager.lock()->DestroyRenderTarget(target_name);
    m_rendererManager.lock()->DestroyRenderTarget(m_configuration->GbufferTargetName());
    m_rendererManager.lock()->DestroyRenderer(renderer_name);
}

void DeferredRendererService::PrepareGameScene()
{
    if (!m_renderer.expired())
    {
        if (m_ambientLightQuad) m_ambientLightQuad->InsertToRendererWithTransformUpdating(m_renderer.lock(),
            MathLib::Matrix4::IDENTITY, m_ambientQuadLightingState);
        if (m_sunLightQuad) m_sunLightQuad->InsertToRendererWithTransformUpdating(m_renderer.lock(),
            MathLib::Matrix4::IDENTITY, m_sunLightQuadLightingState);
    }
    SceneRendererService::PrepareGameScene();
}

void DeferredRendererService::CreateGBuffer(const Renderer::RenderTargetPtr& primary_target)
{
    assert(primary_target);
    assert(!m_rendererManager.expired());
    assert(m_configuration);

    auto [width, height] = primary_target->GetDimension();

    m_rendererManager.lock()->CreateRenderTarget(m_configuration->GbufferTargetName(), RenderTarget::PrimaryType::NotPrimary,
        m_configuration->GbufferUsages());
    m_gBuffer = m_rendererManager.lock()->GetRenderTarget(m_configuration->GbufferTargetName());
    if (m_gBuffer)
    {
        m_gBuffer->InitMultiBackSurface(m_configuration->GbufferSurfaceName(), MathLib::Dimension{ width, height }, m_configuration->GbufferFormats().size(),
            m_configuration->GbufferFormats());
        m_gBuffer->ShareDepthStencilSurface(m_configuration->GbufferDepthName(), primary_target->GetDepthStencilSurface());
    }
    if (const auto deferRender = std::dynamic_pointer_cast<DeferredRenderer, Renderer::Renderer>(m_renderer.lock()))
    {
        deferRender->AttachGBufferTarget(m_gBuffer);
    }
    if (m_ambientLightQuad) BindGBufferToLightingMesh(m_ambientLightQuad);
    if (m_sunLightQuad) BindGBufferToLightingMesh(m_sunLightQuad);
    for (auto& light : m_lightVolumes)
    {
        BindGBufferToLightVolume(light.second);
    }
}

void DeferredRendererService::OnPrimaryRenderTargetCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Renderer::PrimaryRenderTargetCreated, Frameworks::IEvent>(e);
    if (!ev) return;
    auto primaryTarget = ev->GetRenderTarget();
    if (!primaryTarget) return;

    CreateGBuffer(primaryTarget);
}

void DeferredRendererService::OnGameCameraUpdated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<GameCommon::GameCameraUpdated, Frameworks::IEvent>(e);
    if (!ev) return;
    if (!ev->GetCamera()) return;
    for (auto& kv : m_lightVolumes)
    {
        CheckLightVolumeBackfaceCulling(kv.second, ev->GetCamera());
    }
}

void DeferredRendererService::OnSceneGraphChanged(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneGraph::SceneGraphChanged, Frameworks::IEvent>(e);
    if ((!ev) || (ev->GetChild())) return;
    // 抓light entity 被 attach 的訊息來改變 light volume 的 parent
    if (!ev->GetChild()->TypeInfo().IsDerived(Light::TYPE_RTTI)) return;
    if (ev->GetNotifyCode() != SceneGraphChanged::NotifyCode::AttachChild) return;
    const auto light = std::dynamic_pointer_cast<Light, Spatial>(ev->GetChild());
    if (!light) return;
    const auto lightVolume = FindLightVolume(light->GetSpatialName());
    auto parent_node = std::dynamic_pointer_cast<Node, Spatial>(ev->GetParentNode());
    if (lightVolume) lightVolume->ChangeWorldPosition(lightVolume->GetWorldPosition(), parent_node);
}

void DeferredRendererService::OnGBufferTextureCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Renderer::RenderTargetTextureCreated, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetRenderTarget() != m_gBuffer) return;
    if (m_ambientLightQuad) BindGBufferToLightingMesh(m_ambientLightQuad);
    if (m_sunLightQuad) BindGBufferToLightingMesh(m_sunLightQuad);
    for (auto& light : m_lightVolumes)
    {
        BindGBufferToLightVolume(light.second);
    }
}

void DeferredRendererService::OnLightInfoCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneGraph::LightInfoCreated, Frameworks::IEvent>(e);
    if ((!ev) || (!ev->GetLight())) return;
    if (ev->GetLight()->Info().GetLightType() == SceneGraph::LightInfo::LightType::Ambient)
    {
        CreateAmbientLightQuad(ev->GetLight());
    }
    else if (ev->GetLight()->Info().GetLightType() == SceneGraph::LightInfo::LightType::SunLight)
    {
        CreateSunLightQuad(ev->GetLight());
    }
    else if (ev->GetLight()->Info().GetLightType() == SceneGraph::LightInfo::LightType::Point)
    {
        CreatePointLightVolume(ev->GetLight());
    }
}

void DeferredRendererService::OnLightInfoDeleted(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneGraph::LightInfoDeleted, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetLightType() == SceneGraph::LightInfo::LightType::Ambient)
    {
        m_ambientLightQuad = nullptr;
    }
    else if (ev->GetLightType() == SceneGraph::LightInfo::LightType::SunLight)
    {
        m_sunLightQuad = nullptr;
    }
    else if (ev->GetLightType() == SceneGraph::LightInfo::LightType::Point)
    {
        DeletePointLightVolume(ev->GetLightName());
    }
}

void DeferredRendererService::OnLightInfoUpdated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneGraph::LightInfoUpdated, Frameworks::IEvent>(e);
    if ((!ev) || (!ev->GetLight())) return;
    if (ev->GetLight()->Info().GetLightType() == SceneGraph::LightInfo::LightType::Ambient)
    {
        UpdateAmbientLightQuad(ev->GetLight(), ev->GetNotifyCode());
    }
    else if (ev->GetLight()->Info().GetLightType() == SceneGraph::LightInfo::LightType::SunLight)
    {
        UpdateSunLightQuad(ev->GetLight(), ev->GetNotifyCode());
    }
    else if (ev->GetLight()->Info().GetLightType() == SceneGraph::LightInfo::LightType::Point)
    {
        UpdatePointLightVolume(ev->GetLight(), ev->GetNotifyCode());
    }
}

void DeferredRendererService::OnSceneGraphBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneGraph::FactorySceneGraphBuilt, Frameworks::IEvent>(e);
    if (!ev) return;
    auto top_spatials = ev->GetTopLevelSpatial();
    if (top_spatials.empty()) return;
    auto pawn = std::dynamic_pointer_cast<LightVolumePawn, Spatial>(top_spatials[0]);
    if (!pawn) return;
    if ((!m_sceneService.expired()) && (m_sceneService.lock()->GetSceneRoot()))
    {
        m_sceneService.lock()->GetSceneRoot()->AttachChild(pawn, pawn->GetLocalTransform());
    }
    if (!m_sceneGraphRepository.expired())
    {
        pawn->SetHostLight(m_sceneGraphRepository.lock()->QueryLight(ev->GetSceneGraphId()));
    }
    m_lightVolumes.insert_or_assign(ev->GetSceneGraphId(), pawn);
    BindGBufferToLightVolume(pawn);
    CheckLightVolumeBackfaceCulling(ev->GetSceneGraphId());
}

void DeferredRendererService::OnPawnPrimitiveBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneGraph::PawnPrimitiveBuilt, Frameworks::IEvent>(e);
    if (!ev) return;
    auto pawn = std::dynamic_pointer_cast<LightVolumePawn, Pawn>(ev->GetPawn());
    if (!pawn) return;
    BindGBufferToLightVolume(pawn);
    pawn->NotifySpatialRenderStateChanged();
    CheckLightVolumeBackfaceCulling(pawn->GetHostLightName());
}

void DeferredRendererService::OnBuildPrimitiveResponse(const Frameworks::IResponsePtr& r)
{
    if (!r) return;
    const auto res = std::dynamic_pointer_cast<Renderer::BuildRenderablePrimitiveResponse, Frameworks::IResponse>(r);
    if (!res) return;
    if (res->GetRequestRuid() == m_ambientQuadRequester)
    {
        m_ambientLightQuad = std::dynamic_pointer_cast<MeshPrimitive, Primitive>(res->GetPrimitive());
        BindGBufferToLightingMesh(m_ambientLightQuad);
    }
    else if (res->GetRequestRuid() == m_sunLightQuadRequester)
    {
        m_sunLightQuad = std::dynamic_pointer_cast<MeshPrimitive, Primitive>(res->GetPrimitive());
        BindGBufferToLightingMesh(m_sunLightQuad);
    }
}

void DeferredRendererService::CreateAmbientLightQuad(const std::shared_ptr<SceneGraph::Light>& lit)
{
    assert(lit);
    std::string quad_geo_name = std::string("deferred_ambient_quad.geo");
    SquareQuadDtoHelper quad_dto_helper(quad_geo_name);
    quad_dto_helper.XYQuad(MathLib::Vector3(-1.0f, -1.0f, 0.5f), MathLib::Vector3(1.0f, 1.0f, 0.5f))
        .TextureCoord(MathLib::Vector2(0.0f, 1.0f), MathLib::Vector2(1.0f, 0.0f));
    EffectMaterialDtoHelper eff_dto_helper(m_configuration->AmbientEffectName());
    eff_dto_helper.FilenameAtPath(m_configuration->AmbientPassFxFileName());

    MeshPrimitiveDto mesh_dto;
    mesh_dto.Name() = quad_geo_name;
    mesh_dto.GeometryName() = quad_geo_name;
    mesh_dto.TheGeometry() = quad_dto_helper.ToGenericDto();
    mesh_dto.Effects().emplace_back(eff_dto_helper.ToGenericDto());
    mesh_dto.RenderListID() = Renderer::Renderer::RenderListID::DeferredLighting;

    auto mesh_policy = mesh_dto.ConvertToPolicy(nullptr);
    auto request = std::make_shared<Renderer::RequestBuildRenderablePrimitive>(mesh_policy);
    m_ambientQuadRequester = request->GetRuid();
    RequestBus::Post(request);

    m_ambientQuadLightingState.SetAmbientLightColor(lit->GetLightColor());
}

void DeferredRendererService::CreateSunLightQuad(const std::shared_ptr<SceneGraph::Light>& lit)
{
    assert(lit);
    std::string quad_geo_name = std::string("deferred_sunlight_quad.geo");
    SquareQuadDtoHelper quad_dto_helper(quad_geo_name);
    quad_dto_helper.XYQuad(MathLib::Vector3(-1.0f, -1.0f, 0.5f), MathLib::Vector3(1.0f, 1.0f, 0.5f))
        .TextureCoord(MathLib::Vector2(0.0f, 1.0f), MathLib::Vector2(1.0f, 0.0f));
    EffectMaterialDtoHelper eff_dto_helper(m_configuration->SunLightEffectName());
    eff_dto_helper.FilenameAtPath(m_configuration->SunLightPassFxFileName());

    MeshPrimitiveDto mesh_dto;
    mesh_dto.Name() = quad_geo_name;
    mesh_dto.GeometryName() = quad_geo_name;
    mesh_dto.TheGeometry() = quad_dto_helper.ToGenericDto();
    mesh_dto.Effects().emplace_back(eff_dto_helper.ToGenericDto());
    mesh_dto.RenderListID() = Renderer::Renderer::RenderListID::DeferredLighting;

    auto mesh_policy = mesh_dto.ConvertToPolicy(nullptr);
    auto request = std::make_shared<Renderer::RequestBuildRenderablePrimitive>(mesh_policy);
    m_sunLightQuadRequester = request->GetRuid();
    RequestBus::Post(request);

    m_sunLightQuadLightingState.SetSunLight(lit->GetLightDirection(), lit->GetLightColor());
}

void DeferredRendererService::CreatePointLightVolume(const std::shared_ptr<SceneGraph::Light>& lit)
{
    assert(lit);
    std::string vol_geo_name = "deferred_" + lit->GetSpatialName() + "_lit_volume.geo";
    SphereDtoHelper sphere_dto_helper(vol_geo_name);
    sphere_dto_helper.Sphere(MathLib::Vector3::ZERO, lit->GetLightRange(), SPHERE_SLICES, SPHERE_STACKS).BoxBound();
    EffectMaterialDtoHelper eff_dto_helper(m_configuration->LightVolumeEffectName());
    eff_dto_helper.FilenameAtPath(m_configuration->LightVolumePassFxFileName());

    MeshPrimitiveDto mesh_dto;
    mesh_dto.Name() = vol_geo_name;
    mesh_dto.GeometryName() = vol_geo_name;
    mesh_dto.TheGeometry() = sphere_dto_helper.ToGenericDto();
    mesh_dto.Effects().emplace_back(eff_dto_helper.ToGenericDto());
    mesh_dto.RenderListID() = Renderer::Renderer::RenderListID::DeferredLighting;

    PawnDtoHelper pawn_helper(lit->GetSpatialName() + "_lit_volume");
    pawn_helper.Factory(FactoryDesc(LightVolumePawn::TYPE_RTTI.GetName())).MeshPrimitive(mesh_dto)
        .SpatialFlags(Spatial::Spatial_BelongToParent).TopLevel(true).LocalTransform(lit->GetLocalTransform());
    auto dtos = { pawn_helper.ToGenericDto() };
    CommandBus::Post(std::make_shared<BuildSceneGraph>(lit->GetSpatialName(), dtos));
}

void DeferredRendererService::DeletePointLightVolume(const std::string& name)
{
    if (const auto it = m_lightVolumes.find(name); it != m_lightVolumes.end())
    {
        const auto& pawn = it->second;
        pawn->DetachFromParent();
        m_lightVolumes.erase(it);
    }
}

void DeferredRendererService::UpdateAmbientLightQuad(const std::shared_ptr<SceneGraph::Light>& lit, SceneGraph::LightInfoUpdated::NotifyCode notify)
{
    assert(lit);
    if (notify != LightInfoUpdated::NotifyCode::Color) return;
    m_ambientQuadLightingState.SetAmbientLightColor(lit->GetLightColor());
}

void DeferredRendererService::UpdatePointLightVolume(const std::shared_ptr<SceneGraph::Light>& lit, SceneGraph::LightInfoUpdated::NotifyCode notify)
{
    assert(lit);
    if (lit->Info().GetLightType() != LightInfo::LightType::Point) return;
    const auto& volume = FindLightVolume(lit->GetSpatialName());
    if (notify == LightInfoUpdated::NotifyCode::Position)
    {
        volume->ChangeWorldPosition(lit->GetLightPosition(), std::nullopt);
    }
    else if (notify == LightInfoUpdated::NotifyCode::Range)
    {
        volume->SetLocalUniformScale(lit->GetLightRange());
    }
    else if (notify == LightInfoUpdated::NotifyCode::Enable)
    {
        if (lit->Info().IsEnable())
        {
            volume->RemoveSpatialFlag(Spatial::Spatial_Hide);
        }
        else
        {
            volume->AddSpatialFlag(Spatial::Spatial_Hide);
        }
    }
    else if (notify == LightInfoUpdated::NotifyCode::Color)
    {
        volume->NotifySpatialRenderStateChanged();
    }

    CheckLightVolumeBackfaceCulling(lit->GetSpatialName());
}

void DeferredRendererService::UpdateSunLightQuad(const std::shared_ptr<SceneGraph::Light>& lit, SceneGraph::LightInfoUpdated::NotifyCode notify)
{
    assert(lit);
    if ((notify == LightInfoUpdated::NotifyCode::Color) || (notify == LightInfoUpdated::NotifyCode::Direction))
    {
        m_sunLightQuadLightingState.SetSunLight(lit->GetLightDirection(), lit->GetLightColor());
    }
}

void DeferredRendererService::BindGBufferToLightingMesh(const Renderer::MeshPrimitivePtr& mesh)
{
    assert(m_configuration);
    if (!mesh) return;
    if (!m_gBuffer) return;
    if (!m_gBuffer->GetRenderTargetTexture()) return;

    EffectTextureMap::EffectTextures textures = {
        { m_configuration->GbufferNormalSemantic(), m_gBuffer->GetRenderTargetTexture(), m_gBuffer->FindRenderTextureUsageIndex(Graphics::RenderTextureUsage::Normal) },
        { m_configuration->GbufferDiffuseSemantic(), m_gBuffer->GetRenderTargetTexture(), m_gBuffer->FindRenderTextureUsageIndex(Graphics::RenderTextureUsage::Albedo) },
        { m_configuration->GbufferSpecularSemantic(), m_gBuffer->GetRenderTargetTexture(), m_gBuffer->FindRenderTextureUsageIndex(Graphics::RenderTextureUsage::Specular) },
        { m_configuration->GbufferDepthSemantic(), m_gBuffer->GetRenderTargetTexture(), m_gBuffer->FindRenderTextureUsageIndex(Graphics::RenderTextureUsage::Depth) } };
    mesh->ChangeTextureMap({ textures });
}

void DeferredRendererService::BindGBufferToLightVolume(const std::shared_ptr<LightVolumePawn>& volume)
{
    if (!volume) return;
    auto mesh = std::dynamic_pointer_cast<MeshPrimitive, Primitive>(volume->GetPrimitive());
    if (mesh) BindGBufferToLightingMesh(mesh);
}

std::shared_ptr<LightVolumePawn> DeferredRendererService::FindLightVolume(const std::string& name)
{
    if (const auto it = m_lightVolumes.find(name); it != m_lightVolumes.end())
    {
        return it->second;
    }
    return nullptr;
}

void DeferredRendererService::CheckLightVolumeBackfaceCulling(const std::string& lit_name)
{
    auto lit_vol = FindLightVolume(lit_name);
    if (!lit_vol) return;
    if (m_cameraService.expired()) return;
    auto camera = m_cameraService.lock()->GetPrimaryCamera();
    if (!camera) return;
    CheckLightVolumeBackfaceCulling(lit_vol, camera);
}

void DeferredRendererService::CheckLightVolumeBackfaceCulling(const std::shared_ptr<LightVolumePawn>& lit_vol,
    const std::shared_ptr<SceneGraph::Camera>& cam)
{
    if (!lit_vol) return;
    if (!cam) return;
    const BoundingVolume& bv = lit_vol->GetWorldBound();
    if (bv.IsEmpty()) return;
    if (bv.PointInside(cam->GetLocation()))
    {
        lit_vol->ToggleCameraInside(true);
    }
    else
    {
        lit_vol->ToggleCameraInside(false);
    }
}
