#include "DeviceCreatingPolicy.h"
#include "Frameworks/CommandBus.h"
#include "GraphicKernel/GraphicCommands.h"
#include "EngineErrors.h"

using namespace Enigma::Engine;

error DeviceCreatingPolicy::install([[maybe_unused]] Frameworks::ServiceManager* service_manager)
{
    Frameworks::CommandBus::execute(std::make_shared<Graphics::CreateDevice>(m_rqb, m_hwnd));
    return ErrorCode::ok;
}

error DeviceCreatingPolicy::shutdown([[maybe_unused]] Frameworks::ServiceManager* service_manager)
{
    Frameworks::CommandBus::execute(std::make_shared<Graphics::CleanupDevice>());
    return ErrorCode::ok;
}
