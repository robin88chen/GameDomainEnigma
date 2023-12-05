#include "DeviceConstBufferDx11.h"
#include "GraphicAPIDx11.h"
#include "GraphicKernel/IDeviceConstBuffer.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "GraphicKernel/GraphicThread.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

DeviceConstBufferDx11::DeviceConstBufferDx11(const std::string& name) : IDeviceConstBuffer(name)
{
    m_d3dBuffer = nullptr;
}

DeviceConstBufferDx11::~DeviceConstBufferDx11()
{
    SAFE_RELEASE(m_d3dBuffer);
}

error DeviceConstBufferDx11::CreateDataBuffer(unsigned cb_size)
{
    IDeviceConstBuffer::CreateDataBuffer(cb_size);

    GraphicAPIDx11* api_dx11 = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::instance());
    assert(api_dx11);
    ID3D11Device* device = api_dx11->GetD3DDevice();
    if (FATAL_LOG_EXPR(!device)) return ErrorCode::d3dDeviceNullPointer;

    D3D11_BUFFER_DESC desc;
    desc.ByteWidth = cb_size;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    // Create the buffer.
    m_d3dBuffer = nullptr;
    HRESULT hr = device->CreateBuffer(&desc, nullptr, &m_d3dBuffer);
    if (FATAL_LOG_EXPR(hr)) return ErrorCode::dxCreateBuffer;

    Frameworks::EventPublisher::post(std::make_shared<Graphics::ConstBufferResourceCreated>(m_name));
    return ErrorCode::ok;
}

error DeviceConstBufferDx11::Apply(const byte_buffer& data, unsigned dataSize)
{
    //Platforms::Debug::Printf("apply const buffer in thread %d\n", std::this_thread::get_id());
    GraphicAPIDx11* api_dx11 = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::instance());
    assert(api_dx11);
    ID3D11DeviceContext* deviceContext = api_dx11->GetD3DDeviceContext();
    if (FATAL_LOG_EXPR(!deviceContext)) return ErrorCode::d3dDeviceNullPointer;
    assert(deviceContext);

    D3D11_MAPPED_SUBRESOURCE sub;
    HRESULT hr = deviceContext->Map(m_d3dBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub);
    if (FATAL_LOG_EXPR(FAILED(hr))) return ErrorCode::dxBufferMapping;
    memcpy_s(sub.pData, dataSize, &data[0], dataSize);
    deviceContext->Unmap(m_d3dBuffer, 0);

    //todo : 這個沒有改變物件狀態，只是展示面的事件需要嗎??
    //Frameworks::EventPublisher::post(Frameworks::IEventPtr{ menew Graphics::ConstBufferResourceApplied(m_name) });
    return ErrorCode::ok;
}

error DeviceConstBufferDx11::Apply()
{
    return Apply(m_data, m_size);
}

future_error DeviceConstBufferDx11::AsyncApply()
{
    //! lambda 從 this 抓 member data 並不是 by value, 要複製; c++17 可以用 *this; 因為這個物件不能複製, 所以 *this 沒法使用, 還是複製成員, 做物件複製更不划算
    return Graphics::IGraphicAPI::instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), data = this->m_data, size = this->m_size, this]() -> error
        {
            return Apply(data, size);
        }); // 資料 copy 到 functor 裡
    //return Graphics::IGraphicAPI::instance()->GetGraphicThread()->
      //  PushTask([=, *this]() -> error { return Apply(m_data, m_size); }); // 資料 copy 到 functor 裡
}

error DeviceConstBufferDx11::BindToShader(Graphics::IShaderVariable::VarOwner var_of, unsigned bindPoint)
{
    GraphicAPIDx11* api_dx11 = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::instance());
    assert(api_dx11);
    ID3D11DeviceContext* deviceContext = api_dx11->GetD3DDeviceContext();
    if (FATAL_LOG_EXPR(!deviceContext)) return ErrorCode::d3dDeviceNullPointer;
    assert(deviceContext);

    ID3D11Buffer* buffers[1] = { m_d3dBuffer };
    if (var_of == Graphics::IShaderVariable::VarOwner::VarOfPixelShader)
    {
        deviceContext->PSSetConstantBuffers(bindPoint, 1, buffers);
    }
    else if (var_of == Graphics::IShaderVariable::VarOwner::VarOfVertexShader)
    {
        deviceContext->VSSetConstantBuffers(bindPoint, 1, buffers);
    }

    //todo : 展示面的事件需要嗎??

    return ErrorCode::ok;
}

future_error DeviceConstBufferDx11::AsyncBindToShader(Graphics::IShaderVariable::VarOwner var_of, unsigned bindPoint)
{
    return Graphics::IGraphicAPI::instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), this, var_of, bindPoint]()->error
        {
            return BindToShader(var_of, bindPoint);
        });
}

