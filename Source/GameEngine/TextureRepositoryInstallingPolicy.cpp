#include "TextureRepositoryInstallingPolicy.h"
#include "TextureRepository.h"
#include "EngineErrors.h"
#include <cassert>

using namespace Enigma::Engine;

error TextureRepositoryInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->registerSystemService(std::make_shared<TextureRepository>(service_manager, m_storeMapper));
    return ErrorCode::ok;
}

error TextureRepositoryInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->shutdownSystemService(TextureRepository::TYPE_RTTI);
    return ErrorCode::ok;
}
