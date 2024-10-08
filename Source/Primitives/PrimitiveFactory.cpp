#include "PrimitiveFactory.h"
#include "Frameworks/EventPublisher.h"
#include "PrimitiveEvents.h"
#include "Platforms/PlatformLayer.h"
#include "PrimitiveCommands.h"
#include "Frameworks/CommandBus.h"
#include "PrimitiveErrors.h"
#include "PrimitiveAssembler.h"

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
        EventPublisher::enqueue(std::make_shared<PrimitiveCreationFailed>(id, ErrorCode::primitiveFactoryNotExists));
        return nullptr;
    }
    auto prim = creator->second(id);
    EventPublisher::enqueue(std::make_shared<PrimitiveCreated>(id, prim));
    return prim;
}

std::shared_ptr<Primitive> PrimitiveFactory::constitute(const PrimitiveId& id, const Engine::GenericDto& dto, bool is_persisted)
{
    auto creator = m_creators.find(dto.getRtti().rttiName());
    if (creator == m_creators.end())
    {
        Platforms::Debug::Printf("Can't find constitutor of %s\n", dto.getRtti().rttiName().c_str());
        EventPublisher::enqueue(std::make_shared<PrimitiveConstitutionFailed>(id, ErrorCode::primitiveFactoryNotExists));
        return nullptr;
    }
    auto prim = creator->second(id);
    auto disassembler = prim->disassembler();
    disassembler->disassemble(dto);
    prim->disassemble(disassembler);
    EventPublisher::enqueue(std::make_shared<PrimitiveConstituted>(id, prim, is_persisted));
    return prim;
}

void PrimitiveFactory::registerPrimitiveFactory(const std::string& rtti, const PrimitiveCreator& creator)
{
    if (m_creators.find(rtti) != m_creators.end())
    {
        Platforms::Debug::Printf("Primitive factory of %s already exists\n", rtti.c_str());
        return;
    }
    m_creators[rtti] = creator;
}

void PrimitiveFactory::unregisterPrimitiveFactory(const std::string& rtti)
{
    if (m_creators.find(rtti) == m_creators.end())
    {
        Platforms::Debug::Printf("Primitive factory of %s doesn't exist\n", rtti.c_str());
        return;
    }
    m_creators.erase(rtti);
}

void PrimitiveFactory::registerPrimitiveFactory(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<RegisterPrimitiveFactory>(c);
    if (!cmd) return;
    registerPrimitiveFactory(cmd->rttiName(), cmd->creator());
}

void PrimitiveFactory::unregisterPrimitiveFactory(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<UnregisterPrimitiveFactory>(c);
    if (!cmd) return;
    unregisterPrimitiveFactory(cmd->rttiName());
}

