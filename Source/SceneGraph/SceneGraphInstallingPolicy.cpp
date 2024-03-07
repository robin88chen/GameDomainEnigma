#include "SceneGraphInstallingPolicy.h"
#include "GameEngine/TimerService.h"
#include "SceneGraphRepository.h"
#include "SceneGraphFactory.h"
#include "LazyNodeIOService.h"
#include "LightInfoTraversal.h"
#include "SceneGraphErrors.h"
#include "Pawn.h"
#include "Node.h"
#include "LazyNode.h"
#include <cassert>

using namespace Enigma::SceneGraph;

error SceneGraphInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    const auto timer = service_manager->getSystemServiceAs<Engine::TimerService>();
    assert(timer);
    auto scene_graph_repository = std::make_shared<SceneGraphRepository>(service_manager, m_storeMapper);
    scene_graph_repository->factory()->registerSpatialFactory(Pawn::TYPE_RTTI.getName(), Pawn::create, Pawn::constitute);
    scene_graph_repository->factory()->registerSpatialFactory(Node::TYPE_RTTI.getName(), Node::create, Node::constitute);
    scene_graph_repository->factory()->registerSpatialFactory(LazyNode::TYPE_RTTI.getName(), LazyNode::create, LazyNode::constitute);
    service_manager->registerSystemService(scene_graph_repository);
    service_manager->registerSystemService(std::make_shared<LazyNodeIOService>(service_manager, timer, m_dtoDeserializer));
    service_manager->registerSystemService(std::make_shared<LightInfoTraversal>(service_manager));
    return ErrorCode::ok;
}

error SceneGraphInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->shutdownSystemService(SceneGraph::LazyNodeIOService::TYPE_RTTI);
    service_manager->shutdownSystemService(SceneGraph::SceneGraphRepository::TYPE_RTTI);
    service_manager->shutdownSystemService(SceneGraph::LightInfoTraversal::TYPE_RTTI);
    return ErrorCode::ok;
}
