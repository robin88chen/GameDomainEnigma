#include "WorldMapService.h"
#include "WorldMap.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "WorldMapCommands.h"
#include "WorldMapEvents.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "Frameworks/LazyStatus.h"
#include "Platforms/PlatformLayer.h"
#include "SceneGraph/PortalCommands.h"
#include "SceneGraph/VisibilityManagedNode.h"
#include "SceneGraph/EnumDerivedSpatials.h"
#include "SceneGraph/EnumNonDerivedSpatials.h"
#include "SceneGraph/SceneFlattenTraversal.h"
#include "SceneGraph/Spatial.h"

using namespace Enigma::WorldMap;
using namespace Enigma::Frameworks;
using namespace Enigma::SceneGraph;
using namespace Enigma::Terrain;
using namespace Enigma::MathLib;

DEFINE_RTTI(WorldMap, WorldMapService, ISystemService);

std::string WORLD_MAP_TAG = "world_map";
std::string QUADROOT_POSTFIX = "_qroot";

WorldMapService::WorldMapService(ServiceManager* mngr, const std::shared_ptr<SceneGraphRepository>& scene_graph_repository) : ISystemService(mngr)
{
    m_sceneGraphRepository = scene_graph_repository;
    m_needTick = false;
}

WorldMapService::~WorldMapService()
{

}

ServiceResult WorldMapService::onInit()
{
    m_doCreatingWorldMap = std::make_shared<CommandSubscriber>([=](auto c) { doCreatingEmptyWorldMap(c); });
    CommandBus::subscribe(typeid(CreateEmptyWorldMap), m_doCreatingWorldMap);
    m_doDeserializingWorldMap = std::make_shared<CommandSubscriber>([=](auto c) { doDeserializingWorldMap(c); });
    CommandBus::subscribe(typeid(Enigma::WorldMap::DeserializeWorldMap), m_doDeserializingWorldMap);
    m_doAttachingTerrain = std::make_shared<CommandSubscriber>([=](auto c) { doAttachingTerrain(c); });
    CommandBus::subscribe(typeid(Enigma::WorldMap::AttachTerrainToWorldMap), m_doAttachingTerrain);

    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { onSceneGraphBuilt(e); });
    EventPublisher::subscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onLazyNodeInstanced = std::make_shared<EventSubscriber>([=](auto e) { onLazyNodeInstanced(e); });
    EventPublisher::subscribe(typeid(LazyNodeInstanced), m_onLazyNodeInstanced);

    return ServiceResult::Complete;
}

ServiceResult WorldMapService::onTerm()
{
    CommandBus::unsubscribe(typeid(CreateEmptyWorldMap), m_doCreatingWorldMap);
    m_doCreatingWorldMap = nullptr;
    CommandBus::unsubscribe(typeid(Enigma::WorldMap::DeserializeWorldMap), m_doDeserializingWorldMap);
    m_doDeserializingWorldMap = nullptr;
    CommandBus::unsubscribe(typeid(Enigma::WorldMap::AttachTerrainToWorldMap), m_doAttachingTerrain);
    m_doAttachingTerrain = nullptr;

    EventPublisher::unsubscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphBuilt = nullptr;
    EventPublisher::unsubscribe(typeid(LazyNodeInstanced), m_onLazyNodeInstanced);
    m_onLazyNodeInstanced = nullptr;

    return ServiceResult::Complete;
}

std::vector<Enigma::Engine::GenericDtoCollection> WorldMapService::serializeQuadNodeGraphs() const
{
    assert(!m_world.expired());

    std::vector<Engine::GenericDtoCollection> collections;
    EnumDerivedSpatials enumNode(VisibilityManagedNode::TYPE_RTTI);
    m_world.lock()->VisitBy(&enumNode);
    if (enumNode.GetSpatials().empty()) return collections;

    for (auto& node : enumNode.GetSpatials())
    {
        //todo : if quad node has portal node child, then need serialize portal node
        SceneGraph::SceneFlattenTraversal flatten;
        node->VisitBy(&flatten);
        if (flatten.GetSpatials().empty()) continue;
        Engine::GenericDtoCollection collection;
        for (auto& sp : flatten.GetSpatials())
        {
            collection.push_back(sp->SerializeDto());
        }
        collection[0].AsTopLevel(true);
        collections.push_back(collection);
    }
    return collections;
}

Enigma::Engine::GenericDtoCollection WorldMapService::serializeWorldMap() const
{
    assert(!m_world.expired());
    Engine::GenericDtoCollection collection;
    auto world_dto = m_world.lock()->SerializeDto();
    world_dto.AsTopLevel(true);
    collection.push_back(world_dto);
    for (auto node : m_listQuadRoot)
    {
        if (node.expired()) continue;
        collection.push_back(node.lock()->SerializeAsLaziness());
    }
    for (const auto& child : m_world.lock()->GetChildList())
    {
        if (!child) continue;
        EnumNonDerivedSpatials enumSpatials(LazyNode::TYPE_RTTI);
        child->VisitBy(&enumSpatials);
        for (const auto& spatial : enumSpatials.GetSpatials())
        {
            if (!spatial) continue;
            collection.push_back(spatial->SerializeDto());
        }
    }
    return collection;
}

void WorldMapService::deserializeWorldMap(const Engine::GenericDtoCollection& graph)
{
    CommandBus::post(std::make_shared<BuildSceneGraph>(WORLD_MAP_TAG, graph));
}

void WorldMapService::attachTerrainToWorldMap(const std::shared_ptr<TerrainPawn>& terrain,
    const Matrix4& local_transform)
{
    assert(terrain);
    assert(!m_sceneGraphRepository.expired());
    if (FATAL_LOG_EXPR(m_world.expired())) return;
    std::string node_name = terrain->GetSpatialName() + QUADROOT_POSTFIX; // +NODE_FILE_EXT;
    auto quadRootNode = std::dynamic_pointer_cast<VisibilityManagedNode, Node>(m_sceneGraphRepository.lock()->CreateNode(node_name, VisibilityManagedNode::TYPE_RTTI));
    quadRootNode->TheLazyStatus().changeStatus(LazyStatus::Status::Ready);
    quadRootNode->TheFactoryDesc().ClaimAsInstanced(node_name + ".node");
    quadRootNode->AttachChild(terrain, Matrix4::IDENTITY);
    m_world.lock()->AttachChild(quadRootNode, local_transform);
    m_listQuadRoot.push_back(quadRootNode);
}

void WorldMapService::doCreatingEmptyWorldMap(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<CreateEmptyWorldMap, ICommand>(c);
    if (!cmd) return;
    std::vector<Engine::GenericDto> dtos = { cmd->getDto() };
    CommandBus::post(std::make_shared<BuildSceneGraph>(WORLD_MAP_TAG, dtos));
}

void WorldMapService::doDeserializingWorldMap(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<Enigma::WorldMap::DeserializeWorldMap, ICommand>(c);
    if (!cmd) return;
    DeserializeWorldMap(cmd->getGraph());
}

void WorldMapService::doAttachingTerrain(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<Enigma::WorldMap::AttachTerrainToWorldMap, ICommand>(c);
    if (!cmd) return;
    AttachTerrainToWorldMap(cmd->getTerrain(), cmd->getLocalTransform());
}

void WorldMapService::onSceneGraphBuilt(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt, IEvent>(e);
    if ((!ev) || (ev->GetSceneGraphId() != WORLD_MAP_TAG)) return;
    m_world = std::dynamic_pointer_cast<WorldMap, Spatial>(ev->GetTopLevelSpatial()[0]);
    if (!m_world.expired())
    {
        m_world.lock()->TheLazyStatus().changeStatus(LazyStatus::Status::Ready);  // empty world map is ready
        EventPublisher::post(std::make_shared<WorldMapCreated>(m_world.lock()->getName(), m_world.lock()));
        CommandBus::post(std::make_shared<AttachPortalOutsideZone>(m_world.lock()));
    }
}

void WorldMapService::onLazyNodeInstanced(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<LazyNodeInstanced, IEvent>(e);
    if ((!ev) || (!ev->GetNode())) return;
    if (ev->GetNode()->GetParent() == m_world.lock())
    {
        m_listQuadRoot.push_back(ev->GetNode());
    }
}
