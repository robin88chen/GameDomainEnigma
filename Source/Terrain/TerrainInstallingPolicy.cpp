#include "TerrainInstallingPolicy.h"
#include "Frameworks/ServiceManager.h"
#include "Renderer/RendererErrors.h"
#include "TerrainGeometry.h"
#include "Frameworks/CommandBus.h"
#include "GameEngine/GeometryCommands.h"
#include "TerrainPawn.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "TerrainPrimitiveDto.h"
#include "GameEngine/FactoryCommands.h"
#include "TerrainPrimitive.h"
#include <cassert>

using namespace Enigma::Terrain;
using namespace Enigma::Renderer;

error TerrainInstallingPolicy::Install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    Frameworks::CommandBus::Post(std::make_shared<SceneGraph::RegisterSpatialDtoFactory>(TerrainPawn::TYPE_RTTI.getName(),
        [](auto o) { return new TerrainPawn(o); }));
    Frameworks::CommandBus::Post(std::make_shared<Enigma::Engine::RegisterGeometryDtoFactory>(
           TerrainGeometry::TYPE_RTTI.getName(), [](auto dto) { return std::make_shared<TerrainGeometry>(dto); }));
    Frameworks::CommandBus::Post(std::make_shared<Enigma::Engine::RegisterDtoPolicyConverter>(
        TerrainPrimitive::TYPE_RTTI.getName(), TerrainPrimitiveDto::TerrainMeshConvertToPolicy));
    return ErrorCode::ok;
}

error TerrainInstallingPolicy::Shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    Frameworks::CommandBus::Post(std::make_shared<Enigma::Engine::UnRegisterDtoPolicyConverter>(TerrainPrimitive::TYPE_RTTI.getName()));
    Frameworks::CommandBus::Post(std::make_shared<SceneGraph::UnRegisterSpatialDtoFactory>(TerrainPawn::TYPE_RTTI.getName()));
    Frameworks::CommandBus::Post(std::make_shared<Enigma::Engine::UnRegisterGeometryDtoFactory>(TerrainGeometry::TYPE_RTTI.getName()));
    return ErrorCode::ok;
}