#include "TerrainInstallingPolicy.h"
#include "Frameworks/ServiceManager.h"
#include "TerrainPrimitiveBuilder.h"
#include "Renderer/RendererErrors.h"
#include "TerrainGeometry.h"
#include "Frameworks/CommandBus.h"
#include "GameEngine/GeometryCommands.h"
#include <cassert>

using namespace Enigma::Terrain;
using namespace Enigma::Renderer;

error TerrainInstallingPolicy::Install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    Frameworks::CommandBus::Post(std::make_shared<Enigma::Engine::RegisterGeometryDtoFactory>(
           TerrainGeometry::TYPE_RTTI.GetName(), [](auto dto) { return std::make_shared<TerrainGeometry>(dto); }));
    service_manager->RegisterSystemService(std::make_shared<TerrainPrimitiveBuilder>(service_manager));
    return ErrorCode::ok;
}

error TerrainInstallingPolicy::Shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->ShutdownSystemService(TerrainPrimitiveBuilder::TYPE_RTTI);
    return ErrorCode::ok;
}