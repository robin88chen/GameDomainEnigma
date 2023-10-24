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
    m_loadingCommands.clear();
}

Enigma::Frameworks::ServiceResult PrefabIOService::onInit()
{
    m_onDtoDeserialized = std::make_shared<EventSubscriber>([=](auto e) { OnDtoDeserialized(e); });
    EventPublisher::Subscribe(typeid(GenericDtoDeserialized), m_onDtoDeserialized);
    m_onDeserializeDtoFailed = std::make_shared<EventSubscriber>([=](auto e) { OnDeserializeDtoFailed(e); });
    EventPublisher::Subscribe(typeid(DeserializeDtoFailed), m_onDeserializeDtoFailed);

    m_doLoadingPawnPrefab = std::make_shared<CommandSubscriber>([=](auto c) { DoLoadingPawnPrefab(c); });
    CommandBus::Subscribe(typeid(LoadPawnPrefab), m_doLoadingPawnPrefab);

    return ServiceResult::Complete;
}

ServiceResult PrefabIOService::onTick()
{
    if (m_currentCommand) return ServiceResult::Pendding;
    LoadNextPrefab();
    return ServiceResult::Pendding;
}

Enigma::Frameworks::ServiceResult PrefabIOService::onTerm()
{
    m_currentCommand = nullptr;
    m_loadingCommands.clear();

    EventPublisher::Unsubscribe(typeid(GenericDtoDeserialized), m_onDtoDeserialized);
    m_onDtoDeserialized = nullptr;
    EventPublisher::Unsubscribe(typeid(DeserializeDtoFailed), m_onDeserializeDtoFailed);
    m_onDeserializeDtoFailed = nullptr;

    CommandBus::Unsubscribe(typeid(LoadPawnPrefab), m_doLoadingPawnPrefab);
    m_doLoadingPawnPrefab = nullptr;

    return ServiceResult::Complete;
}

void PrefabIOService::LoadNextPrefab()
{
    if (m_currentCommand) return;
    if (m_loadingCommands.empty())
    {
        m_needTick = false;
        return;
    }
    m_currentCommand = m_loadingCommands.front();
    m_loadingCommands.pop_front();
    DeserializePrefab(m_currentCommand->GetPawnDto().GetId(), m_currentCommand->GetPawnDto().GetRtti().GetPrefab());
}

void PrefabIOService::DeserializePrefab(const Ruid& dto_ruid, const std::string& prefab_at_path)
{
    if (prefab_at_path.empty()) return FailPrefabLoading(ErrorCode::emptyPrefabPath);
    if (!m_dtoDeserializer) return FailPrefabLoading(ErrorCode::nullDeserializer);

    m_dtoDeserializer->InvokeDeserialize(dto_ruid, prefab_at_path);
}

void PrefabIOService::CompletePawnPrefabLoading(const std::shared_ptr<SceneGraph::Pawn>& pawn)
{
    assert(pawn);
    assert(m_currentCommand);
    PawnPrefabDto dto = PawnPrefabDto::FromGenericDto(m_currentCommand->GetPawnDto());
    CommandBus::Post(std::make_shared<GameCommon::AttachNodeChild>(dto.ParentName(), pawn, dto.LocalTransform()));
    m_currentCommand = nullptr;
    LoadNextPrefab();
}

void PrefabIOService::FailPrefabLoading(error er)
{
    EventPublisher::Post(std::make_shared<LoadPrefabFailed>(m_currentCommand->GetRuid(), er));
    m_currentCommand = nullptr;
    LoadNextPrefab();
}

void PrefabIOService::OnDtoDeserialized(const Frameworks::IEventPtr& e)
{
    if (!m_currentCommand) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<GenericDtoDeserialized>(e);
    if (!ev) return;
    if (ev->GetRuid() != m_currentCommand->GetPawnDto().GetId()) return;
    CommandBus::Post(std::make_shared<BuildSceneGraph>(m_currentCommand->GetPawnDto().GetName(), ev->GetDtos()));
}

void PrefabIOService::OnDeserializeDtoFailed(const Frameworks::IEventPtr& e)
{
    if (!m_currentCommand) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<DeserializeDtoFailed>(e);
    if (!ev) return;
    if (ev->GetRuid() != m_currentCommand->GetPawnDto().GetId()) return;
    FailPrefabLoading(ev->GetErrorCode());
}

void PrefabIOService::OnSceneGraphBuilt(const Frameworks::IEventPtr& e)
{
    if (!m_currentCommand) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt>(e);
    if (!ev) return;
    if (ev->GetSceneGraphId() != m_currentCommand->GetPawnDto().GetName()) return;
    if ((ev->GetTopLevelSpatial().empty()) || (!ev->GetTopLevelSpatial()[0])) return FailPrefabLoading(ErrorCode::emptyPrefabs);
    auto pawn = std::dynamic_pointer_cast<Pawn>(ev->GetTopLevelSpatial()[0]);
    if (!pawn) return FailPrefabLoading(ErrorCode::invalidPrefab);
    CompletePawnPrefabLoading(pawn);
}

void PrefabIOService::OnBuildSceneGraphFailed(const Frameworks::IEventPtr& e)
{
    if (!m_currentCommand) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<BuildFactorySceneGraphFailed>(e);
    if (!ev) return;
    if (ev->GetSceneGraphId() != m_currentCommand->GetPawnDto().GetName()) return;
    FailPrefabLoading(ev->GetErrorCode());
}

void PrefabIOService::DoLoadingPawnPrefab(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<LoadPawnPrefab>(c);
    if (!cmd) return;
    m_loadingCommands.emplace_back(cmd);
    m_needTick = true;
    LoadNextPrefab();
}
