#include "EngineInstallingPolicy.h"
#include "ShaderRepository.h"
#include "TextureRepository.h"
#include "RenderBufferRepository.h"
#include "TimerService.h"
#include "EngineErrors.h"
#include <cassert>

using namespace Enigma::Engine;

error EngineInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->registerSystemService(std::make_shared<ShaderRepository>(service_manager));
    //service_manager->registerSystemService(std::make_shared<TextureRepository>(service_manager));
    service_manager->registerSystemService(std::make_shared<RenderBufferRepository>(service_manager));
    service_manager->registerSystemService(std::make_shared<TimerService>(service_manager));
    return ErrorCode::ok;
}

error EngineInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->shutdownSystemService(TimerService::TYPE_RTTI);
    service_manager->shutdownSystemService(RenderBufferRepository::TYPE_RTTI);
    //service_manager->shutdownSystemService(TextureRepository::TYPE_RTTI);
    service_manager->shutdownSystemService(ShaderRepository::TYPE_RTTI);
    return ErrorCode::ok;
}
