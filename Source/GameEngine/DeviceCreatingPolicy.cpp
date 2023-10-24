#include "DeviceCreatingPolicy.h"
#include "Frameworks/CommandBus.h"
#include "GraphicKernel/GraphicCommands.h"
#include "EngineErrors.h"

using namespace Enigma::Engine;

error DeviceCreatingPolicy::Install([[maybe_unused]] Frameworks::ServiceManager* service_manager)
{
    Frameworks::CommandBus::send(std::make_shared<Graphics::CreateDevice>(m_rqb, m_hwnd));
    return ErrorCode::ok;
}

error DeviceCreatingPolicy::Shutdown([[maybe_unused]] Frameworks::ServiceManager* service_manager)
{
    Frameworks::CommandBus::send(std::make_shared<Graphics::CleanupDevice>());
    return ErrorCode::ok;
}
