#include "WorldMapService.h"
#include "WorldMap.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "WorldMapCommands.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphEvents.h"

using namespace Enigma::WorldMap;
using namespace Enigma::Frameworks;

DEFINE_RTTI(WorldMap, WorldMapService, ISystemService);

std::string WORLD_MAP_TAG = "world_map";

WorldMapService::WorldMapService(Frameworks::ServiceManager* mngr) : ISystemService(mngr)
{
    m_needTick = false;
}

WorldMapService::~WorldMapService()
{

}

ServiceResult WorldMapService::OnInit()
{
    m_doCreatingWorldMap = std::make_shared<CommandSubscriber>([=](auto c) { DoCreatingEmptyWorldMap(c); });
    CommandBus::Subscribe(typeid(CreateEmptyWorldMap), m_doCreatingWorldMap);

    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { OnSceneGraphBuilt(e); });
    EventPublisher::Subscribe(typeid(SceneGraph::FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    return ServiceResult::Complete;
}

ServiceResult WorldMapService::OnTerm()
{
    CommandBus::Unsubscribe(typeid(CreateEmptyWorldMap), m_doCreatingWorldMap);
    m_doCreatingWorldMap = nullptr;

    EventPublisher::Unsubscribe(typeid(SceneGraph::FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphBuilt = nullptr;
    return ServiceResult::Complete;
}

void WorldMapService::DoCreatingEmptyWorldMap(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<CreateEmptyWorldMap, ICommand>(c);
    if (!cmd) return;
    std::vector<Engine::GenericDto> dtos = { cmd->GetDto() };
    CommandBus::Post(std::make_shared<SceneGraph::BuildSceneGraph>(WORLD_MAP_TAG, dtos));
}

void WorldMapService::OnSceneGraphBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneGraph::FactorySceneGraphBuilt, IEvent>(e);
    if ((!ev) || (ev->GetSceneGraphId() != WORLD_MAP_TAG)) return;
    m_world = std::dynamic_pointer_cast<WorldMap, SceneGraph::Spatial>(ev->GetTopLevelSpatial()[0]);
}
