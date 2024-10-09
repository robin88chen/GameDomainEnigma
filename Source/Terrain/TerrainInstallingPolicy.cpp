#include "TerrainInstallingPolicy.h"
#include "Frameworks/ServiceManager.h"
#include "Renderables/RenderableErrors.h"
#include "TerrainGeometry.h"
#include "Frameworks/CommandBus.h"
#include "Geometries/GeometryCommands.h"
#include "TerrainPawn.h"
#include "Primitives/PrimitiveCommands.h"
#include "SceneGraph/SceneGraphRepository.h"
#include "TerrainPrimitive.h"
#include "Geometries/GeometryRepository.h"
#include <cassert>

using namespace Enigma::Terrain;
using namespace Enigma::Renderables;

error TerrainInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    //Frameworks::CommandBus::enqueue(std::make_shared<SceneGraph::RegisterSpatialDtoFactory>(TerrainPawn::TYPE_RTTI.name(),
      //  [](auto o) { return new TerrainPawn(o); }));
    //auto geometry_repository = service_manager->getSystemServiceAs<Enigma::Geometries::GeometryRepository>();
    //assert(geometry_repository);
    //geometry_repository->registerGeometryFactory(TerrainGeometry::TYPE_RTTI.getName(), createTerrainGeometry, constituteTerrainGeometry);
    std::make_shared<Geometries::RegisterGeometryFactory>(TerrainGeometry::TYPE_RTTI.getName(), TerrainGeometry::create)->execute();
    std::make_shared<Enigma::Primitives::RegisterPrimitiveFactory>(TerrainPrimitive::TYPE_RTTI.getName(), TerrainPrimitive::create)->execute();
    //auto renderable_builder = service_manager->getSystemServiceAs<RenderablePrimitiveBuilder>();
    //assert(renderable_builder);
    //renderable_builder->registerCustomMeshFactory(TerrainPrimitive::TYPE_RTTI.getName(), createTerrainPrimitive, constituteTerrainPrimitive);
    const auto scene_graph_repository = service_manager->getSystemServiceAs<SceneGraph::SceneGraphRepository>();
    assert(scene_graph_repository);
    scene_graph_repository->registerSpatialFactory(TerrainPawn::TYPE_RTTI.getName(), TerrainPawn::create);
    return ErrorCode::ok;
}

error TerrainInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    std::make_shared<Enigma::Primitives::UnregisterPrimitiveFactory>(TerrainPrimitive::TYPE_RTTI.getName())->execute();
    std::make_shared<Geometries::UnregisterGeometryFactory>(TerrainGeometry::TYPE_RTTI.getName())->execute();
    //Frameworks::CommandBus::enqueue(std::make_shared<SceneGraph::UnRegisterSpatialDtoFactory>(TerrainPawn::TYPE_RTTI.name()));
    //Frameworks::CommandBus::enqueue(std::make_shared<Enigma::Geometries::UnRegisterGeometryFactory>(TerrainGeometry::TYPE_RTTI.getName()));
    return ErrorCode::ok;
}