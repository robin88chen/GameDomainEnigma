#include "PrimitiveRepository.h"
#include "PrimitiveFactory.h"
#include "PrimitiveStoreMapper.h"
#include "PrimitiveEvents.h"
#include "PrimitiveQueries.h"
#include "Primitive.h"
#include "PrimitiveErrors.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/QueryDispatcher.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include "PrimitiveCommands.h"
#include "Frameworks/CommandBus.h"

using namespace Enigma::Primitives;
using namespace Enigma::Frameworks;

DEFINE_RTTI(Primitives, PrimitiveRepository, ISystemService);

using error = std::error_code;

PrimitiveRepository::PrimitiveRepository(ServiceManager* srv_manager, const std::shared_ptr<PrimitiveStoreMapper>& store_mapper)
    : ISystemService(srv_manager), m_storeMapper(store_mapper)
{
    m_factory = menew PrimitiveFactory();
    m_needTick = false;
}

PrimitiveRepository::~PrimitiveRepository()
{
    SAFE_DELETE(m_factory);
}

ServiceResult PrimitiveRepository::onInit()
{
    assert(m_storeMapper);

    m_queryPrimitive = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { queryPrimitive(q); });
    QueryDispatcher::subscribe(typeid(QueryPrimitive), m_queryPrimitive);
    m_queryPrimitiveNextSequenceNumber = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { queryPrimitiveNextSequenceNumber(q); });
    QueryDispatcher::subscribe(typeid(QueryPrimitiveNextSequenceNumber), m_queryPrimitiveNextSequenceNumber);
    m_requestPrimitiveCreation = std::make_shared<QuerySubscriber>([=](const IQueryPtr& r) { requestPrimitiveCreation(r); });
    QueryDispatcher::subscribe(typeid(RequestPrimitiveCreation), m_requestPrimitiveCreation);
    m_requestPrimitiveConstitution = std::make_shared<QuerySubscriber>([=](const IQueryPtr& r) { requestPrimitiveConstitution(r); });
    QueryDispatcher::subscribe(typeid(RequestPrimitiveConstitution), m_requestPrimitiveConstitution);

    m_putPrimitive = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { putPrimitive(c); });
    CommandBus::subscribe(typeid(PutPrimitive), m_putPrimitive);
    m_removePrimitive = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { removePrimitive(c); });
    CommandBus::subscribe(typeid(RemovePrimitive), m_removePrimitive);

    m_storeMapper->connect();

    return ServiceResult::Complete;
}

ServiceResult PrimitiveRepository::onTerm()
{
    assert(m_storeMapper);
    m_storeMapper->disconnect();
    m_primitives.clear();

    QueryDispatcher::unsubscribe(typeid(QueryPrimitive), m_queryPrimitive);
    m_queryPrimitive = nullptr;
    QueryDispatcher::unsubscribe(typeid(QueryPrimitiveNextSequenceNumber), m_queryPrimitiveNextSequenceNumber);
    m_queryPrimitiveNextSequenceNumber = nullptr;
    QueryDispatcher::unsubscribe(typeid(RequestPrimitiveCreation), m_requestPrimitiveCreation);
    m_requestPrimitiveCreation = nullptr;
    QueryDispatcher::unsubscribe(typeid(RequestPrimitiveConstitution), m_requestPrimitiveConstitution);
    m_requestPrimitiveConstitution = nullptr;

    CommandBus::unsubscribe(typeid(PutPrimitive), m_putPrimitive);
    m_putPrimitive = nullptr;
    CommandBus::unsubscribe(typeid(RemovePrimitive), m_removePrimitive);
    m_removePrimitive = nullptr;

    return ServiceResult::Complete;
}

bool PrimitiveRepository::hasPrimitive(const PrimitiveId& id)
{
    assert(m_storeMapper);
    std::lock_guard locker{ m_primitiveLock };
    const auto it = m_primitives.find(id);
    if (it != m_primitives.end()) return true;
    return m_storeMapper->hasPrimitive(id.origin());
}

std::shared_ptr<Primitive> PrimitiveRepository::queryPrimitive(const PrimitiveId& id)
{
    if (!hasPrimitive(id)) return nullptr;
    std::lock_guard locker{ m_primitiveLock };
    auto it = m_primitives.find(id);
    if (it != m_primitives.end()) return it->second;
    assert(m_factory);
    const auto dto = m_storeMapper->queryPrimitive(id.origin());
    assert(dto.has_value());
    auto prim = m_factory->constitute(id, dto.value(), true);
    assert(prim);
    m_primitives.insert_or_assign(id, prim);
    return prim;
}

void PrimitiveRepository::removePrimitive(const PrimitiveId& id)
{
    if (!hasPrimitive(id)) return;
    std::lock_guard locker{ m_primitiveLock };
    m_primitives.erase(id);
    error er = m_storeMapper->removePrimitive(id.origin());
    if (er)
    {
        Platforms::Debug::ErrorPrintf("remove primitive %s failed : %s\n", id.name().c_str(), er.message().c_str());
        EventPublisher::post(std::make_shared<RemovePrimitiveFailed>(id, er));
    }
}

void PrimitiveRepository::putPrimitive(const PrimitiveId& id, const std::shared_ptr<Primitive>& primitive)
{
    if (hasPrimitive(id)) return;
    std::lock_guard locker{ m_primitiveLock };
    m_primitives.insert_or_assign(id, primitive);
    if (id != id.origin()) return;  // only put origin primitive to store
    error er = m_storeMapper->putPrimitive(id.origin(), primitive->serializeDto());
    if (er)
    {
        Platforms::Debug::ErrorPrintf("put primitive %s failed : %s\n", id.name().c_str(), er.message().c_str());
        EventPublisher::post(std::make_shared<PutPrimitiveFailed>(id, er));
    }
}

std::uint64_t PrimitiveRepository::nextSequenceNumber()
{
    assert(m_storeMapper);
    return m_storeMapper->nextSequenceNumber();
}

void PrimitiveRepository::queryPrimitive(const IQueryPtr& q)
{
    if (!q) return;
    const auto query = std::dynamic_pointer_cast<QueryPrimitive, IQuery>(q);
    if (!query) return;
    query->setResult(queryPrimitive(query->id()));
}

void PrimitiveRepository::queryPrimitiveNextSequenceNumber(const IQueryPtr& q)
{
    if (!q) return;
    const auto query = std::dynamic_pointer_cast<QueryPrimitiveNextSequenceNumber, IQuery>(q);
    if (!query) return;
    query->setResult(nextSequenceNumber());
}

void PrimitiveRepository::requestPrimitiveCreation(const Frameworks::IQueryPtr& r)
{
    assert(m_factory);
    if (!r) return;
    auto request = std::dynamic_pointer_cast<RequestPrimitiveCreation>(r);
    if (!request) return;
    if (hasPrimitive(request->id()))
    {
        EventPublisher::post(std::make_shared<CreatePrimitiveFailed>(request->id(), ErrorCode::primitiveEntityAlreadyExists));
        return;
    }
    auto primitive = m_factory->create(request->id(), request->rtti());
    if (request->persistenceLevel() == PersistenceLevel::Repository)
    {
        std::lock_guard locker{ m_primitiveLock };
        m_primitives.insert_or_assign(request->id(), primitive);
    }
    else if (request->persistenceLevel() == PersistenceLevel::Store)
    {
        putPrimitive(request->id(), primitive);
    }
    request->setResult(primitive);
}

void PrimitiveRepository::requestPrimitiveConstitution(const Frameworks::IQueryPtr& r)
{
    if (!r) return;
    auto request = std::dynamic_pointer_cast<RequestPrimitiveConstitution>(r);
    if (!request) return;
    if (hasPrimitive(request->id()))
    {
        EventPublisher::post(std::make_shared<ConstitutePrimitiveFailed>(request->id(), ErrorCode::primitiveEntityAlreadyExists));
        return;
    }
    auto primitive = m_factory->constitute(request->id(), request->dto(), false);
    if (request->persistenceLevel() == PersistenceLevel::Repository)
    {
        std::lock_guard locker{ m_primitiveLock };
        m_primitives.insert_or_assign(request->id(), primitive);
    }
    else if (request->persistenceLevel() == PersistenceLevel::Store)
    {
        putPrimitive(request->id(), primitive);
    }
    request->setResult(primitive);
}

void PrimitiveRepository::putPrimitive(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<PutPrimitive>(c);
    if (!cmd) return;
    putPrimitive(cmd->id(), cmd->primitive());
}

void PrimitiveRepository::removePrimitive(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<RemovePrimitive>(c);
    if (!cmd) return;
    removePrimitive(cmd->id());
}
