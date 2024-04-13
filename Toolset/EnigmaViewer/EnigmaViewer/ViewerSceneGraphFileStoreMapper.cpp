#include "ViewerSceneGraphFileStoreMapper.h"

#include <Renderables/ModelPrimitive.h>

#include "GameCommon/AnimatedPawn.h"
#include "ViewerQueries.h"
#include "Frameworks/QueryDispatcher.h"

using namespace EnigmaViewer;

ViewerSceneGraphFileStoreMapper::ViewerSceneGraphFileStoreMapper(const std::string& mapper_filename, const std::string& lazied_mapper_filename, const std::shared_ptr<Enigma::Gateways::IDtoGateway>& gateway) : SceneGraphFileStoreMapper(mapper_filename, lazied_mapper_filename, gateway)
{
}

ViewerSceneGraphFileStoreMapper::~ViewerSceneGraphFileStoreMapper()
{
}

void ViewerSceneGraphFileStoreMapper::subscribeHandlers()
{
    m_hasAnimatedPawn = std::make_shared<Enigma::Frameworks::QuerySubscriber>([=](const Enigma::Frameworks::IQueryPtr& q) { hasAnimatedPawn(q); });
    Enigma::Frameworks::QueryDispatcher::subscribe(typeid(HasAnimatedPawn), m_hasAnimatedPawn);
    m_requestPawnNames = std::make_shared<Enigma::Frameworks::QuerySubscriber>([=](const Enigma::Frameworks::IQueryPtr& q) { requestPawnNames(q); });
    Enigma::Frameworks::QueryDispatcher::subscribe(typeid(RequestPawnNames), m_requestPawnNames);
    m_resolvePawnId = std::make_shared<Enigma::Frameworks::QuerySubscriber>([=](const Enigma::Frameworks::IQueryPtr& q) { resolvePawnId(q); });
    Enigma::Frameworks::QueryDispatcher::subscribe(typeid(ResolvePawnId), m_resolvePawnId);
}

void ViewerSceneGraphFileStoreMapper::unsubscribeHandlers()
{
    Enigma::Frameworks::QueryDispatcher::unsubscribe(typeid(HasAnimatedPawn), m_hasAnimatedPawn);
    m_hasAnimatedPawn = nullptr;
    Enigma::Frameworks::QueryDispatcher::unsubscribe(typeid(RequestPawnNames), m_requestPawnNames);
    m_requestPawnNames = nullptr;
    Enigma::Frameworks::QueryDispatcher::unsubscribe(typeid(ResolvePawnId), m_resolvePawnId);
    m_resolvePawnId = nullptr;
}

bool ViewerSceneGraphFileStoreMapper::hasAnimatedPawn(const std::string& name)
{
    return m_spatialMap.has(Enigma::SceneGraph::SpatialId(name, Enigma::GameCommon::AnimatedPawn::TYPE_RTTI));
}

std::vector<std::string> ViewerSceneGraphFileStoreMapper::pawnNames() const
{
    std::vector<std::string> pawn_names;
    for (const auto& [id, filename] : m_spatialMap.map())
    {
        if (id.rtti().isDerived(Enigma::GameCommon::AnimatedPawn::TYPE_RTTI)) pawn_names.push_back(id.name());
    }
    return pawn_names;
}

std::optional<Enigma::SceneGraph::SpatialId> ViewerSceneGraphFileStoreMapper::pawnId(const std::string& pawn_name) const
{
    for (const auto& [id, filename] : m_spatialMap.map())
    {
        if (id.name() == pawn_name) return id;
    }
    return std::nullopt;
}

void ViewerSceneGraphFileStoreMapper::hasAnimatedPawn(const Enigma::Frameworks::IQueryPtr& q)
{
    const auto query = std::dynamic_pointer_cast<HasAnimatedPawn>(q);
    if (query == nullptr) return;
    query->setResult(hasAnimatedPawn(query->name()));
}

void ViewerSceneGraphFileStoreMapper::requestPawnNames(const Enigma::Frameworks::IQueryPtr& q)
{
    const auto query = std::dynamic_pointer_cast<RequestPawnNames>(q);
    if (query == nullptr) return;
    query->setResult(pawnNames());
}

void ViewerSceneGraphFileStoreMapper::resolvePawnId(const Enigma::Frameworks::IQueryPtr& q)
{
    const auto query = std::dynamic_pointer_cast<ResolvePawnId>(q);
    if (query == nullptr) return;
    query->setResult(pawnId(query->pawnName()));
}

