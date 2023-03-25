#include "GraphicMain.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/RequestBus.h"
#include "Frameworks/ResponseBus.h"
#include "Platforms/MemoryAllocMacro.h"
#include "ControllerErrors.h"
#include "ControllerEvents.h"
#include <cassert>
#include <memory>

using namespace Enigma::Controllers;
GraphicMain* GraphicMain::m_instance = nullptr;

GraphicMain::GraphicMain()
{
    assert(!m_instance);
    m_instance = this;
    m_serviceManager = menew Frameworks::ServiceManager();
}

GraphicMain::~GraphicMain()
{
    m_instance = nullptr;
    delete m_serviceManager;
}

GraphicMain* GraphicMain::Instance()
{
    assert(m_instance);
    return m_instance;
}

error GraphicMain::InstallFrameworks()
{
    assert(m_serviceManager);
    m_serviceManager->RegisterSystemService(std::make_shared<Frameworks::EventPublisher>(m_serviceManager));
    m_serviceManager->RegisterSystemService(std::make_shared <Frameworks::CommandBus>(m_serviceManager));
    m_serviceManager->RegisterSystemService(std::make_shared<Frameworks::RequestBus>(m_serviceManager));
    m_serviceManager->RegisterSystemService(std::make_shared<Frameworks::ResponseBus>(m_serviceManager));

    Frameworks::EventPublisher::Post(std::make_shared<FrameworksInstalled>());

    return ErrorCode::ok;
}

error GraphicMain::ShutdownFrameworks()
{
    assert(m_serviceManager);
    m_serviceManager->ShutdownSystemService(Frameworks::EventPublisher::TYPE_RTTI);
    m_serviceManager->ShutdownSystemService(Frameworks::CommandBus::TYPE_RTTI);
    m_serviceManager->ShutdownSystemService(Frameworks::RequestBus::TYPE_RTTI);
    m_serviceManager->ShutdownSystemService(Frameworks::ResponseBus::TYPE_RTTI);
    return ErrorCode::ok;
}

error GraphicMain::InstallRenderEngine(const InstallingPolicyList& policies)
{
    m_policies = policies;
    for (auto it = m_policies.begin(); it != m_policies.end(); ++it)
    {
        if (*it == nullptr) continue;
        error er = (*it)->Install(m_serviceManager);
        if (er) return er;
    }
    return ErrorCode::ok;
}

error GraphicMain::ShutdownRenderEngine()
{
    for (auto it = m_policies.rbegin(); it != m_policies.rend(); ++it)
    {
        if (*it == nullptr) continue;
        error er = (*it)->Shutdown(m_serviceManager);
        if (er) return er;
    }
    return ErrorCode::ok;
}


void GraphicMain::FrameUpdate()
{
    if (m_serviceManager)
    {
        m_serviceManager->RunOnce();
    }
}

