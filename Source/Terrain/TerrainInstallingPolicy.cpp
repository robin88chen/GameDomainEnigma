#include "TerrainInstallingPolicy.h"
#include "Frameworks/ServiceManager.h"
#include "TerrainGeometry.h"
#include "Geometries/GeometryCommands.h"
#include "TerrainPawn.h"
#include "Primitives/PrimitiveCommands.h"
#include "SceneGraph/SceneGraphRepository.h"
#include "TerrainPrimitive.h"
#include <cassert>
#include <system_error>

using namespace Enigma::Terrain;

error TerrainInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    std::make_shared<Geometries::RegisterGeometryFactory>(TerrainGeometry::TYPE_RTTI.getName(), TerrainGeometry::create)->execute();
    std::make_shared<Enigma::Primitives::RegisterPrimitiveFactory>(TerrainPrimitive::TYPE_RTTI.getName(), TerrainPrimitive::create)->execute();
    const auto scene_graph_repository = service_manager->getSystemServiceAs<SceneGraph::SceneGraphRepository>();
    assert(scene_graph_repository);
    scene_graph_repository->registerSpatialFactory(TerrainPawn::TYPE_RTTI.getName(), TerrainPawn::create);
    return std::error_code();
}

error TerrainInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    std::make_shared<Enigma::Primitives::UnregisterPrimitiveFactory>(TerrainPrimitive::TYPE_RTTI.getName())->execute();
    std::make_shared<Geometries::UnregisterGeometryFactory>(TerrainGeometry::TYPE_RTTI.getName())->execute();
    return std::error_code();
}