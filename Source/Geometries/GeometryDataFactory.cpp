﻿#include "GeometryDataFactory.h"
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
}

void GeometryDataFactory::unregisterHandlers()
{
    CommandBus::unsubscribe(typeid(RegisterGeometryFactory), m_registerGeometryFactory);
    m_registerGeometryFactory = nullptr;
    CommandBus::unsubscribe(typeid(UnRegisterGeometryFactory), m_unregisterGeometryFactory);
    m_unregisterGeometryFactory = nullptr;
}

std::shared_ptr<GeometryData> GeometryDataFactory::create(const GeometryId& id, const Rtti& rtti)
{
    auto creator = m_creators.find(rtti.getName());
    if (creator == m_creators.end())
    {
        Platforms::Debug::Printf("Can't find creator of %s\n", rtti.getName().c_str());
        EventPublisher::enqueue(std::make_shared<CreateGeometryFailed>(id, ErrorCode::geometryFactoryNotExists));
        return nullptr;
    }
    auto geo = creator->second(id);
    EventPublisher::enqueue(std::make_shared<GeometryCreated>(id, geo));
    return geo;
}

std::shared_ptr<GeometryData> GeometryDataFactory::constitute(const GeometryId& id, const GenericDto& dto, bool is_persisted)
{
    auto constitutor = m_constitutors.find(dto.getRtti().GetRttiName());
    if (constitutor == m_constitutors.end())
    {
        Platforms::Debug::Printf("Can't find constitutor of %s\n", dto.getRtti().GetRttiName().c_str());
        EventPublisher::enqueue(std::make_shared<ConstituteGeometryFailed>(id, ErrorCode::geometryFactoryNotExists));
        return nullptr;
    }
    auto geo = constitutor->second(id, dto);
    EventPublisher::enqueue(std::make_shared<GeometryConstituted>(id, geo, is_persisted));
    return geo;
}

void GeometryDataFactory::registerGeometryFactory(const std::string& rtti_name, const GeometryCreator& creator, const GeometryConstitutor& constitutor)
{
    if (m_creators.find(rtti_name) != m_creators.end())
    {
        Platforms::Debug::Printf("Geometry factory of %s already exists\n", rtti_name.c_str());
        return;
    }
    m_creators[rtti_name] = creator;
    m_constitutors[rtti_name] = constitutor;
}

void GeometryDataFactory::unregisterGeometryFactory(const std::string& rtti_name)
{
    if (m_creators.find(rtti_name) == m_creators.end())
    {
        Platforms::Debug::Printf("Geometry factory of %s doesn't exist\n", rtti_name.c_str());
        return;
    }
    m_creators.erase(rtti_name);
    m_constitutors.erase(rtti_name);
}
void GeometryDataFactory::registerGeometryFactory(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<RegisterGeometryFactory>(c);
    if (!cmd) return;
    registerGeometryFactory(cmd->rttiName(), cmd->creator(), cmd->constitutor());
}

void GeometryDataFactory::unregisterGeometryFactory(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<UnRegisterGeometryFactory>(c);
    if (!cmd) return;
    unregisterGeometryFactory(cmd->rttiName());
}

