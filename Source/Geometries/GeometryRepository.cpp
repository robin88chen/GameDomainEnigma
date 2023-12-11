#include "GeometryRepository.h"
#include "GeometryDataStoreMapper.h"
#include "TriangleList.h"
#include "GeometryDataEvents.h"
#include "GeometryDataPolicy.h"
#include "GeometryBuilder.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include "Frameworks/CommandBus.h"
#include "GeometryDataQueries.h"
#include "Frameworks/QueryDispatcher.h"
#include "GeometryDataFactory.h"

using namespace Enigma::Geometries;
using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

DEFINE_RTTI(Geometries, GeometryRepository, ISystemService);

GeometryRepository::GeometryRepository(Frameworks::ServiceManager* srv_manager, const std::shared_ptr<GeometryDataStoreMapper>& store_mapper) : ISystemService(srv_manager)
{
    m_storeMapper = store_mapper;
    m_factory = menew GeometryDataFactory();
    m_needTick = false;

    m_factory->registerGeometryFactory(TriangleList::TYPE_RTTI.getName(),
        [=](auto id) { return std::make_shared<TriangleList>(id); },
        [=](auto id, auto o) { return std::make_shared<TriangleList>(id, o); });
}

GeometryRepository::~GeometryRepository()
{
    if (m_factory) m_factory->unregisterGeometryFactory(TriangleList::TYPE_RTTI.getName());
    SAFE_DELETE(m_factory);
}

ServiceResult GeometryRepository::onInit()
{
    m_queryGeometryData = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { return this->queryGeometryData(q); });
    QueryDispatcher::subscribe(typeid(QueryGeometryData), m_queryGeometryData);

    m_storeMapper->connect();

    return Frameworks::ServiceResult::Complete;
}

ServiceResult GeometryRepository::onTerm()
{
    m_storeMapper->disconnect();
    m_geometries.clear();

    QueryDispatcher::unsubscribe(typeid(QueryGeometryData), m_queryGeometryData);
    m_queryGeometryData = nullptr;

    return Frameworks::ServiceResult::Complete;
}

bool GeometryRepository::hasGeometryData(const GeometryId& id)
{
    assert(m_storeMapper);
    std::lock_guard locker{ m_geometryLock };
    const auto it = m_geometries.find(id);
    if (it != m_geometries.end()) return true;
    return m_storeMapper->hasGeometry(id);
}

std::shared_ptr<GeometryData> GeometryRepository::queryGeometryData(const GeometryId& id)
{
    if (!hasGeometryData(id)) return nullptr;
    std::lock_guard locker{ m_geometryLock };
    auto it = m_geometries.find(id);
    if (it != m_geometries.end()) return it->second;
    assert(m_factory);
    const auto dto = m_storeMapper->queryGeometry(id);
    assert(dto.has_value());
    return m_factory->constitute(id, dto.value());
}

void GeometryRepository::queryGeometryData(const Frameworks::IQueryPtr& q)
{
    if (!q) return;
    const auto query = std::dynamic_pointer_cast<QueryGeometryData, IQuery>(q);
    if (!query) return;
    query->setResult(queryGeometryData(query->id()));
}

void GeometryRepository::removeGeometryData(const GeometryId& id)
{
    if (!hasGeometryData(id)) return;
    std::lock_guard locker{ m_geometryLock };
    m_geometries.erase(id);
    error er = m_storeMapper->removeGeometry(id);
    if (er)
    {
        Platforms::Debug::ErrorPrintf("remove geometry data %s failed : %s\n", id.name().c_str(), er.message().c_str());
        EventPublisher::post(std::make_shared<RemoveGeometryFailed>(id, er));
    }
}

void GeometryRepository::putGeometryData(const GeometryId& id, const std::shared_ptr<GeometryData>& data)
{
    if (hasGeometryData(id)) return;
    std::lock_guard locker{ m_geometryLock };
    m_geometries.insert_or_assign(id, data);
    error er = m_storeMapper->putGeometry(id, data->serializeDto());
    if (er)
    {
        Platforms::Debug::ErrorPrintf("put geometry data %s failed : %s\n", id.name().c_str(), er.message().c_str());
        EventPublisher::post(std::make_shared<PutGeometryFailed>(id, er));
    }
}
