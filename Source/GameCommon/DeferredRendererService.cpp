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
#include "Geometries/StandardGeometryAssemblers.h"
#include "Geometries/GeometryDataQueries.h"
#include "Renderables/RenderablePrimitiveAssembler.h"
#include "Primitives/PrimitiveQueries.h"
#include "Frameworks/EventPublisher.h"
#include "Renderer/RendererEvents.h"
#include "SceneGraph/SceneGraphAssemblers.h"
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
    LightVolumePawn::setDefaultVisualTech(m_configuration->visualTechniqueNameForCameraDefault());
    LightVolumePawn::setInsideVisualTech(m_configuration->visualTechniqueNameForCameraInside());
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

    m_onGameLightCreated = std::make_shared<EventSubscriber>([=](auto e) { onGameLightCreated(e); });
    EventPublisher::subscribe(typeid(GameLightCreated), m_onGameLightCreated);
    m_onLightInfoDeleted = std::make_shared<EventSubscriber>([=](auto e) { onLightInfoDeleted(e); });
    EventPublisher::subscribe(typeid(LightInfoDeleted), m_onLightInfoDeleted);
    m_onLightInfoUpdated = std::make_shared<EventSubscriber>([=](auto e) { onLightInfoUpdated(e); });
    EventPublisher::subscribe(typeid(LightInfoUpdated), m_onLightInfoUpdated);

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

    EventPublisher::unsubscribe(typeid(GameLightCreated), m_onGameLightCreated);
    m_onGameLightCreated = nullptr;
    EventPublisher::unsubscribe(typeid(LightInfoDeleted), m_onLightInfoDeleted);
    m_onLightInfoDeleted = nullptr;
    EventPublisher::unsubscribe(typeid(LightInfoUpdated), m_onLightInfoUpdated);
    m_onLightInfoUpdated = nullptr;
    m_lightingPawns.clear();

    return SceneRendererService::onTerm();
}

void DeferredRendererService::createSceneRenderSystem(const std::string& renderer_name, const std::string& target_name)
{
    assert(!m_rendererManager.expired());
    assert(m_configuration);
    auto rendererManager = m_rendererManager.lock();
    IRendererPtr renderer = std::make_shared<DeferredRenderer>(renderer_name);
    error er = rendererManager->insertRenderer(renderer_name, renderer);
    if (er) return;

    RenderTargetPtr primaryTarget = rendererManager->getPrimaryRenderTarget();
    if (!primaryTarget)
    {
        rendererManager->createRenderTarget(target_name, RenderTarget::PrimaryType::IsPrimary, { Graphics::RenderTextureUsage::Default });
        primaryTarget = rendererManager->getPrimaryRenderTarget();
    }
    if (FATAL_LOG_EXPR(!primaryTarget)) return;

    m_renderer = std::dynamic_pointer_cast<Renderer::Renderer, IRenderer>(rendererManager->getRenderer(renderer_name));
    m_renderer.lock()->selectRendererTechnique(m_configuration->deferredRendererTechniqueName());
    m_renderer.lock()->setRenderTarget(primaryTarget);
    if (primaryTarget->getBackSurface() && primaryTarget->getDepthStencilSurface())
    {
        createGBuffer(primaryTarget);
    }
}

void DeferredRendererService::destroySceneRenderSystem(const std::string& renderer_name, const std::string& target_name)
{
    assert(!m_rendererManager.expired());
    assert(m_configuration);
    m_rendererManager.lock()->destroyRenderTarget(target_name);
    m_rendererManager.lock()->destroyRenderTarget(m_configuration->gbufferTargetName());
    m_rendererManager.lock()->destroyRenderer(renderer_name);
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

    auto [width, height] = primary_target->getDimension();

    m_rendererManager.lock()->createRenderTarget(m_configuration->gbufferTargetName(), m_configuration->gbufferTextureId(), Graphics::MultiBackSurfaceSpecification(m_configuration->gbufferSurfaceName(), MathLib::Dimension<unsigned>{ width, height }, static_cast<unsigned>(m_configuration->gbufferFormats().size()), m_configuration->gbufferFormats()), m_configuration->gbufferDepthName(), primary_target->getDepthStencilSurface(), m_configuration->gbufferUsages());
    m_gBuffer = m_rendererManager.lock()->getRenderTarget(m_configuration->gbufferTargetName());
    if (const auto deferRender = std::dynamic_pointer_cast<DeferredRenderer, Renderer::Renderer>(m_renderer.lock()))
    {
        deferRender->attachGBufferTarget(m_gBuffer.lock());
    }
    bindGBufferToPendingLights();
}

void DeferredRendererService::onPrimaryRenderTargetCreated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PrimaryRenderTargetCreated, IEvent>(e);
    if (!ev) return;
    auto primaryTarget = ev->renderTarget();
    if (!primaryTarget) return;

    createGBuffer(primaryTarget);
}

void DeferredRendererService::onPrimaryRenderTargetResized(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RenderTargetResized, IEvent>(e);
    if (!ev) return;
    const auto target = ev->renderTarget();
    if ((!target) || (!target->isPrimary())) return;

    if ((!m_gBuffer.expired()) && (m_gBuffer.lock()->getDimension() != target->getDimension())) m_gBuffer.lock()->resize(target->getDimension());
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
    if (!ev) return;
    // 抓light entity 被 attach 的訊息來改變 light volume 的 parent
    if (!ev->childId().rtti().isDerived(Light::TYPE_RTTI)) return;
    if (ev->notifyCode() != SceneGraphChanged::NotifyCode::AttachChild) return;
    const auto light = std::dynamic_pointer_cast<Light, Spatial>(Spatial::querySpatial(ev->childId()));
    if (!light) return;
    const auto lightPawn = findLightingPawn(light->id());
    auto parent_node = Node::queryNode(ev->parentId());
    if (lightPawn) lightPawn->changeWorldPosition(lightPawn->getWorldPosition(), parent_node);
}

void DeferredRendererService::onGameLightCreated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<GameLightCreated, IEvent>(e);
    if ((!ev) || (!ev->light())) return;
    if (m_gBuffer.expired())
    {
        m_pendingLightsOfGBufferBind.push_back(ev->light());
        return;
    }
    if (ev->light()->info().lightType() == LightInfo::LightType::Ambient)
    {
        createAmbientLightQuad(ev->light());
    }
    else if (ev->light()->info().lightType() == LightInfo::LightType::SunLight)
    {
        createSunLightQuad(ev->light());
    }
    else if (ev->light()->info().lightType() == LightInfo::LightType::Point)
    {
        createPointLightVolume(ev->light());
    }
}

void DeferredRendererService::onLightInfoDeleted(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<LightInfoDeleted, IEvent>(e);
    if (!ev) return;
    SpatialId pawn_id;
    if (auto pawn = findLightingPawn(ev->lightId())) pawn_id = pawn->id();

    removeLightingPawn(ev->lightId());
    CommandBus::post(std::make_shared<DeleteSceneSpatial>(pawn_id));
}

void DeferredRendererService::onLightInfoUpdated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<LightInfoUpdated, IEvent>(e);
    if ((!ev) || (!ev->light())) return;
    if (ev->light()->info().lightType() == LightInfo::LightType::Ambient)
    {
        updateAmbientLightQuad(ev->light(), ev->notifyCode());
    }
    else if (ev->light()->info().lightType() == LightInfo::LightType::SunLight)
    {
        updateSunLightQuad(ev->light(), ev->notifyCode());
    }
    else if (ev->light()->info().lightType() == LightInfo::LightType::Point)
    {
        updatePointLightVolume(ev->light(), ev->notifyCode());
    }
}

void DeferredRendererService::completeLightingPawnBuilt(const SpatialId& lit_id, const std::shared_ptr<LightingPawn>& lighting_pawn)
{
    if (!lighting_pawn) return;
    m_lightingPawns.insert_or_assign(lit_id, lighting_pawn);
    checkLightVolumeBackfaceCulling(lit_id);
    lighting_pawn->notifySpatialRenderStateChanged();
}

void DeferredRendererService::createAmbientLightQuad(const std::shared_ptr<Light>& lit)
{
    assert(lit);
    const auto amb_quad_id = Geometries::GeometryId(lit->getSpatialName() + "_lit_quad" + ".geo");
    auto quad_geo = Geometries::GeometryData::queryGeometryData(amb_quad_id);
    if (!quad_geo)
    {
        quad_geo = Geometries::SquareQuadAssembler(amb_quad_id).xyQuad(MathLib::Vector3(-1.0f, -1.0f, 0.5f), MathLib::Vector3(1.0f, 1.0f, 0.5f)).textureCoord(MathLib::Vector2(0.0f, 1.0f), MathLib::Vector2(1.0f, 0.0f)).constitute(Geometries::PersistenceLevel::Repository);
    }
    const auto amb_mesh_id = Primitives::PrimitiveId(lit->getSpatialName() + "_lit_quad", MeshPrimitive::TYPE_RTTI);
    auto lit_mesh = Primitives::Primitive::queryPrimitive(amb_mesh_id);
    if (!lit_mesh)
    {
        lit_mesh = Renderables::MeshPrimitiveAssembler(amb_mesh_id).geometryId(amb_quad_id).asNative(amb_mesh_id.name() + ".mesh@DataPath").effect(m_configuration->ambientEffect()).textureMap(getGBufferTextureSemantics()).renderListID(Renderer::Renderer::RenderListID::DeferredLighting).constitute(Primitives::PersistenceLevel::Repository);
    }
    const auto amb_pawn_id = SpatialId(lit->getSpatialName() + "_lit_quad", LightQuadPawn::TYPE_RTTI);
    auto lit_pawn = std::make_shared<QuerySpatial>(amb_pawn_id)->dispatch();
    if (!lit_pawn)
    {
        PawnAssembler pawn_assembler(amb_pawn_id);
        pawn_assembler.primitive(amb_mesh_id).topLevel(true).factory(FactoryDesc(LightQuadPawn::TYPE_RTTI.getName()));
        LightingPawnDto lighting_pawn_dto = LightingPawnDto(pawn_assembler.toPawnDto());
        lighting_pawn_dto.id() = amb_pawn_id;
        lighting_pawn_dto.hostLightId() = lit->id();
        auto pawn_dto = lighting_pawn_dto.toGenericDto();
        lit_pawn = std::make_shared<RequestSpatialConstitution>(amb_pawn_id, pawn_dto, SceneGraph::PersistenceLevel::Repository)->dispatch();
    }
    if (lit_pawn)
    {
        if (const auto parent = std::dynamic_pointer_cast<Node>(lit->getParent()))
        {
            parent->attachChild(lit_pawn, lit->getLocalTransform());
        }
        completeLightingPawnBuilt(lit->id(), std::dynamic_pointer_cast<LightingPawn>(lit_pawn));
    }
}

void DeferredRendererService::createSunLightQuad(const std::shared_ptr<Light>& lit)
{
    assert(lit);
    const auto sun_quad_id = Geometries::GeometryId(lit->getSpatialName() + "_lit_quad" + ".geo");
    auto quad_geo = Geometries::GeometryData::queryGeometryData(sun_quad_id);
    if (!quad_geo)
    {
        quad_geo = Geometries::SquareQuadAssembler(sun_quad_id).xyQuad(MathLib::Vector3(-1.0f, -1.0f, 0.5f), MathLib::Vector3(1.0f, 1.0f, 0.5f)).textureCoord(MathLib::Vector2(0.0f, 1.0f), MathLib::Vector2(1.0f, 0.0f)).constitute(Geometries::PersistenceLevel::Repository);
    }
    const auto sun_mesh_id = Primitives::PrimitiveId(lit->getSpatialName() + "_lit_quad", MeshPrimitive::TYPE_RTTI);
    auto lit_mesh = Primitives::Primitive::queryPrimitive(sun_mesh_id);
    if (!lit_mesh)
    {
        lit_mesh = Renderables::MeshPrimitiveAssembler(sun_mesh_id).geometryId(sun_quad_id).asNative(sun_mesh_id.name() + ".mesh@DataPath").effect(m_configuration->sunLightEffect()).textureMap(getGBufferTextureSemantics()).renderListID(Renderer::Renderer::RenderListID::DeferredLighting).constitute(Primitives::PersistenceLevel::Repository);
    }
    const auto sun_pawn_id = SpatialId(lit->getSpatialName() + "_lit_quad", LightQuadPawn::TYPE_RTTI);
    auto lit_pawn = std::make_shared<QuerySpatial>(sun_pawn_id)->dispatch();
    if (!lit_pawn)
    {
        PawnAssembler pawn_assembler(sun_pawn_id);
        pawn_assembler.primitive(sun_mesh_id)/*.spatialFlags(m_configuration->sunLightSpatialFlags())*/.topLevel(true).factory(FactoryDesc(LightQuadPawn::TYPE_RTTI.getName()));
        LightingPawnDto lighting_pawn_dto = LightingPawnDto(pawn_assembler.toPawnDto());
        lighting_pawn_dto.id() = sun_pawn_id;
        lighting_pawn_dto.hostLightId() = lit->id();
        auto pawn_dto = lighting_pawn_dto.toGenericDto();
        lit_pawn = std::make_shared<RequestSpatialConstitution>(sun_pawn_id, pawn_dto, SceneGraph::PersistenceLevel::Repository)->dispatch();
    }
    if (lit_pawn)
    {
        if (const auto parent = std::dynamic_pointer_cast<Node>(lit->getParent()))
        {
            parent->attachChild(lit_pawn, lit->getLocalTransform());
        }
        completeLightingPawnBuilt(lit->id(), std::dynamic_pointer_cast<LightingPawn>(lit_pawn));
    }
}

void DeferredRendererService::createPointLightVolume(const std::shared_ptr<Light>& lit)
{
    assert(lit);
    const auto vol_geo_id = Geometries::GeometryId("deferred_" + lit->getSpatialName() + "_lit_volume.geo");
    auto vol_geo = Geometries::GeometryData::queryGeometryData(vol_geo_id);
    if (!vol_geo)
    {
        vol_geo = Geometries::SphereAssembler(vol_geo_id).sphere(MathLib::Vector3::ZERO, lit->getLightRange(), SPHERE_SLICES, SPHERE_STACKS).boxBound().constitute(Geometries::PersistenceLevel::Repository);
    }
    const auto vol_mesh_id = Primitives::PrimitiveId(lit->getSpatialName() + "_lit_volume", MeshPrimitive::TYPE_RTTI);
    auto vol_mesh = Primitives::Primitive::queryPrimitive(vol_mesh_id);
    if (!vol_mesh)
    {
        vol_mesh = Renderables::MeshPrimitiveAssembler(vol_mesh_id).geometryId(vol_geo_id).asNative(vol_mesh_id.name() + ".mesh@DataPath").effect(m_configuration->lightVolumeEffect()).textureMap(getGBufferTextureSemantics()).renderListID(Renderer::Renderer::RenderListID::DeferredLighting).constitute(Primitives::PersistenceLevel::Repository);
    }

    auto vol_pawn_id = SpatialId(lit->getSpatialName() + "_lit_volume", LightVolumePawn::TYPE_RTTI);
    auto lit_pawn = std::make_shared<QuerySpatial>(vol_pawn_id)->dispatch();
    if (!lit_pawn)
    {
        PawnAssembler pawn_assembler(vol_pawn_id);
        pawn_assembler.factory(FactoryDesc(LightVolumePawn::TYPE_RTTI.getName())).primitive(vol_mesh_id)
            .topLevel(true).localTransform(lit->getLocalTransform());
        LightingPawnDto lighting_pawn_dto = LightingPawnDto(pawn_assembler.toPawnDto());
        lighting_pawn_dto.id() = vol_pawn_id;
        lighting_pawn_dto.hostLightId() = lit->id();
        auto pawn_dto = lighting_pawn_dto.toGenericDto();
        lit_pawn = std::make_shared<RequestSpatialConstitution>(vol_pawn_id, pawn_dto, SceneGraph::PersistenceLevel::Repository)->dispatch();
    }
    if (lit_pawn)
    {
        if (const auto parent = std::dynamic_pointer_cast<Node>(lit->getParent()))
        {
            parent->attachChild(lit_pawn, lit->getLocalTransform());
        }
        completeLightingPawnBuilt(lit->id(), std::dynamic_pointer_cast<LightingPawn>(lit_pawn));
    }
}

void DeferredRendererService::removeLightingPawn(const SpatialId& lit_id)
{
    if (const auto it = m_lightingPawns.find(lit_id); it != m_lightingPawns.end())
    {
        m_lightingPawns.erase(it);
    }
}

void DeferredRendererService::updateAmbientLightQuad(const std::shared_ptr<Light>& lit, LightInfoUpdated::NotifyCode notify)
{
    assert(lit);
    if (notify != LightInfoUpdated::NotifyCode::Color) return;
    const auto& pawn = findLightingPawn(lit->id());
    if (!pawn) return;
    pawn->notifySpatialRenderStateChanged();
}

void DeferredRendererService::updatePointLightVolume(const std::shared_ptr<Light>& lit, LightInfoUpdated::NotifyCode notify)
{
    assert(lit);
    if (lit->info().lightType() != LightInfo::LightType::Point) return;
    const auto& pawn = findLightingPawn(lit->id());
    if (!pawn) return;
    if (notify == LightInfoUpdated::NotifyCode::Position)
    {
        pawn->changeWorldPosition(lit->getLightPosition(), std::nullopt);
    }
    else if (notify == LightInfoUpdated::NotifyCode::Range)
    {
        pawn->setLocalUniformScale(lit->getLightRange());
    }
    else if (notify == LightInfoUpdated::NotifyCode::Enable)
    {
        if (lit->info().isEnable())
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

    checkLightVolumeBackfaceCulling(lit->id());
}

void DeferredRendererService::updateSunLightQuad(const std::shared_ptr<Light>& lit, LightInfoUpdated::NotifyCode notify)
{
    assert(lit);
    const auto& pawn = findLightingPawn(lit->id());
    if (!pawn) return;
    if ((notify == LightInfoUpdated::NotifyCode::Color) || (notify == LightInfoUpdated::NotifyCode::Direction))
    {
        pawn->notifySpatialRenderStateChanged();
    }
}

void DeferredRendererService::bindGBufferToPendingLights()
{
    if (m_pendingLightsOfGBufferBind.empty()) return;
    for (const auto& light : m_pendingLightsOfGBufferBind)
    {
        if (light.expired()) continue;
        if (light.lock()->info().lightType() == LightInfo::LightType::Ambient)
        {
            createAmbientLightQuad(light.lock());
        }
        else if (light.lock()->info().lightType() == LightInfo::LightType::SunLight)
        {
            createSunLightQuad(light.lock());
        }
        else if (light.lock()->info().lightType() == LightInfo::LightType::Point)
        {
            createPointLightVolume(light.lock());
        }
    }
    m_pendingLightsOfGBufferBind.clear();
}

std::shared_ptr<LightingPawn> DeferredRendererService::findLightingPawn(const SpatialId& lit_id)
{
    if (const auto it = m_lightingPawns.find(lit_id); it != m_lightingPawns.end())
    {
        return it->second.lock();
    }
    return nullptr;
}

void DeferredRendererService::checkLightVolumeBackfaceCulling(const SpatialId& lit_id)
{
    auto lit_vol = std::dynamic_pointer_cast<LightVolumePawn, LightingPawn>(findLightingPawn(lit_id));
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

EffectTextureMapDtoHelper DeferredRendererService::getGBufferTextureSemantics()
{
    assert(m_configuration);
    EffectTextureMapDtoHelper gbuffer_textures;

    gbuffer_textures.textureMapping(m_configuration->gbufferTextureId(), m_configuration->findRenderTextureUsageIndex(Graphics::RenderTextureUsage::Normal), m_configuration->gbufferNormalSemantic());
    gbuffer_textures.textureMapping(m_configuration->gbufferTextureId(), m_configuration->findRenderTextureUsageIndex(Graphics::RenderTextureUsage::Albedo), m_configuration->gbufferDiffuseSemantic());
    gbuffer_textures.textureMapping(m_configuration->gbufferTextureId(), m_configuration->findRenderTextureUsageIndex(Graphics::RenderTextureUsage::Specular), m_configuration->gbufferSpecularSemantic());
    gbuffer_textures.textureMapping(m_configuration->gbufferTextureId(), m_configuration->findRenderTextureUsageIndex(Graphics::RenderTextureUsage::Depth), m_configuration->gbufferDepthSemantic());
    return gbuffer_textures;
}
