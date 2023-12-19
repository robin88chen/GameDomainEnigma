#include "PrefabIOService.h"
#include "PrefabCommands.h"
#include "PrefabErrors.h"
#include "PrefabEvents.h"
#include "PawnPrefabDto.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "GameEngine/DtoEvents.h"
#include "GameCommon/GameSceneCommands.h"
#include <cassert>

using namespace Enigma::Prefabs;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;
using namespace Enigma::SceneGraph;

DEFINE_RTTI(Prefabs, PrefabIOService, ISystemService);

PrefabIOService::PrefabIOService(Frameworks::ServiceManager* srv_mngr, const std::shared_ptr<IDtoDeserializer>& dto_deserializer) : ISystemService(srv_mngr)
{
    m_needTick = false;
    m_dtoDeserializer = dto_deserializer;
}

PrefabIOService::~PrefabIOService()
{
    m_currentCommand = nullptr;
    m_loadedPawn = nullptr;
    m_loadingCommands.clear();
}

Enigma::Frameworks::ServiceResult PrefabIOService::onInit()
{
    m_onDtoDeserialized = std::make_shared<EventSubscriber>([=](auto e) { onDtoDeserialized(e); });
    EventPublisher::subscribe(typeid(GenericDtoDeserialized), m_onDtoDeserialized);
    m_onDeserializeDtoFailed = std::make_shared<EventSubscriber>([=](auto e) { onDeserializeDtoFailed(e); });
    EventPublisher::subscribe(typeid(DeserializeDtoFailed), m_onDeserializeDtoFailed);
    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { onSceneGraphBuilt(e); });
    EventPublisher::subscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onBuildSceneGraphFailed = std::make_shared<EventSubscriber>([=](auto e) { onBuildSceneGraphFailed(e); });
    EventPublisher::subscribe(typeid(BuildFactorySceneGraphFailed), m_onBuildSceneGraphFailed);
    m_onPawnPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { onPawnPrimitiveBuilt(e); });
    EventPublisher::subscribe(typeid(PawnPrimitiveBuilt), m_onPawnPrimitiveBuilt);
    m_onBuildPawnPrimitiveFailed = std::make_shared<EventSubscriber>([=](auto e) { onBuildPawnPrimitiveFailed(e); });
    EventPublisher::subscribe(typeid(BuildPawnPrimitiveFailed), m_onBuildPawnPrimitiveFailed);

    m_loadPawnPrefab = std::make_shared<CommandSubscriber>([=](auto c) { loadPawnPrefab(c); });
    CommandBus::subscribe(typeid(LoadPawnPrefab), m_loadPawnPrefab);

    return ServiceResult::Complete;
}

ServiceResult PrefabIOService::onTick()
{
    if (m_currentCommand) return ServiceResult::Pendding;
    loadNextPrefab();
    return ServiceResult::Pendding;
}

Enigma::Frameworks::ServiceResult PrefabIOService::onTerm()
{
    m_currentCommand = nullptr;
    m_loadedPawn = nullptr;
    m_loadingCommands.clear();

    EventPublisher::unsubscribe(typeid(GenericDtoDeserialized), m_onDtoDeserialized);
    m_onDtoDeserialized = nullptr;
    EventPublisher::unsubscribe(typeid(DeserializeDtoFailed), m_onDeserializeDtoFailed);
    m_onDeserializeDtoFailed = nullptr;
    EventPublisher::unsubscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphBuilt = nullptr;
    EventPublisher::unsubscribe(typeid(BuildFactorySceneGraphFailed), m_onBuildSceneGraphFailed);
    m_onBuildSceneGraphFailed = nullptr;
    EventPublisher::unsubscribe(typeid(PawnPrimitiveBuilt), m_onPawnPrimitiveBuilt);
    m_onPawnPrimitiveBuilt = nullptr;
    EventPublisher::unsubscribe(typeid(BuildPawnPrimitiveFailed), m_onBuildPawnPrimitiveFailed);
    m_onBuildPawnPrimitiveFailed = nullptr;

    CommandBus::unsubscribe(typeid(LoadPawnPrefab), m_loadPawnPrefab);
    m_loadPawnPrefab = nullptr;

    return ServiceResult::Complete;
}

void PrefabIOService::loadNextPrefab()
{
    if (m_currentCommand) return;
    if (m_loadingCommands.empty())
    {
        m_needTick = false;
        return;
    }
    m_currentCommand = m_loadingCommands.front();
    m_loadingCommands.pop_front();
    deserializePrefab(m_currentCommand->getPawnDto().ruid(), m_currentCommand->getPawnDto().GetRtti().GetPrefab());
}

void PrefabIOService::deserializePrefab(const Ruid& dto_ruid, const std::string& prefab_at_path)
{
    if (prefab_at_path.empty()) return failPrefabLoading(ErrorCode::emptyPrefabPath);
    if (!m_dtoDeserializer) return failPrefabLoading(ErrorCode::nullDeserializer);

    m_dtoDeserializer->invokeDeserialize(dto_ruid, prefab_at_path);
}

void PrefabIOService::completePawnPrefabLoading(const std::shared_ptr<SceneGraph::Pawn>& pawn)
{
    assert(pawn);
    assert(m_currentCommand);
    EventPublisher::post(std::make_shared<PawnPrefabLoaded>(m_currentCommand->getRuid(), m_currentCommand->getPawnDto().GetRtti().GetPrefab(), pawn));
    m_currentCommand = nullptr;
    m_loadedPawn = nullptr;
    loadNextPrefab();
}

void PrefabIOService::failPrefabLoading(error er)
{
    EventPublisher::post(std::make_shared<LoadPawnPrefabFailed>(m_currentCommand->getRuid(), er));
    m_currentCommand = nullptr;
    m_loadedPawn = nullptr;
    loadNextPrefab();
}

void PrefabIOService::onDtoDeserialized(const Frameworks::IEventPtr& e)
{
    if (!m_currentCommand) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<GenericDtoDeserialized>(e);
    if (!ev) return;
    if (ev->getRuid() != m_currentCommand->getPawnDto().ruid()) return;
    CommandBus::post(std::make_shared<BuildSceneGraph>(m_currentCommand->getPawnDto().getName(), ev->GetDtos()));
}

void PrefabIOService::onDeserializeDtoFailed(const Frameworks::IEventPtr& e)
{
    if (!m_currentCommand) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<DeserializeDtoFailed>(e);
    if (!ev) return;
    if (ev->getRuid() != m_currentCommand->getPawnDto().ruid()) return;
    failPrefabLoading(ev->GetErrorCode());
}

void PrefabIOService::onSceneGraphBuilt(const Frameworks::IEventPtr& e)
{
    if (!m_currentCommand) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt>(e);
    if (!ev) return;
    if (ev->GetSceneGraphId() != m_currentCommand->getPawnDto().getName()) return;
    if ((ev->GetTopLevelSpatial().empty()) || (!ev->GetTopLevelSpatial()[0])) return failPrefabLoading(ErrorCode::emptyPrefabs);
    auto pawn = std::dynamic_pointer_cast<Pawn>(ev->GetTopLevelSpatial()[0]);
    if (!pawn) return failPrefabLoading(ErrorCode::invalidPrefab);
    if (pawn->GetPrimitive())
    {
        completePawnPrefabLoading(pawn);
    }
    else
    {
        m_loadedPawn = pawn;
    }
}

void PrefabIOService::onPawnPrimitiveBuilt(const Frameworks::IEventPtr& e)
{
    if (!m_currentCommand) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PawnPrimitiveBuilt>(e);
    if (!ev) return;
    if (!m_loadedPawn) return; // no pending pawn
    if (ev->GetPawn() == m_loadedPawn)
    {
        completePawnPrefabLoading(m_loadedPawn);
    }
}

void PrefabIOService::onBuildPawnPrimitiveFailed(const Frameworks::IEventPtr& e)
{
    if (!m_currentCommand) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<BuildPawnPrimitiveFailed>(e);
    if (!ev) return;
    if (!m_loadedPawn) return; // no pending pawn
    if (ev->GetPawn() == m_loadedPawn)
    {
        failPrefabLoading(ev->GetErrorCode());
    }
}

void PrefabIOService::onBuildSceneGraphFailed(const Frameworks::IEventPtr& e)
{
    if (!m_currentCommand) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<BuildFactorySceneGraphFailed>(e);
    if (!ev) return;
    if (ev->GetSceneGraphId() != m_currentCommand->getPawnDto().getName()) return;
    failPrefabLoading(ev->GetErrorCode());
}

void PrefabIOService::loadPawnPrefab(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<LoadPawnPrefab>(c);
    if (!cmd) return;
    m_loadingCommands.emplace_back(cmd);
    m_needTick = true;
    loadNextPrefab();
}
