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
    m_doCreatingWorldMap = std::make_shared<CommandSubscriber>([=](auto c) { DoCreatingEmptyWorldMap(c); });
    CommandBus::Subscribe(typeid(CreateEmptyWorldMap), m_doCreatingWorldMap);
    m_doDeserializingWorldMap = std::make_shared<CommandSubscriber>([=](auto c) { DoDeserializingWorldMap(c); });
    CommandBus::Subscribe(typeid(Enigma::WorldMap::DeserializeWorldMap), m_doDeserializingWorldMap);
    m_doAttachingTerrain = std::make_shared<CommandSubscriber>([=](auto c) { DoAttachingTerrain(c); });
    CommandBus::Subscribe(typeid(Enigma::WorldMap::AttachTerrainToWorldMap), m_doAttachingTerrain);

    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { OnSceneGraphBuilt(e); });
    EventPublisher::Subscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onLazyNodeInstanced = std::make_shared<EventSubscriber>([=](auto e) { OnLazyNodeInstanced(e); });
    EventPublisher::Subscribe(typeid(LazyNodeInstanced), m_onLazyNodeInstanced);

    return ServiceResult::Complete;
}

ServiceResult WorldMapService::onTerm()
{
    CommandBus::Unsubscribe(typeid(CreateEmptyWorldMap), m_doCreatingWorldMap);
    m_doCreatingWorldMap = nullptr;
    CommandBus::Unsubscribe(typeid(Enigma::WorldMap::DeserializeWorldMap), m_doDeserializingWorldMap);
    m_doDeserializingWorldMap = nullptr;
    CommandBus::Unsubscribe(typeid(Enigma::WorldMap::AttachTerrainToWorldMap), m_doAttachingTerrain);
    m_doAttachingTerrain = nullptr;

    EventPublisher::Unsubscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphBuilt = nullptr;
    EventPublisher::Unsubscribe(typeid(LazyNodeInstanced), m_onLazyNodeInstanced);
    m_onLazyNodeInstanced = nullptr;

    return ServiceResult::Complete;
}

std::vector<Enigma::Engine::GenericDtoCollection> WorldMapService::SerializeQuadNodeGraphs() const
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

Enigma::Engine::GenericDtoCollection WorldMapService::SerializeWorldMap() const
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

void WorldMapService::DeserializeWorldMap(const Engine::GenericDtoCollection& graph)
{
    CommandBus::Post(std::make_shared<BuildSceneGraph>(WORLD_MAP_TAG, graph));
}

void WorldMapService::AttachTerrainToWorldMap(const std::shared_ptr<TerrainPawn>& terrain,
    const Matrix4& local_transform)
{
    assert(terrain);
    assert(!m_sceneGraphRepository.expired());
    if (FATAL_LOG_EXPR(m_world.expired())) return;
    std::string node_name = terrain->GetSpatialName() + QUADROOT_POSTFIX; // +NODE_FILE_EXT;
    auto quadRootNode = std::dynamic_pointer_cast<VisibilityManagedNode, Node>(m_sceneGraphRepository.lock()->CreateNode(node_name, VisibilityManagedNode::TYPE_RTTI));
    quadRootNode->TheLazyStatus().ChangeStatus(LazyStatus::Status::Ready);
    quadRootNode->TheFactoryDesc().ClaimAsInstanced(node_name + ".node");
    quadRootNode->AttachChild(terrain, Matrix4::IDENTITY);
    m_world.lock()->AttachChild(quadRootNode, local_transform);
    m_listQuadRoot.push_back(quadRootNode);
}

void WorldMapService::DoCreatingEmptyWorldMap(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<CreateEmptyWorldMap, ICommand>(c);
    if (!cmd) return;
    std::vector<Engine::GenericDto> dtos = { cmd->GetDto() };
    CommandBus::Post(std::make_shared<BuildSceneGraph>(WORLD_MAP_TAG, dtos));
}

void WorldMapService::DoDeserializingWorldMap(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<Enigma::WorldMap::DeserializeWorldMap, ICommand>(c);
    if (!cmd) return;
    DeserializeWorldMap(cmd->GetGraph());
}

void WorldMapService::DoAttachingTerrain(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<Enigma::WorldMap::AttachTerrainToWorldMap, ICommand>(c);
    if (!cmd) return;
    AttachTerrainToWorldMap(cmd->GetTerrain(), cmd->GetLocalTransform());
}

void WorldMapService::OnSceneGraphBuilt(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt, IEvent>(e);
    if ((!ev) || (ev->GetSceneGraphId() != WORLD_MAP_TAG)) return;
    m_world = std::dynamic_pointer_cast<WorldMap, Spatial>(ev->GetTopLevelSpatial()[0]);
    if (!m_world.expired())
    {
        m_world.lock()->TheLazyStatus().ChangeStatus(LazyStatus::Status::Ready);  // empty world map is ready
        EventPublisher::Post(std::make_shared<WorldMapCreated>(m_world.lock()->getName(), m_world.lock()));
        CommandBus::Post(std::make_shared<AttachPortalOutsideZone>(m_world.lock()));
    }
}

void WorldMapService::OnLazyNodeInstanced(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<LazyNodeInstanced, IEvent>(e);
    if ((!ev) || (!ev->GetNode())) return;
    if (ev->GetNode()->GetParent() == m_world.lock())
    {
        m_listQuadRoot.push_back(ev->GetNode());
    }
}
