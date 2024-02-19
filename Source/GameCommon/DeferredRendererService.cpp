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
#include "Geometries/StandardGeometryDtoHelper.h"
#include "Geometries/GeometryDataQueries.h"
#include "Renderables/RenderablePrimitiveDtos.h"
#include "Primitives/PrimitiveQueries.h"
#include "Frameworks/EventPublisher.h"
#include "Renderer/RendererEvents.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphDtoHelper.h"
#include "GameCommon/GameCameraService.h"
#include "LightingPawnDto.h"
#include "DeferredRenderingCommands.h"
#include "Renderables/ModelPrimitive.h"
#include "SceneGraph/SceneGraphQueries.h"

using namespace Enigma::GameCommon;
using namespace Enigma::Frameworks;
using namespace Enigma::Renderables;
using namespace Enigma::Engine;
using namespace Enigma::SceneGraph;
using namespace Enigma::Renderer;

DEFINE_RTTI(GameCommon, DeferredRendererService, SceneRendererService);

//!  samples 的數量太少，光球的 poly 太粗，會導致算出的受光亮面偏移
#define SPHERE_STACKS 40
#define SPHERE_SLICES 80

std::string DeferredSunLightQuadName = "_deferred_sun_light_quad_";
std::string DeferredAmbientLightQuadName = "_deferred_ambient_light_quad_";

DeferredRendererService::DeferredRendererService(ServiceManager* mngr,
    const std::shared_ptr<GameSceneService>& scene_service, const std::shared_ptr<GameCameraService>& camera_service,
    const std::shared_ptr<RendererManager>& renderer_manager,
    const std::shared_ptr<DeferredRendererServiceConfiguration>& configuration) : SceneRendererService(mngr, scene_service, camera_service, renderer_manager, configuration)
{
    m_configuration = configuration;
    LightVolumePawn::SetDefaultVisualTech(m_configuration->visualTechniqueNameForCameraDefault());
    LightVolumePawn::SetInsideVisualTech(m_configuration->visualTechniqueNameForCameraInside());
}

DeferredRendererService::~DeferredRendererService()
{
    m_configuration = nullptr;
    m_lightingPawns.clear();
}

ServiceResult DeferredRendererService::onInit()
{
    m_onPrimaryRenderTargetCreated = std::make_shared<EventSubscriber>([=](auto e) { onPrimaryRenderTargetCreated(e); });
    EventPublisher::subscribe(typeid(PrimaryRenderTargetCreated), m_onPrimaryRenderTargetCreated);
    m_onPrimaryRenderTargetResized = std::make_shared<EventSubscriber>([=](auto e) { onPrimaryRenderTargetResized(e); });
    EventPublisher::subscribe(typeid(RenderTargetResized), m_onPrimaryRenderTargetResized);
    m_onGameCameraUpdated = std::make_shared<EventSubscriber>([=](auto e) { onGameCameraUpdated(e); });
    EventPublisher::subscribe(typeid(GameCameraUpdated), m_onGameCameraUpdated);
    m_onSceneGraphChanged = std::make_shared<EventSubscriber>([=](auto e) { onSceneGraphChanged(e); });
    EventPublisher::subscribe(typeid(SceneGraphChanged), m_onSceneGraphChanged);
    m_onGBufferTextureCreated = std::make_shared<EventSubscriber>([=](auto e) { onGBufferTextureCreated(e); });
    EventPublisher::subscribe(typeid(RenderTargetTextureCreated), m_onGBufferTextureCreated);
    m_onGameLightCreated = std::make_shared<EventSubscriber>([=](auto e) { onGameLightCreated(e); });
    EventPublisher::subscribe(typeid(GameLightCreated), m_onGameLightCreated);
    m_onLightInfoDeleted = std::make_shared<EventSubscriber>([=](auto e) { onLightInfoDeleted(e); });
    EventPublisher::subscribe(typeid(LightInfoDeleted), m_onLightInfoDeleted);
    m_onLightInfoUpdated = std::make_shared<EventSubscriber>([=](auto e) { onLightInfoUpdated(e); });
    EventPublisher::subscribe(typeid(LightInfoUpdated), m_onLightInfoUpdated);
    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { onSceneGraphBuilt(e); });
    EventPublisher::subscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onLightingPawnCreated = std::make_shared<EventSubscriber>([=](auto e) { onLightingPawnCreated(e); });
    EventPublisher::subscribe(typeid(FactorySpatialCreated), m_onLightingPawnCreated);
    m_onPawnPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { onPawnPrimitiveBuilt(e); });
    EventPublisher::subscribe(typeid(PawnPrimitiveBuilt), m_onPawnPrimitiveBuilt);

    m_bindGBuffer = std::make_shared<CommandSubscriber>([=](auto c) { bindGBuffer(c); });
    CommandBus::subscribe(typeid(BindGBuffer), m_bindGBuffer);

    return SceneRendererService::onInit();
}

ServiceResult DeferredRendererService::onTerm()
{
    EventPublisher::unsubscribe(typeid(PrimaryRenderTargetCreated), m_onPrimaryRenderTargetCreated);
    m_onPrimaryRenderTargetCreated = nullptr;
    EventPublisher::unsubscribe(typeid(RenderTargetResized), m_onPrimaryRenderTargetResized);
    m_onPrimaryRenderTargetResized = nullptr;
    EventPublisher::unsubscribe(typeid(GameCameraUpdated), m_onGameCameraUpdated);
    m_onGameCameraUpdated = nullptr;
    EventPublisher::unsubscribe(typeid(SceneGraphChanged), m_onSceneGraphChanged);
    m_onSceneGraphChanged = nullptr;
    EventPublisher::unsubscribe(typeid(RenderTargetTextureCreated), m_onGBufferTextureCreated);
    m_onGBufferTextureCreated = nullptr;
    EventPublisher::unsubscribe(typeid(GameLightCreated), m_onGameLightCreated);
    m_onGameLightCreated = nullptr;
    EventPublisher::unsubscribe(typeid(LightInfoDeleted), m_onLightInfoDeleted);
    m_onLightInfoDeleted = nullptr;
    EventPublisher::unsubscribe(typeid(LightInfoUpdated), m_onLightInfoUpdated);
    m_onLightInfoUpdated = nullptr;
    EventPublisher::unsubscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphBuilt = nullptr;
    EventPublisher::unsubscribe(typeid(FactorySpatialCreated), m_onLightingPawnCreated);
    m_onLightingPawnCreated = nullptr;
    EventPublisher::unsubscribe(typeid(PawnPrimitiveBuilt), m_onPawnPrimitiveBuilt);
    m_onPawnPrimitiveBuilt = nullptr;

    CommandBus::unsubscribe(typeid(BindGBuffer), m_bindGBuffer);
    m_bindGBuffer = nullptr;

    m_lightingPawns.clear();

    return SceneRendererService::onTerm();
}

void DeferredRendererService::createSceneRenderSystem(const std::string& renderer_name, const std::string& target_name)
{
    assert(!m_rendererManager.expired());
    assert(m_configuration);
    auto rendererManager = m_rendererManager.lock();
    IRendererPtr renderer = std::make_shared<DeferredRenderer>(renderer_name);
    error er = rendererManager->InsertRenderer(renderer_name, renderer);
    if (er) return;

    RenderTargetPtr primaryTarget = rendererManager->GetPrimaryRenderTarget();
    if (!primaryTarget)
    {
        rendererManager->CreateRenderTarget(target_name, RenderTarget::PrimaryType::IsPrimary, { Graphics::RenderTextureUsage::Default });
        primaryTarget = rendererManager->GetPrimaryRenderTarget();
    }
    if (FATAL_LOG_EXPR(!primaryTarget)) return;

    m_renderer = std::dynamic_pointer_cast<Renderer::Renderer, IRenderer>(rendererManager->GetRenderer(renderer_name));
    m_renderer.lock()->selectRendererTechnique(m_configuration->deferredRendererTechniqueName());
    m_renderer.lock()->SetRenderTarget(primaryTarget);
    if (primaryTarget->GetBackSurface() && primaryTarget->GetDepthStencilSurface())
    {
        createGBuffer(primaryTarget);
    }
}

void DeferredRendererService::destroySceneRenderSystem(const std::string& renderer_name, const std::string& target_name)
{
    assert(!m_rendererManager.expired());
    assert(m_configuration);
    m_rendererManager.lock()->DestroyRenderTarget(target_name);
    m_rendererManager.lock()->DestroyRenderTarget(m_configuration->gbufferTargetName());
    m_rendererManager.lock()->DestroyRenderer(renderer_name);
}

void DeferredRendererService::prepareGameScene()
{
    SceneRendererService::prepareGameScene();
}

void DeferredRendererService::createGBuffer(const RenderTargetPtr& primary_target)
{
    assert(primary_target);
    assert(!m_rendererManager.expired());
    assert(m_configuration);

    auto [width, height] = primary_target->GetDimension();

    m_rendererManager.lock()->CreateRenderTarget(m_configuration->gbufferTargetName(), RenderTarget::PrimaryType::NotPrimary,
        m_configuration->gbufferUsages());
    m_gBuffer = m_rendererManager.lock()->GetRenderTarget(m_configuration->gbufferTargetName());
    if (!m_gBuffer.expired())
    {
        m_gBuffer.lock()->InitMultiBackSurface(m_configuration->gbufferSurfaceName(), MathLib::Dimension<unsigned>{ width, height }, static_cast<unsigned>(m_configuration->gbufferFormats().size()),
            m_configuration->gbufferFormats());
        m_gBuffer.lock()->ShareDepthStencilSurface(m_configuration->gbufferDepthName(), primary_target->GetDepthStencilSurface());
    }
    if (const auto deferRender = std::dynamic_pointer_cast<DeferredRenderer, Renderer::Renderer>(m_renderer.lock()))
    {
        deferRender->AttachGBufferTarget(m_gBuffer.lock());
    }
    for (auto& light : m_lightingPawns)
    {
        if (!light.second.expired()) bindGBufferToLightingPawn(light.second.lock());
    }
}

void DeferredRendererService::onPrimaryRenderTargetCreated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PrimaryRenderTargetCreated, IEvent>(e);
    if (!ev) return;
    auto primaryTarget = ev->GetRenderTarget();
    if (!primaryTarget) return;

    createGBuffer(primaryTarget);
}

void DeferredRendererService::onPrimaryRenderTargetResized(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RenderTargetResized, IEvent>(e);
    if (!ev) return;
    const auto target = ev->GetRenderTarget();
    if ((!target) || (!target->IsPrimary())) return;

    if ((!m_gBuffer.expired()) && (m_gBuffer.lock()->GetDimension() != target->GetDimension())) m_gBuffer.lock()->Resize(target->GetDimension());
}

void DeferredRendererService::onGameCameraUpdated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<GameCameraUpdated, IEvent>(e);
    if (!ev) return;
    if (!ev->GetCamera()) return;
    for (auto& kv : m_lightingPawns)
    {
        if (kv.second.expired()) continue;
        if (auto volume = std::dynamic_pointer_cast<LightVolumePawn, LightingPawn>(kv.second.lock()))
        {
            checkLightVolumeBackfaceCulling(volume, ev->GetCamera());
        }
    }
}

void DeferredRendererService::onSceneGraphChanged(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneGraphChanged, IEvent>(e);
    if ((!ev) || (ev->GetChild())) return;
    // 抓light entity 被 attach 的訊息來改變 light volume 的 parent
    if (!ev->GetChild()->typeInfo().isDerived(Light::TYPE_RTTI)) return;
    if (ev->GetNotifyCode() != SceneGraphChanged::NotifyCode::AttachChild) return;
    const auto light = std::dynamic_pointer_cast<Light, Spatial>(ev->GetChild());
    if (!light) return;
    const auto lightPawn = findLightingPawn(light->getSpatialName());
    auto parent_node = std::dynamic_pointer_cast<Node, Spatial>(ev->GetParentNode());
    if (lightPawn) lightPawn->changeWorldPosition(lightPawn->getWorldPosition(), parent_node);
}

void DeferredRendererService::onGBufferTextureCreated(const IEventPtr& e)
{
    assert(!m_gBuffer.expired());
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RenderTargetTextureCreated, IEvent>(e);
    if (!ev) return;
    if (ev->GetRenderTarget() != m_gBuffer.lock()) return;
    for (auto& light : m_lightingPawns)
    {
        if (!light.second.expired()) bindGBufferToLightingPawn(light.second.lock());
    }
}

void DeferredRendererService::onGameLightCreated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<GameLightCreated, IEvent>(e);
    if ((!ev) || (!ev->GetLight())) return;
    if (ev->GetLight()->Info().GetLightType() == LightInfo::LightType::Ambient)
    {
        createAmbientLightQuad(ev->GetLight());
    }
    else if (ev->GetLight()->Info().GetLightType() == LightInfo::LightType::SunLight)
    {
        createSunLightQuad(ev->GetLight());
    }
    else if (ev->GetLight()->Info().GetLightType() == LightInfo::LightType::Point)
    {
        createPointLightVolume(ev->GetLight());
    }
}

void DeferredRendererService::onLightInfoDeleted(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<LightInfoDeleted, IEvent>(e);
    if (!ev) return;
    removeLightingPawn(ev->GetLightName());
    CommandBus::post(std::make_shared<DeleteSceneSpatial>(ev->GetLightName()));
}

void DeferredRendererService::onLightInfoUpdated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<LightInfoUpdated, IEvent>(e);
    if ((!ev) || (!ev->GetLight())) return;
    if (ev->GetLight()->Info().GetLightType() == LightInfo::LightType::Ambient)
    {
        updateAmbientLightQuad(ev->GetLight(), ev->GetNotifyCode());
    }
    else if (ev->GetLight()->Info().GetLightType() == LightInfo::LightType::SunLight)
    {
        updateSunLightQuad(ev->GetLight(), ev->GetNotifyCode());
    }
    else if (ev->GetLight()->Info().GetLightType() == LightInfo::LightType::Point)
    {
        updatePointLightVolume(ev->GetLight(), ev->GetNotifyCode());
    }
}

void DeferredRendererService::onSceneGraphBuilt(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt, IEvent>(e);
    if (!ev) return;
    auto top_spatials = ev->GetTopLevelSpatial();
    if (top_spatials.empty()) return;
    if (!top_spatials[0]) return;
    auto lighting_pawn = std::dynamic_pointer_cast<LightingPawn, Spatial>(top_spatials[0]);
    if (!lighting_pawn) return;
    if (const auto lit = m_buildingLightPawns.find(top_spatials[0]->id()); lit != m_buildingLightPawns.end())
    {
        if (!lit->second.m_parentNodeName.empty())
        {
            CommandBus::post(std::make_shared<AttachNodeChild>(lit->second.m_parentNodeName, top_spatials[0], lit->second.m_localTransform));
        }
        m_buildingLightPawns.erase(lit);
    }
}

void DeferredRendererService::onLightingPawnCreated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<FactorySpatialCreated, IEvent>(e);
    if (!ev) return;
    auto lighting_pawn = std::dynamic_pointer_cast<LightingPawn, Spatial>(ev->GetSpatial());
    if (!lighting_pawn) return;
    onLightingPawnBuilt(lighting_pawn->GetHostLightName(), lighting_pawn);
}

void DeferredRendererService::onLightingPawnBuilt(const std::string& lit_name, const std::shared_ptr<LightingPawn>& lighting_pawn)
{
    if (!lighting_pawn) return;
    m_lightingPawns.insert_or_assign(lit_name, lighting_pawn);
    bindGBufferToLightingPawn(lighting_pawn);
    checkLightVolumeBackfaceCulling(lit_name);
}

void DeferredRendererService::onPawnPrimitiveBuilt(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PawnPrimitiveBuilt, IEvent>(e);
    if ((!ev) || (!ev->GetPawn())) return;
    auto lighting_pawn = std::dynamic_pointer_cast<LightingPawn, Pawn>(ev->GetPawn());
    if (!lighting_pawn) return;
    bindGBufferToLightingPawn(lighting_pawn);
    lighting_pawn->notifySpatialRenderStateChanged();
    checkLightVolumeBackfaceCulling(lighting_pawn->GetHostLightName());
}

void DeferredRendererService::bindGBuffer(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<BindGBuffer>(c);
    if (!cmd) return;
    if ((!cmd->GetPawn()) || (!cmd->GetPawn()->GetPrimitive())) return;
    if (const auto model = std::dynamic_pointer_cast<ModelPrimitive>(cmd->GetPawn()->GetPrimitive()))
    {
        const auto mesh_count = model->getMeshPrimitiveCount();
        for (unsigned i = 0; i < mesh_count; i++)
        {
            if (auto mesh = model->getMeshPrimitive(i))
            {
                bindGBufferToLightingMesh(mesh);
            }
        }
    }
    else if (const auto mesh = std::dynamic_pointer_cast<MeshPrimitive>(cmd->GetPawn()->GetPrimitive()))
    {
        bindGBufferToLightingMesh(mesh);
    }
}

void DeferredRendererService::createAmbientLightQuad(const std::shared_ptr<Light>& lit)
{
    assert(lit);
    m_ambientLightQuadId = Geometries::GeometryId(lit->getSpatialName() + "_lit_quad" + ".geo");
    auto quad_geo = Geometries::GeometryData::queryGeometryData(m_ambientLightQuadId);
    if (!quad_geo)
    {
        Geometries::SquareQuadDtoHelper quad_dto_helper(m_ambientLightQuadId);
        quad_dto_helper.xyQuad(MathLib::Vector3(-1.0f, -1.0f, 0.5f), MathLib::Vector3(1.0f, 1.0f, 0.5f))
            .textureCoord(MathLib::Vector2(0.0f, 1.0f), MathLib::Vector2(1.0f, 0.0f));
        quad_geo = std::make_shared<Geometries::RequestGeometryConstitution>(m_ambientLightQuadId, quad_dto_helper.toGenericDto(), Geometries::PersistenceLevel::Repository)->dispatch();
    }
    m_ambientLightMeshId = Primitives::PrimitiveId(lit->getSpatialName() + "_lit_quad", MeshPrimitive::TYPE_RTTI);
    auto lit_mesh = Primitives::Primitive::queryPrimitive(m_ambientLightMeshId);
    if (!lit_mesh)
    {
        MeshPrimitiveDto mesh_dto;
        mesh_dto.id() = m_ambientLightMeshId;
        mesh_dto.factoryDesc() = FactoryDesc(MeshPrimitive::TYPE_RTTI.getName()).ClaimAsNative(mesh_dto.id().name() + ".mesh@DataPath");
        mesh_dto.geometryId() = m_ambientLightQuadId;
        mesh_dto.effects().emplace_back(m_configuration->ambientEffect());
        mesh_dto.renderListID() = Renderer::Renderer::RenderListID::DeferredLighting;
        lit_mesh = std::make_shared<Primitives::RequestPrimitiveConstitution>(m_ambientLightMeshId, mesh_dto.toGenericDto(), Primitives::PersistenceLevel::Repository)->dispatch();
    }
    m_ambientLightPawnId = SpatialId(lit->getSpatialName() + "_lit_quad", LightQuadPawn::TYPE_RTTI);
    auto lit_pawn = std::make_shared<QuerySpatial>(m_ambientLightPawnId)->dispatch();
    if (!lit_pawn)
    {
        PawnDtoHelper pawn_helper(m_ambientLightPawnId);
        pawn_helper.primitive(m_ambientLightMeshId).topLevel(true).factory(FactoryDesc(LightQuadPawn::TYPE_RTTI.getName()));
        LightingPawnDto lighting_pawn_dto = LightingPawnDto(pawn_helper.toPawnDto());
        lighting_pawn_dto.id() = m_ambientLightPawnId;
        lighting_pawn_dto.HostLightName() = lit->getSpatialName();
        auto pawn_dto = lighting_pawn_dto.toGenericDto();
        lit_pawn = std::make_shared<RequestSpatialConstitution>(m_ambientLightPawnId, pawn_dto, SceneGraph::PersistenceLevel::Repository)->dispatch();
        if (lit_pawn)
        {
            if (const auto parent = std::dynamic_pointer_cast<Node>(lit->getParent()))
            {
                parent->attachChild(lit_pawn, lit->getLocalTransform());
            }
        }
    }
    /*PawnDtoHelper pawn_helper(lit->getSpatialName() + "_lit_quad");
    pawn_helper.primitive(mesh_dto.id())
        .topLevel(true)
        .factory(FactoryDesc(LightQuadPawn::TYPE_RTTI.getName()));
    LightingPawnDto lighting_pawn_dto = LightingPawnDto(pawn_helper.toPawnDto());
    auto pawn_id = SpatialId(lighting_pawn_dto.name(), LightQuadPawn::TYPE_RTTI);
    lighting_pawn_dto.id() = pawn_id;
    lighting_pawn_dto.HostLightName() = lit->getSpatialName();
    auto pawn_dto = lighting_pawn_dto.toGenericDto();
    auto dtos = { pawn_dto };
    CommandBus::post(std::make_shared<BuildSceneGraph>(lit->getSpatialName(), dtos));
    insertLightPawnBuildingMeta(pawn_id, lit);*/
}

void DeferredRendererService::createSunLightQuad(const std::shared_ptr<Light>& lit)
{
    assert(lit);
    std::string quad_geo_name = lit->getSpatialName() + "_lit_quad" + ".geo";
    Geometries::SquareQuadDtoHelper quad_dto_helper(quad_geo_name);
    quad_dto_helper.xyQuad(MathLib::Vector3(-1.0f, -1.0f, 0.5f), MathLib::Vector3(1.0f, 1.0f, 0.5f))
        .textureCoord(MathLib::Vector2(0.0f, 1.0f), MathLib::Vector2(1.0f, 0.0f));

    MeshPrimitiveDto mesh_dto;
    mesh_dto.id() = Primitives::PrimitiveId(lit->getSpatialName() + "_lit_quad", MeshPrimitive::TYPE_RTTI);
    mesh_dto.factoryDesc() = FactoryDesc(MeshPrimitive::TYPE_RTTI.getName()).ClaimAsNative(mesh_dto.id().name() + ".mesh@DataPath");
    mesh_dto.geometryId() = quad_geo_name;
    mesh_dto.geometry() = quad_dto_helper.toGenericDto();
    mesh_dto.effects().emplace_back(m_configuration->sunLightEffect());
    mesh_dto.renderListID() = Renderer::Renderer::RenderListID::DeferredLighting;

    PawnDtoHelper pawn_helper(SpatialId(lit->getSpatialName() + "_lit_quad", LightQuadPawn::TYPE_RTTI));
    pawn_helper.primitive(mesh_dto.id())
        .spatialFlags(m_configuration->sunLightSpatialFlags()).topLevel(true)
        .factory(FactoryDesc(LightQuadPawn::TYPE_RTTI.getName()));
    LightingPawnDto lighting_pawn_dto = LightingPawnDto(pawn_helper.toPawnDto());
    auto pawn_id = SpatialId(lighting_pawn_dto.name(), LightQuadPawn::TYPE_RTTI);
    lighting_pawn_dto.id() = pawn_id;
    lighting_pawn_dto.HostLightName() = lit->getSpatialName();
    auto pawn_dto = lighting_pawn_dto.toGenericDto();
    auto lit_pawn = std::make_shared<RequestSpatialConstitution>(pawn_id, pawn_dto, SceneGraph::PersistenceLevel::Repository)->dispatch();
    //CommandBus::post(std::make_shared<ConstituteSpatial>(pawn_id, pawn_dto));
    //auto dtos = { pawn_dto };
    //CommandBus::post(std::make_shared<BuildSceneGraph>(lit->getSpatialName(), dtos));
    insertLightPawnBuildingMeta(pawn_id, lit);
}

void DeferredRendererService::createPointLightVolume(const std::shared_ptr<Light>& lit)
{
    assert(lit);
    std::string vol_geo_name = "deferred_" + lit->getSpatialName() + "_lit_volume.geo";
    Geometries::SphereDtoHelper sphere_dto_helper(vol_geo_name);
    sphere_dto_helper.sphere(MathLib::Vector3::ZERO, lit->GetLightRange(), SPHERE_SLICES, SPHERE_STACKS).boxBound();

    MeshPrimitiveDto mesh_dto;
    mesh_dto.id() = Primitives::PrimitiveId(lit->getSpatialName() + "_lit_volume", MeshPrimitive::TYPE_RTTI);
    mesh_dto.factoryDesc() = FactoryDesc(MeshPrimitive::TYPE_RTTI.getName()).ClaimAsNative(mesh_dto.id().name() + ".mesh@DataPath");
    mesh_dto.geometryId() = vol_geo_name;
    mesh_dto.geometry() = sphere_dto_helper.toGenericDto();
    mesh_dto.effects().emplace_back(m_configuration->lightVolumeEffect());
    mesh_dto.renderListID() = Renderer::Renderer::RenderListID::DeferredLighting;

    PawnDtoHelper pawn_helper(SpatialId(lit->getSpatialName() + "_lit_volume", LightVolumePawn::TYPE_RTTI));
    pawn_helper.factory(FactoryDesc(LightVolumePawn::TYPE_RTTI.getName())).primitive(mesh_dto.id())
        .topLevel(true).localTransform(lit->getLocalTransform());
    LightingPawnDto lighting_pawn_dto = LightingPawnDto(pawn_helper.toPawnDto());
    auto pawn_id = SpatialId(lighting_pawn_dto.name(), LightVolumePawn::TYPE_RTTI);
    lighting_pawn_dto.id() = pawn_id;
    lighting_pawn_dto.HostLightName() = lit->getSpatialName();
    auto pawn_dto = lighting_pawn_dto.toGenericDto();
    auto lit_pawn = std::make_shared<RequestSpatialConstitution>(pawn_id, pawn_dto, SceneGraph::PersistenceLevel::Repository)->dispatch();
    //CommandBus::post(std::make_shared<ConstituteSpatial>(pawn_id, pawn_dto));
    //auto dtos = { pawn_dto };
    //CommandBus::post(std::make_shared<BuildSceneGraph>(lit->getSpatialName(), dtos));
    insertLightPawnBuildingMeta(pawn_id, lit);
}

void DeferredRendererService::insertLightPawnBuildingMeta(const SceneGraph::SpatialId& pawn_id, const std::shared_ptr<Light>& lit)
{
    if (!lit) return;
    SceneGraphLightPawnMeta meta;
    if (lit->getParent()) meta.m_parentNodeName = lit->getParent()->getSpatialName();
    meta.m_localTransform = lit->getLocalTransform();
    m_buildingLightPawns.insert({ pawn_id, meta });
}

void DeferredRendererService::removeLightingPawn(const std::string& name)
{
    if (const auto it = m_lightingPawns.find(name); it != m_lightingPawns.end())
    {
        m_lightingPawns.erase(it);
    }
}

void DeferredRendererService::updateAmbientLightQuad(const std::shared_ptr<Light>& lit, LightInfoUpdated::NotifyCode notify)
{
    assert(lit);
    if (notify != LightInfoUpdated::NotifyCode::Color) return;
    const auto& pawn = findLightingPawn(lit->getSpatialName());
    if (!pawn) return;
    pawn->notifySpatialRenderStateChanged();
}

void DeferredRendererService::updatePointLightVolume(const std::shared_ptr<Light>& lit, LightInfoUpdated::NotifyCode notify)
{
    assert(lit);
    if (lit->Info().GetLightType() != LightInfo::LightType::Point) return;
    const auto& pawn = findLightingPawn(lit->getSpatialName());
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

    checkLightVolumeBackfaceCulling(lit->getSpatialName());
}

void DeferredRendererService::updateSunLightQuad(const std::shared_ptr<Light>& lit, LightInfoUpdated::NotifyCode notify)
{
    assert(lit);
    const auto& pawn = findLightingPawn(lit->getSpatialName());
    if (!pawn) return;
    if ((notify == LightInfoUpdated::NotifyCode::Color) || (notify == LightInfoUpdated::NotifyCode::Direction))
    {
        pawn->notifySpatialRenderStateChanged();
    }
}

void DeferredRendererService::bindGBufferToLightingMesh(const std::shared_ptr<MeshPrimitive>& mesh)
{
    assert(m_configuration);
    if (!mesh) return;
    if (m_gBuffer.expired()) return;
    if (!m_gBuffer.lock()->GetRenderTargetTexture()) return;

    EffectTextureMap::SegmentEffectTextures textures = {
        { m_configuration->gbufferNormalSemantic(), m_gBuffer.lock()->GetRenderTargetTexture(), m_gBuffer.lock()->FindRenderTextureUsageIndex(Graphics::RenderTextureUsage::Normal) },
        { m_configuration->gbufferDiffuseSemantic(), m_gBuffer.lock()->GetRenderTargetTexture(), m_gBuffer.lock()->FindRenderTextureUsageIndex(Graphics::RenderTextureUsage::Albedo) },
        { m_configuration->gbufferSpecularSemantic(), m_gBuffer.lock()->GetRenderTargetTexture(), m_gBuffer.lock()->FindRenderTextureUsageIndex(Graphics::RenderTextureUsage::Specular) },
        { m_configuration->gbufferDepthSemantic(), m_gBuffer.lock()->GetRenderTargetTexture(), m_gBuffer.lock()->FindRenderTextureUsageIndex(Graphics::RenderTextureUsage::Depth) } };
    if (mesh->getTextureMapCount() != 0)
    {
        mesh->bindSemanticTextures(textures);
    }
    else
    {
        mesh->changeTextureMaps({ textures });
    }
}

void DeferredRendererService::bindGBufferToLightingPawn(const std::shared_ptr<LightingPawn>& lighting_pawn)
{
    if (!lighting_pawn) return;
    auto mesh = std::dynamic_pointer_cast<MeshPrimitive>(lighting_pawn->GetPrimitive());
    if (mesh) bindGBufferToLightingMesh(mesh);
}

std::shared_ptr<LightingPawn> DeferredRendererService::findLightingPawn(const std::string& name)
{
    if (const auto it = m_lightingPawns.find(name); it != m_lightingPawns.end())
    {
        return it->second.lock();
    }
    return nullptr;
}

void DeferredRendererService::checkLightVolumeBackfaceCulling(const std::string& lit_name)
{
    auto lit_vol = std::dynamic_pointer_cast<LightVolumePawn, LightingPawn>(findLightingPawn(lit_name));
    if (!lit_vol) return;
    if (m_cameraService.expired()) return;
    auto camera = m_cameraService.lock()->primaryCamera();
    if (!camera) return;
    checkLightVolumeBackfaceCulling(lit_vol, camera);
}

void DeferredRendererService::checkLightVolumeBackfaceCulling(const std::shared_ptr<LightVolumePawn>& lit_vol,
    const std::shared_ptr<Camera>& cam)
{
    if (!lit_vol) return;
    if (!cam) return;
    const BoundingVolume& bv = lit_vol->getWorldBound();
    if (bv.isEmpty()) return;
    if (bv.PointInside(cam->location()))
    {
        lit_vol->ToggleCameraInside(true);
    }
    else
    {
        lit_vol->ToggleCameraInside(false);
    }
}
