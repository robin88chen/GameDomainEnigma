#include "GeometryInstallingPolicy.h"
#include "GeometryRepository.h"
#include "GeometryErrors.h"

using namespace Enigma::Geometries;

error GeometryInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->registerSystemService(std::make_shared<GeometryRepository>(service_manager, m_storeMapper));
    return ErrorCode::ok;
}

error GeometryInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->shutdownSystemService(GeometryRepository::TYPE_RTTI);
    return ErrorCode::ok;
}

