#include "AsyncGraphicAPIDx11.h"
#include "GraphicAPIDx11.h"
#include "GraphicKernel/GraphicThread.h"
#include "GraphicKernel/GraphicErrors.h"

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

AsyncGraphicAPIDx11::AsyncGraphicAPIDx11() : IGraphicAPI()
{
    m_api = new GraphicAPIDx11();
    m_instance = this;
}

AsyncGraphicAPIDx11::~AsyncGraphicAPIDx11()
{
    delete m_api;
}

void AsyncGraphicAPIDx11::CreateDevice(const Graphics::DeviceRequiredBits& rqb, void* hwnd)
{
    m_workerThread->PushTask([=]() -> error { m_api->CreateDevice(rqb, hwnd); return ErrorCode::ok; });
}

void AsyncGraphicAPIDx11::CleanupDevice()
{
    m_workerThread->PushTask([=]() -> error { m_api->CleanupDevice(); return ErrorCode::ok; });
}

