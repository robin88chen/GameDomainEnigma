#include "IGraphicAPI.h"
#include "GraphicThread.h"
#include "MathLib/ColorRGBA.h"
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

future_error IGraphicAPI::AsyncCreateDepthStencilSurface(const MathLib::Dimension& dimension,
    const GraphicFormat& fmt, IDepthStencilSurfacePtr* depth_surface)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->CreateDepthStencilSurface(dimension, fmt, depth_surface); });
}

future_error IGraphicAPI::AsyncShareDepthStencilSurface(const IDepthStencilSurfacePtr& from_depth,
    IDepthStencilSurfacePtr* depth_surface)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->ShareDepthStencilSurface(from_depth, depth_surface); });
}

future_error IGraphicAPI::AsyncClearSurface(const IBackSurfacePtr& back_surface,
    const IDepthStencilSurfacePtr& depth_surface, const MathLib::ColorRGBA& color, float depth_value,
    unsigned stencil_value)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->ClearSurface(back_surface, depth_surface, color, depth_value, stencil_value); });
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

