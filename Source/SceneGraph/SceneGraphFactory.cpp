#include "SceneGraphFactory.h"
#include "Camera.h"
#include "CameraFrustumCommands.h"
#include "CameraFrustumEvents.h"
#include "SceneGraphQueries.h"
#include "SceneGraphErrors.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/QueryDispatcher.h"

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
}

void SceneGraphFactory::unregisterHandlers()
{
    CommandBus::unsubscribe(typeid(CreateCamera), m_createCamera);
    m_createCamera = nullptr;
    CommandBus::unsubscribe(typeid(ConstituteCamera), m_constituteCamera);
    m_constituteCamera = nullptr;
}

std::shared_ptr<Camera> SceneGraphFactory::createCamera(const SpatialId& id)
{
    return std::make_shared<Camera>(id, GraphicCoordSys::LeftHand);
}

std::shared_ptr<Camera> SceneGraphFactory::constituteCamera(const Engine::GenericDto& dto)
{
    return std::make_shared<Camera>(dto);
}

void SceneGraphFactory::createCamera(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<CreateCamera>(c);
    if (!cmd) return;
    auto query = std::make_shared<QueryCamera>(cmd->id());
    QueryDispatcher::dispatch(query);
    if (query->getResult())
    {
        EventPublisher::post(std::make_shared<CreateCameraFailed>(cmd->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    auto camera = createCamera(cmd->id());
    if (camera)
    {
        EventPublisher::post(std::make_shared<CameraCreated>(cmd->id(), camera));
    }
    else
    {
        EventPublisher::post(std::make_shared<CreateCameraFailed>(cmd->id(), ErrorCode::sceneFactoryFailed));
    }
}

void SceneGraphFactory::constituteCamera(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<ConstituteCamera>(c);
    if (!cmd) return;
    auto query = std::make_shared<QueryCamera>(cmd->id());
    QueryDispatcher::dispatch(query);
    if (query->getResult())
    {
        EventPublisher::post(std::make_shared<ConstituteCameraFailed>(cmd->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    auto camera = constituteCamera(cmd->dto());
    if (camera)
    {
        EventPublisher::post(std::make_shared<CameraConstituted>(cmd->id(), camera));
    }
    else
    {
        EventPublisher::post(std::make_shared<ConstituteCameraFailed>(cmd->id(), ErrorCode::sceneFactoryFailed));
    }
}
