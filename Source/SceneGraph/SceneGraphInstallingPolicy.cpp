#include "SceneGraphInstallingPolicy.h"
#include "GameEngine/TimerService.h"
#include "SceneGraphRepository.h"
#include "SceneGraphFactory.h"
#include "LazyNodeHydrationService.h"
#include "LightInfoTraversal.h"
#include "SceneGraphErrors.h"
#include "Pawn.h"
#include "Node.h"
#include "LazyNode.h"
#include "VisibilityManagedNode.h"
#include "PortalManagementNode.h"
#include "Portal.h"
#include "PortalZoneNode.h"
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
    scene_graph_repository->factory()->registerSpatialFactory(VisibilityManagedNode::TYPE_RTTI.getName(), VisibilityManagedNode::create, VisibilityManagedNode::constitute);
    scene_graph_repository->factory()->registerSpatialFactory(PortalManagementNode::TYPE_RTTI.getName(), PortalManagementNode::create, PortalManagementNode::constitute);
    scene_graph_repository->factory()->registerSpatialFactory(Portal::TYPE_RTTI.getName(), Portal::create, Portal::constitute);
    scene_graph_repository->factory()->registerSpatialFactory(PortalZoneNode::TYPE_RTTI.getName(), PortalZoneNode::create, PortalZoneNode::constitute);
    service_manager->registerSystemService(scene_graph_repository);
    service_manager->registerSystemService(std::make_shared<LazyNodeHydrationService>(service_manager, scene_graph_repository, timer));
    service_manager->registerSystemService(std::make_shared<LightInfoTraversal>(service_manager));
    return ErrorCode::ok;
}

error SceneGraphInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->shutdownSystemService(SceneGraph::LazyNodeHydrationService::TYPE_RTTI);
    service_manager->shutdownSystemService(SceneGraph::SceneGraphRepository::TYPE_RTTI);
    service_manager->shutdownSystemService(SceneGraph::LightInfoTraversal::TYPE_RTTI);
    return ErrorCode::ok;
}
