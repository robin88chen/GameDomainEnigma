#include "GeometryRepository.h"
#include "TriangleList.h"
#include "GeometryDataEvents.h"
#include "GeometryDataPolicy.h"
#include "GeometryBuilder.h"
#include "EngineErrors.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include "Frameworks/CommandBus.h"
#include "GeometryCommands.h"

using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

DEFINE_RTTI(Engine, GeometryRepository, ISystemService);

GeometryRepository::GeometryRepository(Frameworks::ServiceManager* srv_manager) : ISystemService(srv_manager)
{
    m_needTick = false;
    m_isCurrentBuilding = false;
    m_builder = menew GeometryBuilder(this);
    CommandBus::Post(std::make_shared<RegisterGeometryDtoFactory>(TriangleList::TYPE_RTTI.getName(),
        [=](auto o) { return std::make_shared<TriangleList>(o); }));
}

GeometryRepository::~GeometryRepository()
{
    CommandBus::Post(std::make_shared<UnRegisterGeometryDtoFactory>(TriangleList::TYPE_RTTI.getName()));
    SAFE_DELETE(m_builder);
}

ServiceResult GeometryRepository::onInit()
{
    m_onGeometryBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnGeometryBuilt(e); });
    EventPublisher::Subscribe(typeid(GeometryDataBuilt), m_onGeometryBuilt);
    m_onBuildGeometryFail = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildGeometryFail(e); });
    EventPublisher::Subscribe(typeid(BuildGeometryDataFailed), m_onBuildGeometryFail);

    m_doBuildingGeometry = std::make_shared<CommandSubscriber>([=](auto c) { this->DoBuildingGeometry(c); });
    CommandBus::Subscribe(typeid(BuildGeometryData), m_doBuildingGeometry);

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
    EventPublisher::Unsubscribe(typeid(GeometryDataBuilt), m_onGeometryBuilt);
    m_onGeometryBuilt = nullptr;
    EventPublisher::Unsubscribe(typeid(BuildGeometryDataFailed), m_onBuildGeometryFail);
    m_onBuildGeometryFail = nullptr;

    CommandBus::Unsubscribe(typeid(BuildGeometryData), m_doBuildingGeometry);
    m_doBuildingGeometry = nullptr;

    return Frameworks::ServiceResult::Complete;
}

bool GeometryRepository::HasGeometryData(const std::string& name)
{
    std::lock_guard locker{ m_geometryLock };
    auto it = m_geometries.find(name);
    return ((it != m_geometries.end()) && (!it->second.expired()));
}

std::shared_ptr<GeometryData> GeometryRepository::QueryGeometryData(const std::string& name)
{
    std::lock_guard locker{ m_geometryLock };
    auto it = m_geometries.find(name);
    if (it == m_geometries.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

error GeometryRepository::BuildGeometry(const GeometryDataPolicy& policy)
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
        ev->getName().c_str(), ev->GetErrorCode().message().c_str());
    m_isCurrentBuilding = false;
}

void GeometryRepository::DoBuildingGeometry(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<BuildGeometryData, ICommand>(c);
    if (!cmd) return;
    BuildGeometry(cmd->GetPolicy());
}
