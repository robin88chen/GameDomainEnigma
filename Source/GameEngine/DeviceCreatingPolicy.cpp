#include "DeviceCreatingPolicy.h"
#include "Frameworks/CommandBus.h"
#include "GraphicKernel/GraphicCommands.h"

using namespace Enigma::Engine;

void DeviceCreatingPolicy::Install([[maybe_unused]] Frameworks::ServiceManager* service_manager)
{
    Frameworks::CommandBus::Send(std::make_shared<Graphics::CreateDevice>(m_rqb, m_hwnd));
}

void DeviceCreatingPolicy::Shutdown([[maybe_unused]] Frameworks::ServiceManager* service_manager)
{
    Frameworks::CommandBus::Send(std::make_shared<Graphics::CleanupDevice>());
}
