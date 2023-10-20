#include "PrefabInstallingPolicy.h"
#include "PrefabErrors.h"
#include "PrefabIOService.h"
#include <cassert>

using namespace Enigma::Prefabs;

error PrefabInstallingPolicy::Install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->RegisterSystemService(std::make_shared<PrefabIOService>(service_manager, m_dtoDeserializer));
    return ErrorCode::ok;
}

error PrefabInstallingPolicy::Shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->ShutdownSystemService(PrefabIOService::TYPE_RTTI);
    return ErrorCode::ok;
}
