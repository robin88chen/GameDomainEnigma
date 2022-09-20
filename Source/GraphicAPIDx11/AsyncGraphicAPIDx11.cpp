#include "AsyncGraphicAPIDx11.h"
#include "GraphicAPIDx11.h"
#include "GraphicKernel/GraphicThread.h"
#include "GraphicKernel/GraphicErrors.h"

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

AsyncGraphicAPIDx11::AsyncGraphicAPIDx11() : GraphicAPIDx11()
{
}

AsyncGraphicAPIDx11::~AsyncGraphicAPIDx11()
{
}

void AsyncGraphicAPIDx11::CreateDevice(const Graphics::DeviceRequiredBits& rqb, void* hwnd)
{
    m_workerThread->PushTask([=]() -> error { GraphicAPIDx11::CreateDevice(rqb, hwnd); return ErrorCode::ok; });
}

void AsyncGraphicAPIDx11::CleanupDevice()
{
    m_workerThread->PushTask([=]() -> error { GraphicAPIDx11::CleanupDevice(); return ErrorCode::ok; });
}

void AsyncGraphicAPIDx11::BeginScene()
{
    m_workerThread->PushTask([=]() -> error { GraphicAPIDx11::BeginScene(); return ErrorCode::ok; });
}

void AsyncGraphicAPIDx11::EndScene()
{
    m_workerThread->PushTask([=]() -> error { GraphicAPIDx11::EndScene(); return ErrorCode::ok; });
}
void AsyncGraphicAPIDx11::CreatePrimaryBackSurface(const std::string& back_name,
    const std::string& depth_name)
{
    m_workerThread->PushTask([=]() -> error { GraphicAPIDx11::CreatePrimaryBackSurface(back_name, depth_name); return ErrorCode::ok; });
}

void AsyncGraphicAPIDx11::Clear(const Graphics::IBackSurfacePtr& back_surface,
    const Graphics::IDepthStencilSurfacePtr& depth_surface, const MathLib::ColorRGBA& color, float depth_value,
    unsigned stencil_value)
{
    m_workerThread->PushTask([=]() -> error { GraphicAPIDx11::Clear(back_surface, depth_surface, color, depth_value, stencil_value); return ErrorCode::ok; });
}

void AsyncGraphicAPIDx11::Flip()
{
    m_workerThread->PushTask([=]() -> error { GraphicAPIDx11::Flip(); return ErrorCode::ok; });
}