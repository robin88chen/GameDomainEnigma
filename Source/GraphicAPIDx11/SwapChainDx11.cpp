#include "SwapChainDx11.h"
#include <memory>
#include <cassert>
#include "MemoryMacro.h"
#include "GraphicErrors.h"

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

SwapChainDx11::SwapChainDx11(void)
{
    ZeroMemory(&m_currentDesc, sizeof(m_currentDesc));
    m_dxgiSwapChain = 0;
}

SwapChainDx11::~SwapChainDx11(void)
{
    ToggleFullscreen(false);
    SAFE_RELEASE(m_dxgiSwapChain);
}

void SwapChainDx11::ToggleFullscreen(bool full_screen)
{
    BOOL flag = full_screen ? TRUE : FALSE;
    if (m_dxgiSwapChain) m_dxgiSwapChain->SetFullscreenState(flag, NULL);
}

error SwapChainDx11::Present()
{
    if (m_dxgiSwapChain) m_dxgiSwapChain->Present(0, 0); //DXGI_PRESENT_DO_NOT_SEQUENCE);
    return ErrorCode::ok;
}

error SwapChainDx11::ResizeBackSurfaces(unsigned int width, unsigned int height)
{
    assert(m_dxgiSwapChain);
    HRESULT hr = m_dxgiSwapChain->ResizeBuffers(m_currentDesc.BufferCount, width, height,
        m_currentDesc.BufferDesc.Format, m_currentDesc.Flags);
    if (FAILED(hr))
    {
        return ErrorCode::resizeBackSurfaceFail;
    }
    return ErrorCode::ok;
}