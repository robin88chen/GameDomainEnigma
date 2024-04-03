#include "ViewerSceneGraphFileStoreMapper.h"
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
}

void ViewerSceneGraphFileStoreMapper::unsubscribeHandlers()
{
    Enigma::Frameworks::QueryDispatcher::unsubscribe(typeid(HasAnimatedPawn), m_hasAnimatedPawn);
    m_hasAnimatedPawn = nullptr;
}

bool ViewerSceneGraphFileStoreMapper::hasAnimatedPawn(const std::string& name)
{
    return m_spatialMap.has(Enigma::SceneGraph::SpatialId(name, Enigma::GameCommon::AnimatedPawn::TYPE_RTTI));
}

void ViewerSceneGraphFileStoreMapper::hasAnimatedPawn(const Enigma::Frameworks::IQueryPtr& q)
{
    const auto query = std::dynamic_pointer_cast<HasAnimatedPawn>(q);
    if (query == nullptr) return;
    query->setResult(hasAnimatedPawn(query->name()));
}

