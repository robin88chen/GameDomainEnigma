#include "EffectMaterialSourceRepositoryInstallingPolicy.h"
#include "EffectMaterialSourceRepository.h"
#include "EngineErrors.h"
#include "MaterialVariableMap.h"
#include "Platforms/MemoryAllocMacro.h"
#include <cassert>

using namespace Enigma::Engine;

error EffectMaterialSourceRepositoryInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->registerSystemService(std::make_shared<EffectMaterialSourceRepository>(service_manager, m_storeMapper));
    menew MaterialVariableMap;
    return ErrorCode::ok;
}

error EffectMaterialSourceRepositoryInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    delete MaterialVariableMap::instance();
    service_manager->shutdownSystemService(EffectMaterialSourceRepository::TYPE_RTTI);
    return ErrorCode::ok;
}
