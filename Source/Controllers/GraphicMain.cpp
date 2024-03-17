#include "GraphicMain.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/QueryDispatcher.h"
#include "Platforms/MemoryMacro.h"
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

GraphicMain* GraphicMain::instance()
{
    assert(m_instance);
    return m_instance;
}

error GraphicMain::installFrameworks()
{
    assert(m_serviceManager);
    m_serviceManager->registerSystemService(std::make_shared<Frameworks::EventPublisher>(m_serviceManager));
    m_serviceManager->registerSystemService(std::make_shared <Frameworks::CommandBus>(m_serviceManager));
    m_serviceManager->registerSystemService(std::make_shared<Frameworks::QueryDispatcher>(m_serviceManager));

    Frameworks::EventPublisher::post(std::make_shared<FrameworksInstalled>());

    return ErrorCode::ok;
}

error GraphicMain::shutdownFrameworks()
{
    assert(m_serviceManager);
    m_serviceManager->shutdownSystemService(Frameworks::EventPublisher::TYPE_RTTI);
    m_serviceManager->shutdownSystemService(Frameworks::CommandBus::TYPE_RTTI);
    m_serviceManager->shutdownSystemService(Frameworks::QueryDispatcher::TYPE_RTTI);
    return ErrorCode::ok;
}

error GraphicMain::installRenderEngine(const InstallingPolicyList& policies)
{
    m_policies = policies;
    for (auto it = m_policies.begin(); it != m_policies.end(); ++it)
    {
        if (*it == nullptr) continue;
        error er = (*it)->install(m_serviceManager);
        if (er) return er;
    }
    m_serviceManager->runToState(Frameworks::ServiceManager::ServiceState::WaitingToRun); // make sure all services initialized
    Frameworks::EventPublisher::post(std::make_shared<RenderEngineInstalled>());
    return ErrorCode::ok;
}

error GraphicMain::shutdownRenderEngine()
{
    for (auto it = m_policies.rbegin(); it != m_policies.rend(); ++it)
    {
        if (*it == nullptr) continue;
        error er = (*it)->shutdown(m_serviceManager);
        if (er) return er;
    }
    return ErrorCode::ok;
}


void GraphicMain::frameUpdate()
{
    if (m_serviceManager)
    {
        m_serviceManager->runOnce();
    }
}

