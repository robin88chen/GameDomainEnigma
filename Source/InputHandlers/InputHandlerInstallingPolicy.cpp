#include "InputHandlerInstallingPolicy.h"
#include "InputHandlerService.h"
#include <cassert>

using namespace Enigma::InputHandlers;

error InputHandlerInstallingPolicy::Install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    m_inputHandler = std::make_shared<InputHandlerService>(service_manager);
    service_manager->RegisterSystemService(m_inputHandler);
    return std::error_code();
}

error InputHandlerInstallingPolicy::Shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->ShutdownSystemService(InputHandlerService::TYPE_RTTI);
    m_inputHandler = nullptr;
    return std::error_code();
}

