#include "PrimitiveRepository.h"
#include "PrimitiveFactory.h"
#include "PrimitiveStoreMapper.h"
#include "PrimitiveEvents.h"
#include "PrimitiveQueries.h"
#include "Primitive.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/QueryDispatcher.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

DEFINE_RTTI(Engine, PrimitiveRepository, ISystemService);

using error = std::error_code;

PrimitiveRepository::PrimitiveRepository(Frameworks::ServiceManager* srv_manager, const std::shared_ptr<PrimitiveStoreMapper>& store_mapper)
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

    m_storeMapper->connect();

    return ServiceResult::Complete;
}

ServiceResult PrimitiveRepository::onTerm()
{
    assert(m_storeMapper);
    m_storeMapper->disconnect();

    QueryDispatcher::unsubscribe(typeid(QueryPrimitive), m_queryPrimitive);
    m_queryPrimitive = nullptr;

    return ServiceResult::Complete;
}

bool PrimitiveRepository::hasPrimitive(const PrimitiveId& id)
{
    assert(m_storeMapper);
    std::lock_guard locker{ m_primitiveLock };
    const auto it = m_primitives.find(id);
    if (it != m_primitives.end()) return true;
    return m_storeMapper->hasPrimitive(id);
}

std::shared_ptr<Primitive> PrimitiveRepository::queryPrimitive(const PrimitiveId& id)
{
    if (!hasPrimitive(id)) return nullptr;
    std::lock_guard locker{ m_primitiveLock };
    auto it = m_primitives.find(id);
    if (it != m_primitives.end()) return it->second;
    assert(m_factory);
    const auto dto = m_storeMapper->queryPrimitive(id);
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
    error er = m_storeMapper->removePrimitive(id);
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
    error er = m_storeMapper->putPrimitive(id, primitive->serializeDto());
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

void PrimitiveRepository::queryPrimitive(const Frameworks::IQueryPtr& q)
{
    if (!q) return;
    const auto query = std::dynamic_pointer_cast<QueryPrimitive, IQuery>(q);
    if (!query) return;
    query->setResult(queryPrimitive(query->id()));
}
