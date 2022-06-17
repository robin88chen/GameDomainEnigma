#include "IGraphicAPI.h"
#include "GraphicThread.h"
#include <cassert>

using namespace Enigma::Graphics;

IGraphicAPI* IGraphicAPI::m_instance = nullptr;

IGraphicAPI::IGraphicAPI()
{
    assert(!m_instance);
    m_instance = this;
    m_workerThread = new GraphicThread{};
}

IGraphicAPI::~IGraphicAPI()
{
    m_instance = nullptr;
    if (m_workerThread) delete m_workerThread;
}

IGraphicAPI* IGraphicAPI::Instance()
{
    assert(m_instance);
    return m_instance;
}

future_error IGraphicAPI::AsyncCreateDevice(const DeviceRequiredBits& rqb, void* hwnd)
{
    return m_workerThread->PushTask(std::bind(&IGraphicAPI::CreateDevice, this, rqb, hwnd));
}

future_error IGraphicAPI::AsyncCleanupDevice()
{
    return m_workerThread->PushTask(std::bind(&IGraphicAPI::CleanupDevice, this));
}

future_error IGraphicAPI::AsyncGetPrimaryBackSurface(IBackSurfacePtr* back_surface, IDepthStencilSurfacePtr* depth_surface)
{
    return m_workerThread->PushTask(std::bind(&IGraphicAPI::GetPrimaryBackSurface, this,
        back_surface, depth_surface));
}

future_error IGraphicAPI::AsyncCreateBackSurface(const MathLib::Dimension& dimension, const GraphicFormat& fmt, IBackSurfacePtr* back_surface)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->CreateBackSurface(dimension, fmt, back_surface); });
}

void IGraphicAPI::TerminateGraphicThread()
{
    if (m_workerThread)
    {
        m_workerThread->Terminate();
    }
}

GraphicThread* IGraphicAPI::GetGraphicThread()
{
    assert(m_workerThread);
    return m_workerThread;
}

