#include "SceneGraphFactory.h"
#include "Camera.h"
#include "Pawn.h"
#include "Light.h"
#include "CameraFrustumCommands.h"
#include "SceneGraphCommands.h"
#include "CameraFrustumEvents.h"
#include "SceneGraphDtos.h"
#include "SceneGraphEvents.h"
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
}

void SceneGraphFactory::unregisterHandlers()
{
    CommandBus::unsubscribe(typeid(RegisterSpatialFactory), m_registerSpatialFactory);
    m_registerSpatialFactory = nullptr;
    CommandBus::unsubscribe(typeid(UnregisterSpatialFactory), m_unregisterSpatialFactory);
    m_unregisterSpatialFactory = nullptr;
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

std::shared_ptr<Light> SceneGraphFactory::createLight(const SpatialId& id, const LightInfo& info)
{
    auto light = std::make_shared<Light>(id, info);
    EventPublisher::post(std::make_shared<SpatialCreated>(id, light));
    return light;
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
