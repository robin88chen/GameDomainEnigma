#include "DeferredRendererService.h"
#include "DeferredRendererServiceConfiguration.h"
#include "GameCameraEvents.h"
#include "GameLightEvents.h"
#include "GameSceneCommands.h"
#include "GameSceneService.h"
#include "LightVolumePawn.h"
#include "LightQuadPawn.h"
#include "Controllers/GraphicMain.h"
#include "Frameworks/CommandBus.h"
#include "Platforms/PlatformLayer.h"
#include "Renderer/DeferredRenderer.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "SceneGraph/Light.h"
#include "GameEngine/EffectDtoHelper.h"
#include "GameEngine/StandardGeometryDtoHelper.h"
#include "Renderer/RenderablePrimitiveDtos.h"
#include "Frameworks/EventPublisher.h"
#include "Renderer/RendererEvents.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphDtoHelper.h"
#include "GameCommon/GameCameraService.h"
#include "LightingPawnDto.h"
#include "DeferredRenderingCommands.h"
#include "Renderer/ModelPrimitive.h"

using namespace Enigma::GameCommon;
using namespace Enigma::Frameworks;
using namespace Enigma::Renderer;
using namespace Enigma::Engine;
using namespace Enigma::SceneGraph;


DEFINE_RTTI(GameCommon, DeferredRendererService, SceneRendererService);

//!  samples 的數量太少，光球的 poly 太粗，會導致算出的受光亮面偏移
#define SPHERE_STACKS 40
#define SPHERE_SLICES 80

std::string DeferredSunLightQuadName = "_deferred_sun_light_quad_";
std::string DeferredAmbientLightQuadName = "_deferred_ambient_light_quad_";

DeferredRendererService::DeferredRendererService(ServiceManager* mngr,
    const std::shared_ptr<GameSceneService>& scene_service, const std::shared_ptr<GameCameraService>& camera_service,
    const std::shared_ptr<Renderer::RendererManager>& renderer_manager,
    const std::shared_ptr<DeferredRendererServiceConfiguration>& configuration) : SceneRendererService(mngr, scene_service, camera_service, renderer_manager, configuration)
{
    m_configuration = configuration;
    LightVolumePawn::SetDefaultVisualTech(m_configuration->VisualTechniqueNameForCameraDefault());
    LightVolumePawn::SetInsideVisualTech(m_configuration->VisualTechniqueNameForCameraInside());
}

DeferredRendererService::~DeferredRendererService()
{
    m_configuration = nullptr;
    m_lightingPawns.clear();
}

ServiceResult DeferredRendererService::onInit()
{
    m_onPrimaryRenderTargetCreated = std::make_shared<EventSubscriber>([=](auto e) { OnPrimaryRenderTargetCreated(e); });
    EventPublisher::subscribe(typeid(Renderer::PrimaryRenderTargetCreated), m_onPrimaryRenderTargetCreated);
    m_onPrimaryRenderTargetResized = std::make_shared<EventSubscriber>([=](auto e) { OnPrimaryRenderTargetResized(e); });
    EventPublisher::subscribe(typeid(Renderer::RenderTargetResized), m_onPrimaryRenderTargetResized);
    m_onGameCameraUpdated = std::make_shared<EventSubscriber>([=](auto e) { OnGameCameraUpdated(e); });
    EventPublisher::subscribe(typeid(GameCameraUpdated), m_onGameCameraUpdated);
    m_onSceneGraphChanged = std::make_shared<EventSubscriber>([=](auto e) { OnSceneGraphChanged(e); });
    EventPublisher::subscribe(typeid(SceneGraph::SceneGraphChanged), m_onSceneGraphChanged);
    m_onGBufferTextureCreated = std::make_shared<EventSubscriber>([=](auto e) { OnGBufferTextureCreated(e); });
    EventPublisher::subscribe(typeid(Renderer::RenderTargetTextureCreated), m_onGBufferTextureCreated);
    m_onGameLightCreated = std::make_shared<EventSubscriber>([=](auto e) { OnGameLightCreated(e); });
    EventPublisher::subscribe(typeid(GameLightCreated), m_onGameLightCreated);
    m_onLightInfoDeleted = std::make_shared<EventSubscriber>([=](auto e) { OnLightInfoDeleted(e); });
    EventPublisher::subscribe(typeid(SceneGraph::LightInfoDeleted), m_onLightInfoDeleted);
    m_onLightInfoUpdated = std::make_shared<EventSubscriber>([=](auto e) { OnLightInfoUpdated(e); });
    EventPublisher::subscribe(typeid(SceneGraph::LightInfoUpdated), m_onLightInfoUpdated);
    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { OnSceneGraphBuilt(e); });
    EventPublisher::subscribe(typeid(SceneGraph::FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onLightingPawnCreated = std::make_shared<EventSubscriber>([=](auto e) { OnLightingPawnCreated(e); });
    EventPublisher::subscribe(typeid(SceneGraph::FactorySpatialCreated), m_onLightingPawnCreated);
    m_onPawnPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { OnPawnPrimitiveBuilt(e); });
    EventPublisher::subscribe(typeid(SceneGraph::PawnPrimitiveBuilt), m_onPawnPrimitiveBuilt);

    m_doBindingGBuffer = std::make_shared<CommandSubscriber>([=](auto c) { DoBindingGBuffer(c); });
    CommandBus::subscribe(typeid(BindGBuffer), m_doBindingGBuffer);

    return SceneRendererService::onInit();
}

ServiceResult DeferredRendererService::onTerm()
{
    EventPublisher::unsubscribe(typeid(Renderer::PrimaryRenderTargetCreated), m_onPrimaryRenderTargetCreated);
    m_onPrimaryRenderTargetCreated = nullptr;
    EventPublisher::unsubscribe(typeid(Renderer::RenderTargetResized), m_onPrimaryRenderTargetResized);
    m_onPrimaryRenderTargetResized = nullptr;
    EventPublisher::unsubscribe(typeid(GameCameraUpdated), m_onGameCameraUpdated);
    m_onGameCameraUpdated = nullptr;
    EventPublisher::unsubscribe(typeid(SceneGraph::SceneGraphChanged), m_onSceneGraphChanged);
    m_onSceneGraphChanged = nullptr;
    EventPublisher::unsubscribe(typeid(Renderer::RenderTargetTextureCreated), m_onGBufferTextureCreated);
    m_onGBufferTextureCreated = nullptr;
    EventPublisher::unsubscribe(typeid(GameLightCreated), m_onGameLightCreated);
    m_onGameLightCreated = nullptr;
    EventPublisher::unsubscribe(typeid(SceneGraph::LightInfoDeleted), m_onLightInfoDeleted);
    m_onLightInfoDeleted = nullptr;
    EventPublisher::unsubscribe(typeid(SceneGraph::LightInfoUpdated), m_onLightInfoUpdated);
    m_onLightInfoUpdated = nullptr;
    EventPublisher::unsubscribe(typeid(SceneGraph::FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphBuilt = nullptr;
    EventPublisher::unsubscribe(typeid(SceneGraph::FactorySpatialCreated), m_onLightingPawnCreated);
    m_onLightingPawnCreated = nullptr;
    EventPublisher::unsubscribe(typeid(SceneGraph::PawnPrimitiveBuilt), m_onPawnPrimitiveBuilt);
    m_onPawnPrimitiveBuilt = nullptr;

    CommandBus::unsubscribe(typeid(BindGBuffer), m_doBindingGBuffer);
    m_doBindingGBuffer = nullptr;

    m_lightingPawns.clear();

    return SceneRendererService::onTerm();
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
    if (!m_gBuffer.expired())
    {
        m_gBuffer.lock()->InitMultiBackSurface(m_configuration->GbufferSurfaceName(), MathLib::Dimension<unsigned>{ width, height }, static_cast<unsigned>(m_configuration->GbufferFormats().size()),
            m_configuration->GbufferFormats());
        m_gBuffer.lock()->ShareDepthStencilSurface(m_configuration->GbufferDepthName(), primary_target->GetDepthStencilSurface());
    }
    if (const auto deferRender = std::dynamic_pointer_cast<DeferredRenderer, Renderer::Renderer>(m_renderer.lock()))
    {
        deferRender->AttachGBufferTarget(m_gBuffer.lock());
    }
    for (auto& light : m_lightingPawns)
    {
        if (!light.second.expired()) BindGBufferToLightingPawn(light.second.lock());
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

void DeferredRendererService::OnPrimaryRenderTargetResized(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Renderer::RenderTargetResized, Frameworks::IEvent>(e);
    if (!ev) return;
    const auto target = ev->GetRenderTarget();
    if ((!target) || (!target->IsPrimary())) return;

    if ((!m_gBuffer.expired()) && (m_gBuffer.lock()->GetDimension() != target->GetDimension())) m_gBuffer.lock()->Resize(target->GetDimension());
}

void DeferredRendererService::OnGameCameraUpdated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<GameCommon::GameCameraUpdated, Frameworks::IEvent>(e);
    if (!ev) return;
    if (!ev->GetCamera()) return;
    for (auto& kv : m_lightingPawns)
    {
        if (kv.second.expired()) continue;
        if (auto volume = std::dynamic_pointer_cast<LightVolumePawn, LightingPawn>(kv.second.lock()))
        {
            CheckLightVolumeBackfaceCulling(volume, ev->GetCamera());
        }
    }
}

void DeferredRendererService::OnSceneGraphChanged(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneGraph::SceneGraphChanged, Frameworks::IEvent>(e);
    if ((!ev) || (ev->GetChild())) return;
    // 抓light entity 被 attach 的訊息來改變 light volume 的 parent
    if (!ev->GetChild()->typeInfo().isDerived(Light::TYPE_RTTI)) return;
    if (ev->GetNotifyCode() != SceneGraphChanged::NotifyCode::AttachChild) return;
    const auto light = std::dynamic_pointer_cast<Light, Spatial>(ev->GetChild());
    if (!light) return;
    const auto lightPawn = FindLightingPawn(light->getSpatialName());
    auto parent_node = std::dynamic_pointer_cast<Node, Spatial>(ev->GetParentNode());
    if (lightPawn) lightPawn->changeWorldPosition(lightPawn->getWorldPosition(), parent_node);
}

void DeferredRendererService::OnGBufferTextureCreated(const Frameworks::IEventPtr& e)
{
    assert(!m_gBuffer.expired());
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Renderer::RenderTargetTextureCreated, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetRenderTarget() != m_gBuffer.lock()) return;
    for (auto& light : m_lightingPawns)
    {
        if (!light.second.expired()) BindGBufferToLightingPawn(light.second.lock());
    }
}

void DeferredRendererService::OnGameLightCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<GameLightCreated, Frameworks::IEvent>(e);
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
    RemoveLightingPawn(ev->GetLightName());
    CommandBus::post(std::make_shared<DeleteSceneSpatial>(ev->GetLightName()));
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
    if (!top_spatials[0]) return;
    auto lighting_pawn = std::dynamic_pointer_cast<LightingPawn, Spatial>(top_spatials[0]);
    if (!lighting_pawn) return;
    if (const auto lit = m_buildingLightPawns.find(top_spatials[0]->getSpatialName()); lit != m_buildingLightPawns.end())
    {
        if (!lit->second.m_parentNodeName.empty())
        {
            CommandBus::post(std::make_shared<AttachNodeChild>(lit->second.m_parentNodeName, top_spatials[0], lit->second.m_localTransform));
        }
        m_buildingLightPawns.erase(lit);
    }
}

void DeferredRendererService::OnLightingPawnCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<FactorySpatialCreated, Frameworks::IEvent>(e);
    if (!ev) return;
    auto lighting_pawn = std::dynamic_pointer_cast<LightingPawn, Spatial>(ev->GetSpatial());
    if (!lighting_pawn) return;
    OnLightingPawnBuilt(lighting_pawn->GetHostLightName(), lighting_pawn);
}

void DeferredRendererService::OnLightingPawnBuilt(const std::string& lit_name, const std::shared_ptr<LightingPawn>& lighting_pawn)
{
    if (!lighting_pawn) return;
    m_lightingPawns.insert_or_assign(lit_name, lighting_pawn);
    BindGBufferToLightingPawn(lighting_pawn);
    CheckLightVolumeBackfaceCulling(lit_name);
}

void DeferredRendererService::OnPawnPrimitiveBuilt(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PawnPrimitiveBuilt, IEvent>(e);
    if ((!ev) || (!ev->GetPawn())) return;
    auto lighting_pawn = std::dynamic_pointer_cast<LightingPawn, Pawn>(ev->GetPawn());
    if (!lighting_pawn) return;
    BindGBufferToLightingPawn(lighting_pawn);
    lighting_pawn->notifySpatialRenderStateChanged();
    CheckLightVolumeBackfaceCulling(lighting_pawn->GetHostLightName());
}

void DeferredRendererService::DoBindingGBuffer(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<BindGBuffer>(c);
    if (!cmd) return;
    if ((!cmd->GetPawn()) || (!cmd->GetPawn()->GetPrimitive())) return;
    if (const auto model = std::dynamic_pointer_cast<ModelPrimitive>(cmd->GetPawn()->GetPrimitive()))
    {
        const auto mesh_count = model->GetMeshPrimitiveCount();
        for (unsigned i = 0; i < mesh_count; i++)
        {
            if (auto mesh = model->GetMeshPrimitive(i))
            {
                BindGBufferToLightingMesh(mesh);
            }
        }
    }
    else if (const auto mesh = std::dynamic_pointer_cast<MeshPrimitive>(cmd->GetPawn()->GetPrimitive()))
    {
        BindGBufferToLightingMesh(mesh);
    }
}

void DeferredRendererService::CreateAmbientLightQuad(const std::shared_ptr<SceneGraph::Light>& lit)
{
    assert(lit);
    std::string quad_geo_name = lit->getSpatialName() + "_lit_quad" + ".geo";
    SquareQuadDtoHelper quad_dto_helper(quad_geo_name);
    quad_dto_helper.XYQuad(MathLib::Vector3(-1.0f, -1.0f, 0.5f), MathLib::Vector3(1.0f, 1.0f, 0.5f))
        .TextureCoord(MathLib::Vector2(0.0f, 1.0f), MathLib::Vector2(1.0f, 0.0f));
    EffectMaterialDtoHelper eff_dto_helper(m_configuration->AmbientEffectName());
    eff_dto_helper.FilenameAtPath(m_configuration->AmbientPassFxFileName());

    MeshPrimitiveDto mesh_dto;
    mesh_dto.Name() = quad_geo_name;
    mesh_dto.GeometryName() = quad_geo_name;
    mesh_dto.TheGeometry() = quad_dto_helper.toGenericDto();
    mesh_dto.Effects().emplace_back(eff_dto_helper.toGenericDto());
    mesh_dto.RenderListID() = Renderer::Renderer::RenderListID::DeferredLighting;

    PawnDtoHelper pawn_helper(lit->getSpatialName() + "_lit_quad");
    pawn_helper.MeshPrimitive(mesh_dto)
        .TopLevel(true)
        .Factory(FactoryDesc(LightQuadPawn::TYPE_RTTI.getName()));
    LightingPawnDto lighting_pawn_dto = LightingPawnDto(pawn_helper.ToPawnDto());
    lighting_pawn_dto.HostLightName() = lit->getSpatialName();
    auto pawn_dto = lighting_pawn_dto.toGenericDto();
    auto dtos = { pawn_dto };
    CommandBus::post(std::make_shared<BuildSceneGraph>(lit->getSpatialName(), dtos));
    InsertLightPawnBuildingMeta(pawn_dto.getName(), lit);
}

void DeferredRendererService::CreateSunLightQuad(const std::shared_ptr<SceneGraph::Light>& lit)
{
    assert(lit);
    std::string quad_geo_name = lit->getSpatialName() + "_lit_quad" + ".geo";
    SquareQuadDtoHelper quad_dto_helper(quad_geo_name);
    quad_dto_helper.XYQuad(MathLib::Vector3(-1.0f, -1.0f, 0.5f), MathLib::Vector3(1.0f, 1.0f, 0.5f))
        .TextureCoord(MathLib::Vector2(0.0f, 1.0f), MathLib::Vector2(1.0f, 0.0f));
    EffectMaterialDtoHelper eff_dto_helper(m_configuration->SunLightEffectName());
    eff_dto_helper.FilenameAtPath(m_configuration->SunLightPassFxFileName());

    MeshPrimitiveDto mesh_dto;
    mesh_dto.Name() = quad_geo_name;
    mesh_dto.GeometryName() = quad_geo_name;
    mesh_dto.TheGeometry() = quad_dto_helper.toGenericDto();
    mesh_dto.Effects().emplace_back(eff_dto_helper.toGenericDto());
    mesh_dto.RenderListID() = Renderer::Renderer::RenderListID::DeferredLighting;

    PawnDtoHelper pawn_helper(lit->getSpatialName() + "_lit_quad");
    pawn_helper.MeshPrimitive(mesh_dto)
        .SpatialFlags(m_configuration->SunLightSpatialFlags()).TopLevel(true)
        .Factory(FactoryDesc(LightQuadPawn::TYPE_RTTI.getName()));
    LightingPawnDto lighting_pawn_dto = LightingPawnDto(pawn_helper.ToPawnDto());
    lighting_pawn_dto.HostLightName() = lit->getSpatialName();
    auto pawn_dto = lighting_pawn_dto.toGenericDto();
    auto dtos = { pawn_dto };
    CommandBus::post(std::make_shared<BuildSceneGraph>(lit->getSpatialName(), dtos));
    InsertLightPawnBuildingMeta(pawn_dto.getName(), lit);
}

void DeferredRendererService::CreatePointLightVolume(const std::shared_ptr<SceneGraph::Light>& lit)
{
    assert(lit);
    std::string vol_geo_name = "deferred_" + lit->getSpatialName() + "_lit_volume.geo";
    SphereDtoHelper sphere_dto_helper(vol_geo_name);
    sphere_dto_helper.Sphere(MathLib::Vector3::ZERO, lit->GetLightRange(), SPHERE_SLICES, SPHERE_STACKS).BoxBound();
    EffectMaterialDtoHelper eff_dto_helper(m_configuration->LightVolumeEffectName());
    eff_dto_helper.FilenameAtPath(m_configuration->LightVolumePassFxFileName());

    MeshPrimitiveDto mesh_dto;
    mesh_dto.Name() = vol_geo_name;
    mesh_dto.GeometryName() = vol_geo_name;
    mesh_dto.TheGeometry() = sphere_dto_helper.toGenericDto();
    mesh_dto.Effects().emplace_back(eff_dto_helper.toGenericDto());
    mesh_dto.RenderListID() = Renderer::Renderer::RenderListID::DeferredLighting;

    PawnDtoHelper pawn_helper(lit->getSpatialName() + "_lit_volume");
    pawn_helper.Factory(FactoryDesc(LightVolumePawn::TYPE_RTTI.getName())).MeshPrimitive(mesh_dto)
        .TopLevel(true).LocalTransform(lit->getLocalTransform());
    LightingPawnDto lighting_pawn_dto = LightingPawnDto(pawn_helper.ToPawnDto());
    lighting_pawn_dto.HostLightName() = lit->getSpatialName();
    auto pawn_dto = lighting_pawn_dto.toGenericDto();
    auto dtos = { pawn_dto };
    CommandBus::post(std::make_shared<BuildSceneGraph>(lit->getSpatialName(), dtos));
    InsertLightPawnBuildingMeta(pawn_dto.getName(), lit);
}

void DeferredRendererService::InsertLightPawnBuildingMeta(const std::string& pawn_name, const std::shared_ptr<SceneGraph::Light>& lit)
{
    if (!lit) return;
    SceneGraphLightPawnMeta meta;
    if (lit->getParent()) meta.m_parentNodeName = lit->getParent()->getSpatialName();
    meta.m_localTransform = lit->getLocalTransform();
    m_buildingLightPawns.insert({ pawn_name, meta });
}

void DeferredRendererService::RemoveLightingPawn(const std::string& name)
{
    if (const auto it = m_lightingPawns.find(name); it != m_lightingPawns.end())
    {
        m_lightingPawns.erase(it);
    }
}

void DeferredRendererService::UpdateAmbientLightQuad(const std::shared_ptr<SceneGraph::Light>& lit, SceneGraph::LightInfoUpdated::NotifyCode notify)
{
    assert(lit);
    if (notify != LightInfoUpdated::NotifyCode::Color) return;
    const auto& pawn = FindLightingPawn(lit->getSpatialName());
    if (!pawn) return;
    pawn->notifySpatialRenderStateChanged();
}

void DeferredRendererService::UpdatePointLightVolume(const std::shared_ptr<SceneGraph::Light>& lit, SceneGraph::LightInfoUpdated::NotifyCode notify)
{
    assert(lit);
    if (lit->Info().GetLightType() != LightInfo::LightType::Point) return;
    const auto& pawn = FindLightingPawn(lit->getSpatialName());
    if (!pawn) return;
    if (notify == LightInfoUpdated::NotifyCode::Position)
    {
        pawn->changeWorldPosition(lit->GetLightPosition(), std::nullopt);
    }
    else if (notify == LightInfoUpdated::NotifyCode::Range)
    {
        pawn->setLocalUniformScale(lit->GetLightRange());
    }
    else if (notify == LightInfoUpdated::NotifyCode::Enable)
    {
        if (lit->Info().IsEnable())
        {
            pawn->removeSpatialFlag(Spatial::Spatial_Hide);
        }
        else
        {
            pawn->addSpatialFlag(Spatial::Spatial_Hide);
        }
    }
    else if (notify == LightInfoUpdated::NotifyCode::Color)
    {
        pawn->notifySpatialRenderStateChanged();
    }

    CheckLightVolumeBackfaceCulling(lit->getSpatialName());
}

void DeferredRendererService::UpdateSunLightQuad(const std::shared_ptr<SceneGraph::Light>& lit, SceneGraph::LightInfoUpdated::NotifyCode notify)
{
    assert(lit);
    const auto& pawn = FindLightingPawn(lit->getSpatialName());
    if (!pawn) return;
    if ((notify == LightInfoUpdated::NotifyCode::Color) || (notify == LightInfoUpdated::NotifyCode::Direction))
    {
        pawn->notifySpatialRenderStateChanged();
    }
}

void DeferredRendererService::BindGBufferToLightingMesh(const Renderer::MeshPrimitivePtr& mesh)
{
    assert(m_configuration);
    if (!mesh) return;
    if (m_gBuffer.expired()) return;
    if (!m_gBuffer.lock()->GetRenderTargetTexture()) return;

    EffectTextureMap::SegmentEffectTextures textures = {
        { m_configuration->GbufferNormalSemantic(), m_gBuffer.lock()->GetRenderTargetTexture(), m_gBuffer.lock()->FindRenderTextureUsageIndex(Graphics::RenderTextureUsage::Normal) },
        { m_configuration->GbufferDiffuseSemantic(), m_gBuffer.lock()->GetRenderTargetTexture(), m_gBuffer.lock()->FindRenderTextureUsageIndex(Graphics::RenderTextureUsage::Albedo) },
        { m_configuration->GbufferSpecularSemantic(), m_gBuffer.lock()->GetRenderTargetTexture(), m_gBuffer.lock()->FindRenderTextureUsageIndex(Graphics::RenderTextureUsage::Specular) },
        { m_configuration->GbufferDepthSemantic(), m_gBuffer.lock()->GetRenderTargetTexture(), m_gBuffer.lock()->FindRenderTextureUsageIndex(Graphics::RenderTextureUsage::Depth) } };
    if (mesh->GetTextureMapCount() != 0)
    {
        mesh->BindSegmentTextures(textures);
    }
    else
    {
        mesh->ChangeTextureMaps({ textures });
    }
}

void DeferredRendererService::BindGBufferToLightingPawn(const std::shared_ptr<LightingPawn>& lighting_pawn)
{
    if (!lighting_pawn) return;
    auto mesh = std::dynamic_pointer_cast<MeshPrimitive, Primitive>(lighting_pawn->GetPrimitive());
    if (mesh) BindGBufferToLightingMesh(mesh);
}

std::shared_ptr<LightingPawn> DeferredRendererService::FindLightingPawn(const std::string& name)
{
    if (const auto it = m_lightingPawns.find(name); it != m_lightingPawns.end())
    {
        return it->second.lock();
    }
    return nullptr;
}

void DeferredRendererService::CheckLightVolumeBackfaceCulling(const std::string& lit_name)
{
    auto lit_vol = std::dynamic_pointer_cast<LightVolumePawn, LightingPawn>(FindLightingPawn(lit_name));
    if (!lit_vol) return;
    if (m_cameraService.expired()) return;
    auto camera = m_cameraService.lock()->primaryCamera();
    if (!camera) return;
    CheckLightVolumeBackfaceCulling(lit_vol, camera);
}

void DeferredRendererService::CheckLightVolumeBackfaceCulling(const std::shared_ptr<LightVolumePawn>& lit_vol,
    const std::shared_ptr<SceneGraph::Camera>& cam)
{
    if (!lit_vol) return;
    if (!cam) return;
    const BoundingVolume& bv = lit_vol->getWorldBound();
    if (bv.IsEmpty()) return;
    if (bv.PointInside(cam->location()))
    {
        lit_vol->ToggleCameraInside(true);
    }
    else
    {
        lit_vol->ToggleCameraInside(false);
    }
}
