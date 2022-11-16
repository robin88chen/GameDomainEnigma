#include "GeometryRepository.h"
#include "TriangleList.h"
#include "GeometryDataContract.h"
#include "GeometryDataEvents.h"
#include "GeometryDataPolicy.h"
#include "GeometryBuilder.h"
#include "EngineErrors.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"

using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

DEFINE_RTTI(Engine, GeometryRepository, ISystemService);

GeometryRepository::GeometryRepository(Frameworks::ServiceManager* srv_manager) : ISystemService(srv_manager)
{
    m_needTick = false;
    m_isCurrentBuilding = false;
    m_builder = menew GeometryBuilder(this);
}

GeometryRepository::~GeometryRepository()
{
    SAFE_DELETE(m_builder);
}

ServiceResult GeometryRepository::OnInit()
{

    return Frameworks::ServiceResult::Complete;
}

ServiceResult GeometryRepository::OnTick()
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

ServiceResult GeometryRepository::OnTerm()
{
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

std::shared_ptr<GeometryData> GeometryRepository::Create(const Contract& contract)
{
    if (contract.GetRtti().GetRttiName() == TriangleList::TYPE_RTTI.GetName())
    {
        return CreateTriangleList(TriangleListContract::FromContract(contract));
    }
    return nullptr;
}

std::shared_ptr<GeometryData> GeometryRepository::CreateTriangleList(const TriangleListContract& contract)
{
    if (HasGeometryData(contract.Name())) return QueryGeometryData(contract.Name());
    std::shared_ptr<GeometryData> geometry = std::make_shared<TriangleList>(contract);
    std::lock_guard locker{ m_geometryLock };
    m_geometries.insert_or_assign(contract.Name(), geometry);
    return geometry;
}
