#include "EngineInstallingPolicy.h"
#include "GenericDtoFactories.h"
#include "GeometryRepository.h"
#include "ShaderRepository.h"
#include "EffectMaterialManager.h"
#include "MaterialVariableMap.h"
#include "TextureRepository.h"
#include "RenderBufferRepository.h"
#include "TimerService.h"
#include "Platforms/MemoryAllocMacro.h"
#include <cassert>

using namespace Enigma::Engine;

void EngineInstallingPolicy::Install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->RegisterSystemService(std::make_shared<GenericDtoFactories>(service_manager));
    service_manager->RegisterSystemService(std::make_shared<GeometryRepository>(service_manager));
    service_manager->RegisterSystemService(std::make_shared<ShaderRepository>(service_manager));
    service_manager->RegisterSystemService(std::make_shared<EffectMaterialManager>(service_manager));
    menew MaterialVariableMap;
    service_manager->RegisterSystemService(std::make_shared<TextureRepository>(service_manager));
    service_manager->RegisterSystemService(std::make_shared<RenderBufferRepository>(service_manager));
    service_manager->RegisterSystemService(std::make_shared<TimerService>(service_manager));
}

void EngineInstallingPolicy::Shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->ShutdownSystemService(TimerService::TYPE_RTTI);
    service_manager->ShutdownSystemService(RenderBufferRepository::TYPE_RTTI);
    service_manager->ShutdownSystemService(TextureRepository::TYPE_RTTI);
    delete MaterialVariableMap::Instance();
    service_manager->ShutdownSystemService(EffectMaterialManager::TYPE_RTTI);
    service_manager->ShutdownSystemService(ShaderRepository::TYPE_RTTI);
    service_manager->ShutdownSystemService(GeometryRepository::TYPE_RTTI);
    service_manager->ShutdownSystemService(GenericDtoFactories::TYPE_RTTI);
}
