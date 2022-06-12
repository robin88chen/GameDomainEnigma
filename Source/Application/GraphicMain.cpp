#include "GraphicMain.h"
#include "EventPublisher.h"
#include "CommandBus.h"
#include "MemoryAllocMacro.h"
#include "ApplicationErrors.h"
#include "ApplicationEvents.h"
#include <cassert>

using namespace Enigma::Application;
GraphicMain* GraphicMain::m_instance = nullptr;

GraphicMain::GraphicMain(AsyncType useAsyncDevice, GraphicCoordSys coordSys)
{
    assert(!m_instance);
#if TARGET_PLATFORM == PLATFORM_ANDROID
    m_asyncType = AsyncType::NotAsyncDevice;  // android always use sync device
#else
    m_asyncType = useAsyncDevice;
#endif
    m_coordSys = coordSys;
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
    m_serviceManager->RegisterSystemService(menew Frameworks::EventPublisher(m_serviceManager));
    m_serviceManager->RegisterSystemService(menew Frameworks::CommandBus(m_serviceManager));

    Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew FrameworksInstalled });

    return ErrorCode::ok;
}

error GraphicMain::ShutdownFrameworks()
{
    assert(m_serviceManager);
    m_serviceManager->ShutdownSystemService(Frameworks::EventPublisher::TYPE_RTTI);
    m_serviceManager->ShutdownSystemService(Frameworks::CommandBus::TYPE_RTTI);
    return ErrorCode::ok;
}

void GraphicMain::FrameUpdate()
{
    if (m_serviceManager)
    {
        m_serviceManager->RunOnce();
    }
}
