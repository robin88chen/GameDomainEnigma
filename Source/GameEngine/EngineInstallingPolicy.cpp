#include "EngineInstallingPolicy.h"
#include "GenericDtoFactories.h"
#include "ShaderRepository.h"
#include "EffectMaterialManager.h"
#include "MaterialVariableMap.h"
#include "TextureRepository.h"
#include "RenderBufferRepository.h"
#include "TimerService.h"
#include "Platforms/MemoryAllocMacro.h"
#include "EngineErrors.h"
#include <cassert>

using namespace Enigma::Engine;

error EngineInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->registerSystemService(std::make_shared<GenericDtoFactories>(service_manager));
    service_manager->registerSystemService(std::make_shared<ShaderRepository>(service_manager));
    //service_manager->registerSystemService(std::make_shared<EffectMaterialManager>(service_manager));
    //menew MaterialVariableMap;
    service_manager->registerSystemService(std::make_shared<TextureRepository>(service_manager));
    service_manager->registerSystemService(std::make_shared<RenderBufferRepository>(service_manager));
    service_manager->registerSystemService(std::make_shared<TimerService>(service_manager));
    return ErrorCode::ok;
}

error EngineInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->shutdownSystemService(TimerService::TYPE_RTTI);
    service_manager->shutdownSystemService(RenderBufferRepository::TYPE_RTTI);
    service_manager->shutdownSystemService(TextureRepository::TYPE_RTTI);
    //delete MaterialVariableMap::instance();
    //service_manager->shutdownSystemService(EffectMaterialManager::TYPE_RTTI);
    service_manager->shutdownSystemService(ShaderRepository::TYPE_RTTI);
    service_manager->shutdownSystemService(GenericDtoFactories::TYPE_RTTI);
    return ErrorCode::ok;
}
