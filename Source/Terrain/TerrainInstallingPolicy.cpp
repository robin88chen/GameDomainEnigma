#include "TerrainInstallingPolicy.h"
#include "Frameworks/ServiceManager.h"
#include "Renderables/RenderableErrors.h"
#include "TerrainGeometry.h"
#include "Frameworks/CommandBus.h"
#include "Geometries/GeometryCommands.h"
#include "TerrainPawn.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "Renderables/RenderablePrimitiveBuilder.h"
#include "SceneGraph/SceneGraphRepository.h"
#include "TerrainPrimitive.h"
#include <cassert>

using namespace Enigma::Terrain;
using namespace Enigma::Renderables;

error TerrainInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    //Frameworks::CommandBus::post(std::make_shared<SceneGraph::RegisterSpatialDtoFactory>(TerrainPawn::TYPE_RTTI.getName(),
      //  [](auto o) { return new TerrainPawn(o); }));
    auto geometry_repository = service_manager->getSystemServiceAs<Enigma::Geometries::GeometryRepository>();
    assert(geometry_repository);
    geometry_repository->factory()->registerGeometryFactory(TerrainGeometry::TYPE_RTTI.getName(),
        [](auto id) { return std::make_shared<TerrainGeometry>(id); },
        [](auto id, auto dto) { return std::make_shared<TerrainGeometry>(id, dto); });
    auto renderable_builder = service_manager->getSystemServiceAs<RenderablePrimitiveBuilder>();
    assert(renderable_builder);
    renderable_builder->registerCustomMeshFactory(TerrainPrimitive::TYPE_RTTI.getName(), TerrainPrimitive::create, TerrainPrimitive::constitute);
    const auto scene_graph_repository = service_manager->getSystemServiceAs<SceneGraph::SceneGraphRepository>();
    assert(scene_graph_repository);
    scene_graph_repository->factory()->registerSpatialFactory(TerrainPawn::TYPE_RTTI.getName(), TerrainPawn::create, TerrainPawn::constitute);
    return ErrorCode::ok;
}

error TerrainInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    //Frameworks::CommandBus::post(std::make_shared<SceneGraph::UnRegisterSpatialDtoFactory>(TerrainPawn::TYPE_RTTI.getName()));
    Frameworks::CommandBus::post(std::make_shared<Enigma::Geometries::UnRegisterGeometryFactory>(TerrainGeometry::TYPE_RTTI.getName()));
    return ErrorCode::ok;
}