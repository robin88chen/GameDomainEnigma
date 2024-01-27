#include "SceneGraphFactory.h"
#include "Camera.h"
#include "Pawn.h"
#include "CameraFrustumCommands.h"
#include "SceneGraphCommands.h"
#include "CameraFrustumEvents.h"
#include "SceneGraphDtos.h"
#include "SceneGraphEvents.h"
#include "SceneGraphQueries.h"
#include "SceneGraphErrors.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/QueryDispatcher.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;

SceneGraphFactory::SceneGraphFactory()
{

}

SceneGraphFactory::~SceneGraphFactory()
{

}

void SceneGraphFactory::registerHandlers()
{
    m_createCamera = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { createCamera(c); });
    CommandBus::subscribe(typeid(CreateCamera), m_createCamera);
    m_constituteCamera = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { constituteCamera(c); });
    CommandBus::subscribe(typeid(ConstituteCamera), m_constituteCamera);
    m_createPawn = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { createPawn(c); });
    CommandBus::subscribe(typeid(CreatePawn), m_createPawn);
    m_constitutePawn = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { constitutePawn(c); });
    CommandBus::subscribe(typeid(ConstitutePawn), m_constitutePawn);
}

void SceneGraphFactory::unregisterHandlers()
{
    CommandBus::unsubscribe(typeid(CreateCamera), m_createCamera);
    m_createCamera = nullptr;
    CommandBus::unsubscribe(typeid(ConstituteCamera), m_constituteCamera);
    m_constituteCamera = nullptr;
    CommandBus::unsubscribe(typeid(CreatePawn), m_createPawn);
    m_createPawn = nullptr;
    CommandBus::unsubscribe(typeid(ConstitutePawn), m_constitutePawn);
    m_constitutePawn = nullptr;
}

std::shared_ptr<Camera> SceneGraphFactory::createCamera(const SpatialId& id)
{
    auto camera = std::make_shared<Camera>(id, GraphicCoordSys::LeftHand);
    EventPublisher::post(std::make_shared<CameraCreated>(id, camera));
    return camera;
}

std::shared_ptr<Camera> SceneGraphFactory::constituteCamera(const SpatialId& id, const Engine::GenericDto& dto, bool is_persisted)
{
    auto camera = std::make_shared<Camera>(id, dto);
    EventPublisher::post(std::make_shared<CameraConstituted>(id, camera, is_persisted));
    return camera;
}

void SceneGraphFactory::createCamera(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<CreateCamera>(c);
    if (!cmd) return;
    const auto query = std::make_shared<QueryCamera>(cmd->id());
    QueryDispatcher::dispatch(query);
    if (query->getResult())
    {
        EventPublisher::post(std::make_shared<CreateCameraFailed>(cmd->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    if (createCamera(cmd->id()) == nullptr)
    {
        EventPublisher::post(std::make_shared<CreateCameraFailed>(cmd->id(), ErrorCode::sceneFactoryFailed));
    }
}

void SceneGraphFactory::constituteCamera(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<ConstituteCamera>(c);
    if (!cmd) return;
    const auto query = std::make_shared<QueryCamera>(cmd->id());
    QueryDispatcher::dispatch(query);
    if (query->getResult())
    {
        EventPublisher::post(std::make_shared<ConstituteCameraFailed>(cmd->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    if (constituteCamera(cmd->id(), cmd->dto(), false) == nullptr)
    {
        EventPublisher::post(std::make_shared<ConstituteCameraFailed>(cmd->id(), ErrorCode::sceneFactoryFailed));
    }
}

std::shared_ptr<Pawn> SceneGraphFactory::createPawn(const SpatialId& id)
{
    auto pawn = std::make_shared<Pawn>(id);
    EventPublisher::post(std::make_shared<PawnCreated>(id, pawn));
    return pawn;
}

std::shared_ptr<Pawn> SceneGraphFactory::constitutePawn(const SpatialId& id, const Engine::GenericDtoCollection& dtos)
{
    auto pawn = std::make_shared<Pawn>(id, dtos[0]);
    EventPublisher::post(std::make_shared<PawnConstituted>(id, pawn));
    return pawn;
}

void SceneGraphFactory::createPawn(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<CreatePawn>(c);
    if (!cmd) return;
    const auto query = std::make_shared<QuerySpatial>(cmd->id());
    QueryDispatcher::dispatch(query);
    if (query->getResult())
    {
        EventPublisher::post(std::make_shared<CreatePawnFailed>(cmd->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    if (createPawn(cmd->id()) == nullptr)
    {
        EventPublisher::post(std::make_shared<CreatePawnFailed>(cmd->id(), ErrorCode::sceneFactoryFailed));
    }
}

void SceneGraphFactory::constitutePawn(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<ConstitutePawn>(c);
    if (!cmd) return;
    const auto query = std::make_shared<QuerySpatial>(cmd->id());
    QueryDispatcher::dispatch(query);
    if (query->getResult())
    {
        EventPublisher::post(std::make_shared<ConstitutePawnFailed>(cmd->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    if (constitutePawn(cmd->id(), cmd->dtos()) == nullptr)
    {
        EventPublisher::post(std::make_shared<ConstitutePawnFailed>(cmd->id(), ErrorCode::sceneFactoryFailed));
    }
}
