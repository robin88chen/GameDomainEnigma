#include "GeometryRepository.h"
#include "GeometryDataStoreMapper.h"
#include "TriangleList.h"
#include "GeometryDataEvents.h"
#include "GeometryDataPolicy.h"
#include "GeometryBuilder.h"
#include "GeometryErrors.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include "Frameworks/CommandBus.h"
#include "GeometryCommands.h"
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
    m_isCurrentBuilding = false;
    m_builder = menew GeometryBuilder(this);

    m_factory->registerGeometryFactory(TriangleList::TYPE_RTTI.getName(),
        [=](auto id) { return std::make_shared<TriangleList>(id); },
        [=](auto id, auto o) { return std::make_shared<TriangleList>(id, o); });
}

GeometryRepository::~GeometryRepository()
{
    if (m_factory) m_factory->unregisterGeometryFactory(TriangleList::TYPE_RTTI.getName());
    SAFE_DELETE(m_factory);
    SAFE_DELETE(m_builder);
}

ServiceResult GeometryRepository::onInit()
{
    //m_onGeometryBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnGeometryBuilt(e); });
    //EventPublisher::subscribe(typeid(GeometryDataBuilt), m_onGeometryBuilt);
    //m_onBuildGeometryFail = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildGeometryFail(e); });
    //EventPublisher::subscribe(typeid(BuildGeometryDataFailed), m_onBuildGeometryFail);

    //m_doBuildingGeometry = std::make_shared<CommandSubscriber>([=](auto c) { this->DoBuildingGeometry(c); });
    //CommandBus::subscribe(typeid(BuildGeometryData), m_doBuildingGeometry);

    m_queryGeometryData = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { return this->queryGeometryData(q); });
    QueryDispatcher::subscribe(typeid(QueryGeometryData), m_queryGeometryData);

    m_storeMapper->connect();

    return Frameworks::ServiceResult::Complete;
}

ServiceResult GeometryRepository::onTick()
{
    if (m_isCurrentBuilding) return Frameworks::ServiceResult::Pendding;
    std::lock_guard locker{ m_policiesLock };
    if (m_policies.empty())
    {
        m_needTick = false;
        return Frameworks::ServiceResult::Pendding;
    }
    assert(m_builder);
    m_builder->BuildGeometry(m_policies.front());
    m_policies.pop();
    m_isCurrentBuilding = true;
    return Frameworks::ServiceResult::Pendding;
}

ServiceResult GeometryRepository::onTerm()
{
    m_storeMapper->disconnect();
    m_geometries.clear();

    //EventPublisher::unsubscribe(typeid(GeometryDataBuilt), m_onGeometryBuilt);
    //m_onGeometryBuilt = nullptr;
    //EventPublisher::unsubscribe(typeid(BuildGeometryDataFailed), m_onBuildGeometryFail);
    //m_onBuildGeometryFail = nullptr;

    //CommandBus::unsubscribe(typeid(BuildGeometryData), m_doBuildingGeometry);
    //m_doBuildingGeometry = nullptr;

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

/*error GeometryRepository::BuildGeometry(const GeometryDataPolicy& policy)
{
    std::lock_guard locker{ m_policiesLock };
    m_policies.push(policy);
    m_needTick = true;
    return ErrorCode::ok;
}

void GeometryRepository::OnFactoryGeometryCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<FactoryGeometryCreated, IEvent>(e);
    if (!ev) return;
    std::lock_guard locker{ m_geometryLock };
    m_geometries.insert_or_assign(ev->GetDto().getName(), ev->GetGeometryData());
}

void GeometryRepository::OnGeometryBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<GeometryDataBuilt, IEvent>(e);
    if (!ev) return;
    //std::lock_guard locker{ m_geometryLock };
    //m_geometries.insert_or_assign(ev->getName(), ev->GetGeometryData());
    m_isCurrentBuilding = false;
}

void GeometryRepository::OnBuildGeometryFail(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<BuildGeometryDataFailed, IEvent>(e);
    if (!ev) return;
    Platforms::Debug::ErrorPrintf("geometry data %s build failed : %s\n",
        ev->id().name().c_str(), ev->error().message().c_str());
    m_isCurrentBuilding = false;
}

void GeometryRepository::DoBuildingGeometry(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<BuildGeometryData, ICommand>(c);
    if (!cmd) return;
    BuildGeometry(cmd->policy());
}*/

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
