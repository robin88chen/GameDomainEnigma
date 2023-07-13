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

ServiceResult WorldMapService::OnInit()
{
    m_doCreatingWorldMap = std::make_shared<CommandSubscriber>([=](auto c) { DoCreatingEmptyWorldMap(c); });
    CommandBus::Subscribe(typeid(CreateEmptyWorldMap), m_doCreatingWorldMap);
    m_doAttachingTerrain = std::make_shared<CommandSubscriber>([=](auto c) { DoAttachingTerrain(c); });
    CommandBus::Subscribe(typeid(Enigma::WorldMap::AttachTerrainToWorldMap), m_doAttachingTerrain);

    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { OnSceneGraphBuilt(e); });
    EventPublisher::Subscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    return ServiceResult::Complete;
}

ServiceResult WorldMapService::OnTerm()
{
    CommandBus::Unsubscribe(typeid(CreateEmptyWorldMap), m_doCreatingWorldMap);
    m_doCreatingWorldMap = nullptr;
    CommandBus::Unsubscribe(typeid(Enigma::WorldMap::AttachTerrainToWorldMap), m_doAttachingTerrain);
    m_doAttachingTerrain = nullptr;

    EventPublisher::Unsubscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphBuilt = nullptr;
    return ServiceResult::Complete;
}

std::vector<Enigma::Engine::GenericDto> WorldMapService::SerializeTerrains() const
{
    assert(!m_world.expired());

    std::vector<Engine::GenericDto> dtos;
    EnumDerivedSpatials enumTerrain(TerrainPawn::TYPE_RTTI);
    m_world.lock()->VisitBy(&enumTerrain);
    if (enumTerrain.GetSpatials().empty()) return dtos;

    for (auto& terrain : enumTerrain.GetSpatials())
    {
        dtos.push_back(terrain->SerializeDto());
    }
    return dtos;
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
    quadRootNode->TheFactoryDesc().ClaimAsDeferred();
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
        EventPublisher::Post(std::make_shared<WorldMapCreated>(m_world.lock()->GetName(), m_world.lock()));
        CommandBus::Post(std::make_shared<AttachPortalOutsideZone>(m_world.lock()));
    }
}
