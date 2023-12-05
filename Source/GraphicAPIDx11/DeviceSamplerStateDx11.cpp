#include "DeviceSamplerStateDx11.h"
#include "GraphicAPIDx11.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "GraphicKernel/GraphicThread.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

DeviceSamplerStateDx11::DeviceSamplerStateDx11(const std::string& name) : IDeviceSamplerState(name)
{
    m_d3dSampler = nullptr;
}

DeviceSamplerStateDx11::~DeviceSamplerStateDx11()
{
    SAFE_RELEASE(m_d3dSampler);
}

error DeviceSamplerStateDx11::CreateFromData(const SamplerStateData& data)
{
    IDeviceSamplerState::CreateFromData(data);

    SAFE_RELEASE(m_d3dSampler);

    GraphicAPIDx11* api_dx11 = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::instance());
    assert(api_dx11);
    ID3D11Device* device = api_dx11->GetD3DDevice();
    if (FATAL_LOG_EXPR(!device)) return ErrorCode::d3dDeviceNullPointer;

    D3D11_SAMPLER_DESC desc;
    desc.Filter = FilterByData();
    desc.AddressU = AddressModeByData(m_data.m_addressModeU);
    desc.AddressV = AddressModeByData(m_data.m_addressModeV);
    desc.AddressW = AddressModeByData(m_data.m_addressModeW);
    desc.MipLODBias = m_data.m_mipmapLODBias;
    desc.MaxAnisotropy = m_data.m_levelAnisotropic;
    desc.ComparisonFunc = CompareFuncByData();
    desc.BorderColor[0] = m_data.m_borderColor[0];
    desc.BorderColor[1] = m_data.m_borderColor[1];
    desc.BorderColor[2] = m_data.m_borderColor[2];
    desc.BorderColor[3] = m_data.m_borderColor[3];
    desc.MinLOD = m_data.m_minLOD;
    desc.MaxLOD = m_data.m_maxLOD;

    HRESULT hr = device->CreateSamplerState(&desc, &m_d3dSampler);
    if (FATAL_LOG_EXPR(hr)) return ErrorCode::dxCreateSamplerState;

    Frameworks::EventPublisher::post(std::make_shared<Graphics::SamplerStateResourceCreated>(m_name));
    return ErrorCode::ok;
}

error DeviceSamplerStateDx11::BindToShader(Graphics::IShaderVariable::VarOwner var_of, unsigned bindPoint)
{
    GraphicAPIDx11* api_dx11 = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::instance());
    assert(api_dx11);
    ID3D11DeviceContext* deviceContext = api_dx11->GetD3DDeviceContext();
    if (FATAL_LOG_EXPR(!deviceContext)) return ErrorCode::d3dDeviceNullPointer;
    assert(deviceContext);

    ID3D11SamplerState* samplers[1] = { m_d3dSampler };
    if (var_of == Graphics::IShaderVariable::VarOwner::VarOfPixelShader)
    {
        deviceContext->PSSetSamplers(bindPoint, 1, samplers);
    }
    else if (var_of == Graphics::IShaderVariable::VarOwner::VarOfVertexShader)
    {
        deviceContext->VSSetSamplers(bindPoint, 1, samplers);
    }

    //todo : 展示面的事件需要嗎??

    return ErrorCode::ok;
}

future_error DeviceSamplerStateDx11::AsyncBindToShader(Graphics::IShaderVariable::VarOwner var_of, unsigned bindPoint)
{
    return Graphics::IGraphicAPI::instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), this, var_of, bindPoint]()->error
            {
                return BindToShader(var_of, bindPoint);
            });
}

D3D11_FILTER DeviceSamplerStateDx11::FilterByData()
{
    if (m_data.m_levelAnisotropic != 0) return D3D11_FILTER_ANISOTROPIC;
    unsigned char _min = 0, _mag = 0, _mip = 0;
    if (m_data.m_minFilter == SamplerStateData::Filter::Linear) _min = 0x10;
    if (m_data.m_magFilter == SamplerStateData::Filter::Linear) _mag = 0x04;
    if (m_data.m_mipFilter == SamplerStateData::Filter::Linear) _mip = 0x01;
    if (m_data.m_cmpFunc != SamplerStateData::CompareFunc::Never) return static_cast<D3D11_FILTER>(_min + _mag + _mip + 0x80);
    return static_cast<D3D11_FILTER>(_min + _mag + _mip);
}

D3D11_TEXTURE_ADDRESS_MODE DeviceSamplerStateDx11::AddressModeByData(SamplerStateData::AddressMode mode)
{
    if (mode == SamplerStateData::AddressMode::Clamp) return D3D11_TEXTURE_ADDRESS_CLAMP;
    if (mode == SamplerStateData::AddressMode::Wrap) return D3D11_TEXTURE_ADDRESS_WRAP;
    if (mode == SamplerStateData::AddressMode::Mirror) return D3D11_TEXTURE_ADDRESS_MIRROR;
    if (mode == SamplerStateData::AddressMode::Border) return D3D11_TEXTURE_ADDRESS_BORDER;
    return D3D11_TEXTURE_ADDRESS_CLAMP;
}

D3D11_COMPARISON_FUNC DeviceSamplerStateDx11::CompareFuncByData()
{
    return static_cast<D3D11_COMPARISON_FUNC>(m_data.m_cmpFunc);
}
