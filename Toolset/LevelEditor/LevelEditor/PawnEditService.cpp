#include "PawnEditService.h"
#include "Frameworks/StringFormat.h"
#include "LevelEditorCommands.h"
#include "LevelEditorErrors.h"
#include "PawnLoader.h"
#include "LevelEditorEvents.h"
#include "Frameworks/EventPublisher.h"
#include "GameCommon/GamesceneCommands.h"
#include "Frameworks/CommandBus.h"
#include "MathLib/Matrix4.h"
#include "Platforms/MemoryMacro.h"
#include "Prefabs/PawnPrefabDto.h"
#include "Prefabs/PrefabCommands.h"
#include "Prefabs/PrefabEvents.h"
#include "GameEngine/FactoryDesc.h"
#include "GameCommon/AnimatedPawn.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;
using namespace Enigma::SceneGraph;
using namespace Enigma::GameCommon;
using namespace Enigma::MathLib;
using namespace Enigma::Prefabs;
using namespace Enigma::Engine;

Rtti PawnEditService::TYPE_RTTI{ "LevelEditor.PawnEditService", &ISystemService::TYPE_RTTI };

PawnEditService::PawnEditService(ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    m_needTick = false;
    m_pawnLoader = nullptr;
}

PawnEditService::~PawnEditService()
{
    SAFE_DELETE(m_pawnLoader);
}

ServiceResult PawnEditService::OnInit()
{
    m_onPrefabLoaded = std::make_shared<EventSubscriber>([=](auto e) { onPrefabLoaded(e); });
    EventPublisher::Subscribe(typeid(PrefabLoaded), m_onPrefabLoaded);
    m_onLoadPrefabFailed = std::make_shared<EventSubscriber>([=](auto e) { onLoadPrefabFailed(e); });
    EventPublisher::Subscribe(typeid(LoadPawnFailed), m_onLoadPrefabFailed);

    m_pawnLoader = new PawnLoader();

    return ServiceResult::Complete;
}

ServiceResult PawnEditService::OnTick()
{
    if (m_currentLoadingPawn) return ServiceResult::Pendding;
    loadNextPawn();
    return ServiceResult::Pendding;
}

ServiceResult PawnEditService::OnTerm()
{
    EventPublisher::Unsubscribe(typeid(PawnLoaded), m_onPrefabLoaded);
    m_onPrefabLoaded = nullptr;
    EventPublisher::Unsubscribe(typeid(LoadPawnFailed), m_onLoadPrefabFailed);
    m_onLoadPrefabFailed = nullptr;

    SAFE_DELETE(m_pawnLoader);

    return ServiceResult::Complete;
}

error PawnEditService::putCandidatePawn(const std::string& name, const std::string& full_path, const std::string& parent_name, const Vector3& position)
{
    LoadingPawnMeta meta{ name, full_path, parent_name, position };
    m_loadingPawns.push_back(meta);
    m_needTick = true;
    return ErrorCode::ok;
}

void PawnEditService::loadNextPawn()
{
    if (m_currentLoadingPawn) return;
    if (m_loadingPawns.empty())
    {
        m_needTick = false;
        return;
    }
    m_currentLoadingPawn = m_loadingPawns.front();
    m_loadingPawns.pop_front();
    PawnPrefabDto dto;
    dto.Name() = m_currentLoadingPawn->m_name;
    dto.IsTopLevel() = true;
    dto.TheFactoryDesc() = FactoryDesc(AnimatedPawn::TYPE_RTTI.GetName()).ClaimByPrefab(m_currentLoadingPawn->m_full_path);
    dto.LocalTransform() = Matrix4::MakeTranslateTransform(m_currentLoadingPawn->m_position);
    CommandBus::Post(std::make_shared<LoadPawnPrefab>(dto.ToGenericDto()));
}

void PawnEditService::onPrefabLoaded(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PrefabLoaded>(e);
    if (!ev) return;
    if (!m_currentLoadingPawn) return;
    if (ev->GetPrefabAtPath() != m_currentLoadingPawn->m_full_path) return;
    m_currentLoadingPawn = std::nullopt;
}

void PawnEditService::onLoadPrefabFailed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<LoadPrefabFailed>(e);
    if (!ev) return;
    if (!m_currentLoadingPawn) return;
    //if (ev->GetPrefabFilePath() != m_currentLoadingPawn->m_full_path) return;
    CommandBus::Post(std::make_shared<OutputMessage>(string_format("Load Pawn Failed : %s", ev->GetError().message().c_str())));
    m_currentLoadingPawn = std::nullopt;
}

void PawnEditService::putPawnAt(const std::shared_ptr<Pawn>& pawn, const Vector3& position)
{
    assert(pawn);
    CommandBus::Post(std::make_shared<AttachSceneRootChild>(pawn, Matrix4::MakeTranslateTransform(position)));
}
