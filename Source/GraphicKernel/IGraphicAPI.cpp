#include "IGraphicAPI.h"
#include "GraphicThread.h"
#include "GraphicAssetRepository.h"
#include "MathLib/ColorRGBA.h"
#include <cassert>

using namespace Enigma::Graphics;

IGraphicAPI* IGraphicAPI::m_instance = nullptr;

IGraphicAPI::IGraphicAPI()
{
    assert(!m_instance);
    m_instance = this;
    m_workerThread = new GraphicThread{};
    m_repository = new AssetRepository{};
}

IGraphicAPI::~IGraphicAPI()
{
    m_instance = nullptr;
    if (m_workerThread) delete m_workerThread;
    if (m_repository) delete m_repository;
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

future_error IGraphicAPI::AsyncFlip()
{
    return m_workerThread->PushTask([=]() -> error { return this->Flip(); });
}

future_error IGraphicAPI::AsyncCreatePrimaryBackSurface(const std::string& back_name, const std::string& depth_name)
{
    return m_workerThread->PushTask(std::bind(&IGraphicAPI::CreatePrimaryBackSurface, this,
        back_name, depth_name));
}

future_error IGraphicAPI::AsyncCreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension, 
    const GraphicFormat& fmt)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->CreateBackSurface(back_name, dimension, fmt); });
}

future_error IGraphicAPI::AsyncCreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension, 
    unsigned buff_count, const std::vector<GraphicFormat>& fmts)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->CreateBackSurface(back_name, dimension, buff_count, fmts); });
}

future_error IGraphicAPI::AsyncCreateDepthStencilSurface(const std::string& depth_name, const MathLib::Dimension& dimension,
    const GraphicFormat& fmt)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->CreateDepthStencilSurface(depth_name, dimension, fmt); });
}

future_error IGraphicAPI::AsyncShareDepthStencilSurface(const std::string& depth_name, 
    const IDepthStencilSurfacePtr& from_depth)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->ShareDepthStencilSurface(depth_name, from_depth); });
}

future_error IGraphicAPI::AsyncClearSurface(const IBackSurfacePtr& back_surface,
    const IDepthStencilSurfacePtr& depth_surface, const MathLib::ColorRGBA& color, float depth_value,
    unsigned stencil_value)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->ClearSurface(back_surface, depth_surface, color, depth_value, stencil_value); });
}

future_error IGraphicAPI::AsyncBindBackSurface(const IBackSurfacePtr& back_surface,
    const IDepthStencilSurfacePtr& depth_surface)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->BindBackSurface(back_surface, depth_surface); });
}

future_error IGraphicAPI::AsyncBindViewPort(const TargetViewPort& vp)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->BindViewPort(vp); });
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

