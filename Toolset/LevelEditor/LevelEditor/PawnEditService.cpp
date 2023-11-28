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
#include "WorldMap/WorldMapQueries.h"
#include "WorldMap/WorldMapCommands.h"
#include "WorldMap/WorldMapEvents.h"
#include "Frameworks/QueryDispatcher.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;
using namespace Enigma::SceneGraph;
using namespace Enigma::GameCommon;
using namespace Enigma::MathLib;
using namespace Enigma::Prefabs;
using namespace Enigma::Engine;
using namespace Enigma::WorldMap;

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

ServiceResult PawnEditService::onInit()
{
    m_onPrefabLoaded = std::make_shared<EventSubscriber>([=](auto e) { onPrefabLoaded(e); });
    EventPublisher::subscribe(typeid(PawnPrefabLoaded), m_onPrefabLoaded);
    m_onLoadPrefabFailed = std::make_shared<EventSubscriber>([=](auto e) { onLoadPrefabFailed(e); });
    EventPublisher::subscribe(typeid(LoadPawnPrefabFailed), m_onLoadPrefabFailed);
    m_onFittingNodeCreated = std::make_shared<EventSubscriber>([=](auto e) { onFittingNodeCreated(e); });
    EventPublisher::subscribe(typeid(FittingNodeCreated), m_onFittingNodeCreated);
    m_onCreateFittingNodeFailed = std::make_shared<EventSubscriber>([=](auto e) { onCreateFittingNodeFailed(e); });
    EventPublisher::subscribe(typeid(CreateFittingNodeFailed), m_onCreateFittingNodeFailed);

    m_pawnLoader = new PawnLoader();

    return ServiceResult::Complete;
}

ServiceResult PawnEditService::onTick()
{
    if (m_currentLoadingPawn) return ServiceResult::Pendding;
    loadNextPawn();
    return ServiceResult::Pendding;
}

ServiceResult PawnEditService::onTerm()
{
    EventPublisher::unsubscribe(typeid(PawnLoaded), m_onPrefabLoaded);
    m_onPrefabLoaded = nullptr;
    EventPublisher::unsubscribe(typeid(LoadPawnPrefabFailed), m_onLoadPrefabFailed);
    m_onLoadPrefabFailed = nullptr;
    EventPublisher::unsubscribe(typeid(FittingNodeCreated), m_onFittingNodeCreated);
    m_onFittingNodeCreated = nullptr;
    EventPublisher::unsubscribe(typeid(CreateFittingNodeFailed), m_onCreateFittingNodeFailed);
    m_onCreateFittingNodeFailed = nullptr;

    SAFE_DELETE(m_pawnLoader);

    return ServiceResult::Complete;
}

error PawnEditService::putCandidatePawn(const std::string& name, const std::string& full_path, const Vector3& position)
{
    LoadingPawnMeta meta{ name, full_path, position };
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
    dto.name() = m_currentLoadingPawn->m_name;
    dto.isTopLevel() = true;
    dto.factoryDesc() = FactoryDesc(AnimatedPawn::TYPE_RTTI.getName()).ClaimByPrefab(m_currentLoadingPawn->m_full_path);
    dto.worldTransform() = Matrix4::MakeTranslateTransform(m_currentLoadingPawn->m_position);
    CommandBus::post(std::make_shared<LoadPawnPrefab>(dto.toGenericDto()));
}

void PawnEditService::onPrefabLoaded(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PawnPrefabLoaded>(e);
    if (!ev) return;
    if (!m_currentLoadingPawn) return;
    if (ev->getPrefabAtPath() != m_currentLoadingPawn->m_full_path) return;
    m_loadedPawn = ev->getPawn();
    auto has_put = tryPutPawnAt(m_loadedPawn, m_currentLoadingPawn->m_position);
    if (has_put)
    {
        completePutCandidatePawn();
    }
    else
    {
        createFittingNodeForPawn(m_loadedPawn, m_currentLoadingPawn->m_position);
    }
}

void PawnEditService::onLoadPrefabFailed(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<LoadPawnPrefabFailed>(e);
    if (!ev) return;
    if (!m_currentLoadingPawn) return;
    //if (ev->GetPrefabFilePath() != m_currentLoadingPawn->m_full_path) return;
    failPutCandidatePawn(ev->GetError());
}

void PawnEditService::onFittingNodeCreated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<FittingNodeCreated>(e);
    if (!ev) return;
    if (ev->getRequestRuid() != m_creatNodeRuid) return;
    auto has_put = tryPutPawnAt(m_loadedPawn, m_currentLoadingPawn->m_position);
    if (has_put)
    {
        completePutCandidatePawn();
    }
    else
    {
        failPutCandidatePawn(ErrorCode::pawnPutProcedureError);
    }
}

void PawnEditService::onCreateFittingNodeFailed(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<CreateFittingNodeFailed>(e);
    if (!ev) return;
    if (ev->getRequestRuid() != m_creatNodeRuid) return;
    failPutCandidatePawn(ev->getError());
}

bool PawnEditService::tryPutPawnAt(const std::shared_ptr<Pawn>& pawn, const Vector3& position)
{
    auto world_transform = Matrix4::MakeTranslateTransform(position);
    BoundingVolume bv = BoundingVolume::CreateFromTransform(pawn->getModelBound(), world_transform);
    auto query = std::make_shared<QueryFittingNode>(bv);
    QueryDispatcher::dispatch(query);
    if (query->getResult() == nullptr) return false;
    auto node = query->getResult();
    auto inv_node_transform = node->getWorldTransform().Inverse();
    CommandBus::post(std::make_shared<AttachNodeChild>(node->getSpatialName(), pawn, inv_node_transform * world_transform));
    return true;
}

void PawnEditService::createFittingNodeForPawn(const std::shared_ptr<Pawn>& pawn, const Vector3& position)
{
    auto world_transform = Matrix4::MakeTranslateTransform(position);
    BoundingVolume bv = BoundingVolume::CreateFromTransform(pawn->getModelBound(), world_transform);
    auto cmd = std::make_shared<CreateFittingQuadNode>(bv);
    m_creatNodeRuid = cmd->getRuid();
    CommandBus::post(cmd);
}

void PawnEditService::completePutCandidatePawn()
{
    CommandBus::post(std::make_shared<OutputMessage>(string_format("Pawn %s put at (%f, %f, %f)", m_currentLoadingPawn->m_name.c_str(), m_currentLoadingPawn->m_position.X(), m_currentLoadingPawn->m_position.Y(), m_currentLoadingPawn->m_position.Z())));
    m_currentLoadingPawn = std::nullopt;
    m_loadedPawn = nullptr;
}

void PawnEditService::failPutCandidatePawn(const error& err)
{
    CommandBus::post(std::make_shared<OutputMessage>(string_format("Pawn %s put failed : %s", m_currentLoadingPawn->m_name.c_str(), err.message().c_str())));
    m_currentLoadingPawn = std::nullopt;
    m_loadedPawn = nullptr;
}
