#include "LightMeshAssembler.h"
#include "DeferredRenderingConfiguration.h"
#include "Frameworks/QueryDispatcher.h"
#include "LightingMeshQueries.h"
#include "Geometries/StandardGeometryAssemblers.h"
#include "Primitives/PrimitiveQueries.h"
#include "Renderables/MeshPrimitiveAssembler.h"

using namespace Enigma::Rendering;
using namespace Enigma::Frameworks;
using namespace Enigma::Renderables;
using namespace Enigma::Geometries;
using namespace Enigma::Engine;

//!  samples 的數量太少，光球的 poly 太粗，會導致算出的受光亮面偏移
#define SPHERE_STACKS 40
#define SPHERE_SLICES 80

LightMeshAssembler::LightMeshAssembler(const std::shared_ptr<DeferredRenderingConfiguration>& configuration)
    : m_configuration(configuration)
{
    assert(m_configuration);
}

LightMeshAssembler::~LightMeshAssembler()
{
    m_configuration = nullptr;
}

void LightMeshAssembler::registerHandlers()
{
    m_requestAmbientLightMeshAssembly = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { requestAmbientLightMeshAssembly(q); });
    QueryDispatcher::subscribe(typeid(RequestAmbientLightMeshAssembly), m_requestAmbientLightMeshAssembly);
    m_requestSunLightMeshAssembly = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { requestSunLightMeshAssembly(q); });
    QueryDispatcher::subscribe(typeid(RequestSunLightMeshAssembly), m_requestSunLightMeshAssembly);
    m_requestPointLightMeshAssembly = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { requestPointLightMeshAssembly(q); });
    QueryDispatcher::subscribe(typeid(RequestPointLightMeshAssembly), m_requestPointLightMeshAssembly);
}

void LightMeshAssembler::unregisterHandlers()
{
    QueryDispatcher::unsubscribe(typeid(RequestAmbientLightMeshAssembly), m_requestAmbientLightMeshAssembly);
    m_requestAmbientLightMeshAssembly = nullptr;
    QueryDispatcher::unsubscribe(typeid(RequestSunLightMeshAssembly), m_requestSunLightMeshAssembly);
    m_requestSunLightMeshAssembly = nullptr;
    QueryDispatcher::unsubscribe(typeid(RequestPointLightMeshAssembly), m_requestPointLightMeshAssembly);
    m_requestPointLightMeshAssembly = nullptr;
}

std::shared_ptr<MeshPrimitive> LightMeshAssembler::assembleAmbientLightMesh(const Primitives::PrimitiveId& mesh_id)
{
    const auto amb_quad_id = GeometryId(mesh_id.name() + ".geo");
    auto quad_geo = GeometryData::queryGeometryData(amb_quad_id);
    if (!quad_geo)
    {
        quad_geo = SquareQuadAssembler(amb_quad_id).xyQuad(MathLib::Vector3(-1.0f, -1.0f, 0.5f), MathLib::Vector3(1.0f, 1.0f, 0.5f)).textureCoord(MathLib::Vector2(0.0f, 1.0f), MathLib::Vector2(1.0f, 0.0f)).constitute();
    }
    auto lit_mesh = Primitives::Primitive::queryPrimitive(mesh_id);
    if (!lit_mesh)
    {
        lit_mesh = assembleLightMesh(amb_quad_id, mesh_id, m_configuration->ambientEffect());
    }
    return std::dynamic_pointer_cast<MeshPrimitive>(lit_mesh);
}

std::shared_ptr<MeshPrimitive> LightMeshAssembler::assembleSunLightMesh(const Primitives::PrimitiveId& mesh_id)
{
    const auto sun_quad_id = GeometryId(mesh_id.name() + ".geo");
    auto quad_geo = GeometryData::queryGeometryData(sun_quad_id);
    if (!quad_geo)
    {
        quad_geo = SquareQuadAssembler(sun_quad_id).xyQuad(MathLib::Vector3(-1.0f, -1.0f, 0.5f), MathLib::Vector3(1.0f, 1.0f, 0.5f)).textureCoord(MathLib::Vector2(0.0f, 1.0f), MathLib::Vector2(1.0f, 0.0f)).constitute();
    }
    auto lit_mesh = Primitives::Primitive::queryPrimitive(mesh_id);
    if (!lit_mesh)
    {
        lit_mesh = assembleLightMesh(sun_quad_id, mesh_id, m_configuration->sunLightEffect());
    }
    return std::dynamic_pointer_cast<MeshPrimitive>(lit_mesh);
}

std::shared_ptr<MeshPrimitive> LightMeshAssembler::assemblePointLightMesh(const Primitives::PrimitiveId& mesh_id, float sphere_radius)
{
    const auto vol_geo_id = GeometryId("deferred_" + mesh_id.name() + ".geo");
    auto vol_geo = GeometryData::queryGeometryData(vol_geo_id);
    if (!vol_geo)
    {
        vol_geo = SphereAssembler(vol_geo_id).sphere(MathLib::Vector3::ZERO, sphere_radius, SPHERE_SLICES, SPHERE_STACKS).boxBound().constitute();
    }
    auto vol_mesh = Primitives::Primitive::queryPrimitive(mesh_id);
    if (!vol_mesh)
    {
        vol_mesh = assembleLightMesh(vol_geo_id, mesh_id, m_configuration->lightVolumeEffect());
    }
    return std::dynamic_pointer_cast<MeshPrimitive>(vol_mesh);
}

std::shared_ptr<Enigma::Primitives::Primitive> LightMeshAssembler::assembleLightMesh(const Geometries::GeometryId& geometry_id, const Primitives::PrimitiveId& mesh_id, const Engine::EffectMaterialId& effect_material_id)
{
    std::shared_ptr<MeshPrimitiveAssembler> assembler = std::make_shared<MeshPrimitiveAssembler>(mesh_id);
    assembler->geometryId(geometry_id);
    assembler->addEffect(effect_material_id);
    assembler->addTextureMap(std::make_shared<EffectTextureMapAssembler>(getGBufferTextureSemantics()));
    assembler->renderListID(Renderer::Renderer::RenderListID::DeferredLighting);
    assembler->asNative(mesh_id.name() + ".mesh@DataPath");
    return std::make_shared<Primitives::RequestPrimitiveConstitution>(mesh_id, assembler->assemble())->dispatch();
}

void LightMeshAssembler::requestAmbientLightMeshAssembly(const IQueryPtr& q)
{
    if (!q) return;
    const auto ambient_query = std::dynamic_pointer_cast<RequestAmbientLightMeshAssembly>(q);
    if (!ambient_query) return;
    ambient_query->setResult(assembleAmbientLightMesh(ambient_query->meshId()));
}

void LightMeshAssembler::requestSunLightMeshAssembly(const IQueryPtr& q)
{
    if (!q) return;
    const auto sun_query = std::dynamic_pointer_cast<RequestSunLightMeshAssembly>(q);
    if (!sun_query) return;
    sun_query->setResult(assembleSunLightMesh(sun_query->meshId()));
}

void LightMeshAssembler::requestPointLightMeshAssembly(const IQueryPtr& q)
{
    if (!q) return;
    const auto point_query = std::dynamic_pointer_cast<RequestPointLightMeshAssembly>(q);
    if (!point_query) return;
    point_query->setResult(assemblePointLightMesh(point_query->meshId(), point_query->sphereRadius()));
}

EffectTextureMapAssembler LightMeshAssembler::getGBufferTextureSemantics()
{
    assert(m_configuration);
    EffectTextureMapAssembler gbuffer_textures;

    gbuffer_textures.addTextureMapping(m_configuration->gbufferTextureId(), m_configuration->findRenderTextureUsageIndex(Graphics::RenderTextureUsage::Normal), m_configuration->gbufferNormalSemantic());
    gbuffer_textures.addTextureMapping(m_configuration->gbufferTextureId(), m_configuration->findRenderTextureUsageIndex(Graphics::RenderTextureUsage::Albedo), m_configuration->gbufferDiffuseSemantic());
    gbuffer_textures.addTextureMapping(m_configuration->gbufferTextureId(), m_configuration->findRenderTextureUsageIndex(Graphics::RenderTextureUsage::Specular), m_configuration->gbufferSpecularSemantic());
    gbuffer_textures.addTextureMapping(m_configuration->gbufferTextureId(), m_configuration->findRenderTextureUsageIndex(Graphics::RenderTextureUsage::Depth), m_configuration->gbufferDepthSemantic());
    return gbuffer_textures;
}
