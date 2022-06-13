#include "GraphicAPIDx11.h"
#include "SwapChainDx11.h"
#include "AdapterDx11.h"
#include "GraphicErrors.h"
#include "MemoryAllocMacro.h"
#include "MemoryMacro.h"

using namespace Enigma::Graphics;

GraphicAPIDx11::GraphicAPIDx11() : IGraphicAPI()
{
    m_apiVersion = IGraphicAPI::APIVersion::API_Dx11;
    m_wnd = nullptr;
    m_creator = nullptr;
    m_swapChain = nullptr;
    m_adapter = nullptr;
}

GraphicAPIDx11::~GraphicAPIDx11()
{
    SAFE_DELETE(m_creator);
    SAFE_DELETE(m_swapChain);
    SAFE_DELETE(m_adapter);
}

error GraphicAPIDx11::CreateDevice(const DeviceRequiredBits& rqb, AsyncType use_async, void* hwnd)
{
    return ErrorCode::ok;
}

error GraphicAPIDx11::CleanupDevice()
{
    return ErrorCode::ok;
}
