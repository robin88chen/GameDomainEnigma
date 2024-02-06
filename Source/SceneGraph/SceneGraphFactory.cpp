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
    registerHandlers();
}

SceneGraphFactory::~SceneGraphFactory()
{
    unregisterHandlers();
}

void SceneGraphFactory::registerHandlers()
{
    m_registerSpatialFactory = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { registerSpatialFactory(c); });
    CommandBus::subscribe(typeid(RegisterSpatialFactory), m_registerSpatialFactory);
    m_unregisterSpatialFactory = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { unregisterSpatialFactory(c); });
    CommandBus::subscribe(typeid(UnregisterSpatialFactory), m_unregisterSpatialFactory);
    m_createCamera = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { createCamera(c); });
    CommandBus::subscribe(typeid(CreateCamera), m_createCamera);
    m_constituteCamera = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { constituteCamera(c); });
    CommandBus::subscribe(typeid(ConstituteCamera), m_constituteCamera);
    m_createSpatial = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { createSpatial(c); });
    CommandBus::subscribe(typeid(CreateSpatial), m_createSpatial);
    m_constituteSpatial = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { constituteSpatial(c); });
    CommandBus::subscribe(typeid(ConstituteSpatial), m_constituteSpatial);
}

void SceneGraphFactory::unregisterHandlers()
{
    CommandBus::unsubscribe(typeid(RegisterSpatialFactory), m_registerSpatialFactory);
    m_registerSpatialFactory = nullptr;
    CommandBus::unsubscribe(typeid(UnregisterSpatialFactory), m_unregisterSpatialFactory);
    m_unregisterSpatialFactory = nullptr;
    CommandBus::unsubscribe(typeid(CreateCamera), m_createCamera);
    m_createCamera = nullptr;
    CommandBus::unsubscribe(typeid(ConstituteCamera), m_constituteCamera);
    m_constituteCamera = nullptr;
    CommandBus::unsubscribe(typeid(CreateSpatial), m_createSpatial);
    m_createSpatial = nullptr;
    CommandBus::unsubscribe(typeid(ConstituteSpatial), m_constituteSpatial);
    m_constituteSpatial = nullptr;
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

std::shared_ptr<Spatial> SceneGraphFactory::createSpatial(const SpatialId& id)
{
    auto creator = m_creators.find(id.rtti().getName());
    if (creator == m_creators.end())
    {
        Platforms::Debug::Printf("Can't find creator of %s\n", id.rtti().getName().c_str());
        EventPublisher::post(std::make_shared<CreateSpatialFailed>(id, ErrorCode::spatialFactoryNotFound));
        return nullptr;
    }
    auto spatial = creator->second(id);
    EventPublisher::post(std::make_shared<SpatialCreated>(id, spatial));
    return spatial;
}

std::shared_ptr <Spatial> SceneGraphFactory::constituteSpatial(const SpatialId& id, const Engine::GenericDto& dto, bool is_persisted)
{
    auto constitutor = m_constitutors.find(dto.getRtti().GetRttiName());
    if (constitutor == m_constitutors.end())
    {
        Platforms::Debug::Printf("Can't find constitutor of %s\n", dto.getRtti().GetRttiName().c_str());
        EventPublisher::post(std::make_shared<ConstituteSpatialFailed>(id, ErrorCode::spatialFactoryNotFound));
        return nullptr;
    }
    auto spatial = constitutor->second(id, dto);
    EventPublisher::post(std::make_shared<SpatialConstituted>(id, spatial, is_persisted));
    return spatial;
}

void SceneGraphFactory::registerSpatialFactory(const std::string& rtti, const SpatialCreator& creator, const SpatialConstitutor& constitutor)
{
    if (m_creators.find(rtti) != m_creators.end())
    {
        Platforms::Debug::Printf("Primitive factory of %s already exists\n", rtti.c_str());
        return;
    }
    m_creators[rtti] = creator;
    m_constitutors[rtti] = constitutor;
}

void SceneGraphFactory::unregisterSpatialFactory(const std::string& rtti)
{
    if (m_creators.find(rtti) == m_creators.end())
    {
        Platforms::Debug::Printf("Primitive factory of %s doesn't exist\n", rtti.c_str());
        return;
    }
    m_creators.erase(rtti);
    m_constitutors.erase(rtti);
}

void SceneGraphFactory::registerSpatialFactory(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<RegisterSpatialFactory>(c);
    if (!cmd) return;
    registerSpatialFactory(cmd->rttiName(), cmd->creator(), cmd->constitutor());
}

void SceneGraphFactory::unregisterSpatialFactory(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<UnregisterSpatialFactory>(c);
    if (!cmd) return;
    unregisterSpatialFactory(cmd->rttiName());
}

void SceneGraphFactory::createSpatial(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<CreateSpatial>(c);
    if (!cmd) return;
    const auto query = std::make_shared<QuerySpatial>(cmd->id());
    QueryDispatcher::dispatch(query);
    if (query->getResult())
    {
        EventPublisher::post(std::make_shared<CreateSpatialFailed>(cmd->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    if (createSpatial(cmd->id()) == nullptr)
    {
        EventPublisher::post(std::make_shared<CreateSpatialFailed>(cmd->id(), ErrorCode::sceneFactoryFailed));
    }
}

void SceneGraphFactory::constituteSpatial(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<ConstituteSpatial>(c);
    if (!cmd) return;
    const auto query = std::make_shared<QuerySpatial>(cmd->id());
    QueryDispatcher::dispatch(query);
    if (query->getResult())
    {
        EventPublisher::post(std::make_shared<ConstituteSpatialFailed>(cmd->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    if (constituteSpatial(cmd->id(), cmd->dto(), false) == nullptr)
    {
        EventPublisher::post(std::make_shared<ConstituteSpatialFailed>(cmd->id(), ErrorCode::sceneFactoryFailed));
    }
}
