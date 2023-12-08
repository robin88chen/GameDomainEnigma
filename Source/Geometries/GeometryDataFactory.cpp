#include "GeometryDataFactory.h"
#include "GeometryErrors.h"
#include "Frameworks/EventPublisher.h"
#include "GeometryDataEvents.h"
#include "Platforms/PlatformLayer.h"
#include "GeometryCommands.h"
#include "Frameworks/CommandBus.h"
#include "GeometryDataQueries.h"
#include "Frameworks/QueryDispatcher.h"

using namespace Enigma::Geometries;
using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

GeometryDataFactory::GeometryDataFactory()
{
    registerHandlers();
}

GeometryDataFactory::~GeometryDataFactory()
{
    unregisterHandlers();
}

void GeometryDataFactory::registerHandlers()
{
    m_registerGeometryFactory = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { registerGeometryFactory(c); });
    CommandBus::subscribe(typeid(RegisterGeometryFactory), m_registerGeometryFactory);
    m_unregisterGeometryFactory = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { unregisterGeometryFactory(c); });
    CommandBus::subscribe(typeid(UnRegisterGeometryFactory), m_unregisterGeometryFactory);
    m_createGeometry = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { createGeometry(c); });
    CommandBus::subscribe(typeid(CreateGeometry), m_createGeometry);
    m_constituteGeometry = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { constituteGeometry(c); });
    CommandBus::subscribe(typeid(ConstituteGeometry), m_constituteGeometry);
}

void GeometryDataFactory::unregisterHandlers()
{
    CommandBus::unsubscribe(typeid(RegisterGeometryFactory), m_registerGeometryFactory);
    m_registerGeometryFactory = nullptr;
    CommandBus::unsubscribe(typeid(UnRegisterGeometryFactory), m_unregisterGeometryFactory);
    m_unregisterGeometryFactory = nullptr;
    CommandBus::unsubscribe(typeid(CreateGeometry), m_createGeometry);
    m_createGeometry = nullptr;
    CommandBus::unsubscribe(typeid(ConstituteGeometry), m_constituteGeometry);
    m_constituteGeometry = nullptr;
}

std::shared_ptr<GeometryData> GeometryDataFactory::create(const GeometryId& id, const Rtti& rtti)
{
    auto creator = m_creators.find(rtti.getName());
    if (creator == m_creators.end())
    {
        Platforms::Debug::Printf("Can't find creator of %s\n", rtti.getName().c_str());
        EventPublisher::post(std::make_shared<CreateGeometryFailed>(id, ErrorCode::geometryFactoryNotExists));
        return nullptr;
    }
    auto geo = creator->second(id);
    EventPublisher::post(std::make_shared<GeometryCreated>(id, geo));
    return geo;
}

std::shared_ptr<GeometryData> GeometryDataFactory::constitute(const GeometryId& id, const GenericDto& dto)
{
    auto constitutor = m_constitutors.find(dto.GetRtti().GetRttiName());
    if (constitutor == m_constitutors.end())
    {
        Platforms::Debug::Printf("Can't find constitutor of %s\n", dto.GetRtti().GetRttiName().c_str());
        EventPublisher::post(std::make_shared<ConstituteGeometryFailed>(id, ErrorCode::geometryFactoryNotExists));
        return nullptr;
    }
    auto geo = constitutor->second(id, dto);
    EventPublisher::post(std::make_shared<GeometryConstituted>(id, geo));
    return geo;
}

void GeometryDataFactory::registerGeometryFactory(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<RegisterGeometryFactory>(c);
    if (!cmd) return;
    if (m_creators.find(cmd->rttiName()) != m_creators.end())
    {
        Platforms::Debug::Printf("Geometry factory of %s already exists\n", cmd->rttiName().c_str());
        return;
    }
    m_creators[cmd->rttiName()] = cmd->creator();
    m_constitutors[cmd->rttiName()] = cmd->constitutor();
}

void GeometryDataFactory::unregisterGeometryFactory(const ICommandPtr& c)
{
    auto cmd = std::dynamic_pointer_cast<UnRegisterGeometryFactory>(c);
    if (m_creators.find(cmd->rttiName()) == m_creators.end())
    {
        Platforms::Debug::Printf("Geometry factory of %s doesn't exist\n", cmd->rttiName().c_str());
        return;
    }
    m_creators.erase(cmd->rttiName());
    m_constitutors.erase(cmd->rttiName());
}

void GeometryDataFactory::createGeometry(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<CreateGeometry>(c);
    if (!cmd) return;
    const auto query = std::make_shared<QueryGeometryData>(cmd->id());
    QueryDispatcher::dispatch(query);
    if (query->getResult())
    {
        EventPublisher::post(std::make_shared<CreateGeometryFailed>(cmd->id(), ErrorCode::geometryEntityAlreadyExists));
        return;
    }
    create(cmd->id(), cmd->rtti());
}

void GeometryDataFactory::constituteGeometry(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<ConstituteGeometry>(c);
    if (!cmd) return;
    const auto query = std::make_shared<QueryGeometryData>(cmd->id());
    QueryDispatcher::dispatch(query);
    if (query->getResult())
    {
        EventPublisher::post(std::make_shared<ConstituteGeometryFailed>(cmd->id(), ErrorCode::geometryEntityAlreadyExists));
        return;
    }
    constitute(cmd->id(), cmd->dto());
}
