﻿#include "SceneGraphInstallingPolicy.h"
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
#include "OutRegionNode.h"
#include "Light.h"
#include <cassert>

using namespace Enigma::SceneGraph;

error SceneGraphInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    const auto timer = service_manager->getSystemServiceAs<Engine::TimerService>();
    assert(timer);
    auto scene_graph_repository = std::make_shared<SceneGraphRepository>(service_manager, m_storeMapper);
    //! ADR: constitution methods 好像可以不需要
    scene_graph_repository->registerSpatialFactory(Pawn::TYPE_RTTI.getName(), Pawn::create);
    scene_graph_repository->registerSpatialFactory(Node::TYPE_RTTI.getName(), Node::create);
    scene_graph_repository->registerSpatialFactory(LazyNode::TYPE_RTTI.getName(), LazyNode::create);
    scene_graph_repository->registerSpatialFactory(VisibilityManagedNode::TYPE_RTTI.getName(), VisibilityManagedNode::create);
    scene_graph_repository->registerSpatialFactory(PortalManagementNode::TYPE_RTTI.getName(), PortalManagementNode::create);
    scene_graph_repository->registerSpatialFactory(Portal::TYPE_RTTI.getName(), Portal::create);
    scene_graph_repository->registerSpatialFactory(PortalZoneNode::TYPE_RTTI.getName(), PortalZoneNode::create);
    scene_graph_repository->registerSpatialFactory(OutRegionNode::TYPE_RTTI.getName(), OutRegionNode::create);
    scene_graph_repository->registerSpatialLightFactory(Light::TYPE_RTTI.getName(), Light::create);
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
