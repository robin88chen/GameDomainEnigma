#include "GeometryRepository.h"

using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

DEFINE_RTTI(Engine, GeometryRepository, ISystemService);

GeometryRepository::GeometryRepository(Frameworks::ServiceManager* srv_manager) : ISystemService(srv_manager)
{
    m_needTick = false;
}

GeometryRepository::~GeometryRepository()
{
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
