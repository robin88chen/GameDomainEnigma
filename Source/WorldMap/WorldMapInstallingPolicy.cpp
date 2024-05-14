#include "Frameworks/ServiceManager.h"
#include "WorldMapErrors.h"
#include "WorldMapInstallingPolicy.h"
#include "WorldMapRepository.h"
#include <cassert>

using namespace Enigma::WorldMap;

error WorldMapInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->registerSystemService(std::make_shared<WorldMapRepository>(service_manager, m_storeMapper));
    return ErrorCode::ok;
}

error WorldMapInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->shutdownSystemService(WorldMapRepository::TYPE_RTTI);
    return ErrorCode::ok;
}