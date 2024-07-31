﻿#include "PrimitiveFactory.h"
#include "Frameworks/EventPublisher.h"
#include "PrimitiveEvents.h"
#include "Platforms/PlatformLayer.h"
#include "PrimitiveCommands.h"
#include "Frameworks/CommandBus.h"
#include "PrimitiveErrors.h"
#include "PrimitiveQueries.h"
#include "Frameworks/QueryDispatcher.h"

using namespace Enigma::Primitives;
using namespace Enigma::Frameworks;

PrimitiveFactory::PrimitiveFactory()
{
    registerHandlers();
}

PrimitiveFactory::~PrimitiveFactory()
{
    unregisterHandlers();
}

void PrimitiveFactory::registerHandlers()
{
    m_registerPrimitiveFactory = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { registerPrimitiveFactory(c); });
    CommandBus::subscribe(typeid(RegisterPrimitiveFactory), m_registerPrimitiveFactory);
    m_unregisterPrimitiveFactory = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { unregisterPrimitiveFactory(c); });
    CommandBus::subscribe(typeid(UnregisterPrimitiveFactory), m_unregisterPrimitiveFactory);
}

void PrimitiveFactory::unregisterHandlers()
{
    CommandBus::unsubscribe(typeid(RegisterPrimitiveFactory), m_registerPrimitiveFactory);
    m_registerPrimitiveFactory = nullptr;
    CommandBus::unsubscribe(typeid(UnregisterPrimitiveFactory), m_unregisterPrimitiveFactory);
    m_unregisterPrimitiveFactory = nullptr;
}

std::shared_ptr<Primitive> PrimitiveFactory::create(const PrimitiveId& id, const Frameworks::Rtti& rtti)
{
    auto creator = m_creators.find(rtti.getName());
    if (creator == m_creators.end())
    {
        Platforms::Debug::Printf("Can't find creator of %s\n", rtti.getName().c_str());
        EventPublisher::enqueue(std::make_shared<CreatePrimitiveFailed>(id, ErrorCode::primitiveFactoryNotExists));
        return nullptr;
    }
    auto prim = creator->second(id);
    EventPublisher::enqueue(std::make_shared<PrimitiveCreated>(id, prim));
    return prim;
}

std::shared_ptr<Primitive> PrimitiveFactory::constitute(const PrimitiveId& id, const Engine::GenericDto& dto, bool is_persisted)
{
    auto constitutor = m_constitutors.find(dto.getRtti().GetRttiName());
    if (constitutor == m_constitutors.end())
    {
        Platforms::Debug::Printf("Can't find constitutor of %s\n", dto.getRtti().GetRttiName().c_str());
        EventPublisher::enqueue(std::make_shared<ConstitutePrimitiveFailed>(id, ErrorCode::primitiveFactoryNotExists));
        return nullptr;
    }
    auto prim = constitutor->second(id, dto);
    EventPublisher::enqueue(std::make_shared<PrimitiveConstituted>(id, prim, is_persisted));
    return prim;
}

void PrimitiveFactory::registerPrimitiveFactory(const std::string& rtti, const PrimitiveCreator& creator, const PrimitiveConstitutor& constitutor)
{
    if (m_creators.find(rtti) != m_creators.end())
    {
        Platforms::Debug::Printf("Primitive factory of %s already exists\n", rtti.c_str());
        return;
    }
    m_creators[rtti] = creator;
    m_constitutors[rtti] = constitutor;
}

void PrimitiveFactory::unregisterPrimitiveFactory(const std::string& rtti)
{
    if (m_creators.find(rtti) == m_creators.end())
    {
        Platforms::Debug::Printf("Primitive factory of %s doesn't exist\n", rtti.c_str());
        return;
    }
    m_creators.erase(rtti);
    m_constitutors.erase(rtti);
}

void PrimitiveFactory::registerPrimitiveFactory(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<RegisterPrimitiveFactory>(c);
    if (!cmd) return;
    registerPrimitiveFactory(cmd->rttiName(), cmd->creator(), cmd->constitutor());
}

void PrimitiveFactory::unregisterPrimitiveFactory(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<UnregisterPrimitiveFactory>(c);
    if (!cmd) return;
    unregisterPrimitiveFactory(cmd->rttiName());
}

