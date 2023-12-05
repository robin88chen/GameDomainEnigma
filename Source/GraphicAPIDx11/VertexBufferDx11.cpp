#include "VertexBufferDx11.h"
#include "GraphicAPIDx11.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

VertexBufferDx11::VertexBufferDx11(const std::string& name) : IVertexBuffer(name)
{
    m_sizeofVertex = 0;
    m_bufferSize = 0;
    m_d3dBuffer = nullptr;
}

VertexBufferDx11::~VertexBufferDx11()
{
    SAFE_RELEASE(m_d3dBuffer);
}

error VertexBufferDx11::create(unsigned sizeofVertex, unsigned sizeBuffer)
{
    assert(Graphics::IGraphicAPI::instance()->IsValidGraphicThread(std::this_thread::get_id()));
    m_sizeofVertex = sizeofVertex;
    m_bufferSize = sizeBuffer;
    assert(m_bufferSize > 0);

    GraphicAPIDx11* graphic = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::instance());
    assert(graphic);
    if (FATAL_LOG_EXPR(!graphic->GetD3DDevice())) return ErrorCode::d3dDeviceNullPointer;

    D3D11_BUFFER_DESC bd;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0;
    bd.ByteWidth = m_bufferSize;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.MiscFlags = 0;
    bd.StructureByteStride = 0;
    HRESULT hr = graphic->GetD3DDevice()->CreateBuffer(&bd, NULL, &m_d3dBuffer);
    if (FATAL_LOG_EXPR(FAILED(hr))) return ErrorCode::deviceCreateVertexBuffer;

    Frameworks::EventPublisher::post(std::make_shared<Graphics::VertexBufferResourceCreated>(m_name));
    return ErrorCode::ok;
}

error VertexBufferDx11::UpdateBuffer(const byte_buffer& dataVertex)
{
    assert(Graphics::IGraphicAPI::instance()->IsValidGraphicThread(std::this_thread::get_id()));
    assert(!dataVertex.empty());
    if (FATAL_LOG_EXPR(dataVertex.size() > m_bufferSize))
    {
        Frameworks::EventPublisher::post(std::make_shared<Graphics::VertexBufferUpdateFailed>(m_name, ErrorCode::bufferSize));
        return ErrorCode::bufferSize;
    }

    GraphicAPIDx11* graphic = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::instance());
    assert(graphic);
    if (FATAL_LOG_EXPR(!graphic->GetD3DDevice()))
    {
        Frameworks::EventPublisher::post(std::make_shared<Graphics::VertexBufferUpdateFailed>(m_name, ErrorCode::d3dDeviceNullPointer));
        return ErrorCode::d3dDeviceNullPointer;
    }

    D3D11_BOX d3dBox = { 0, 0, 0, static_cast<unsigned int>(dataVertex.size()), 1, 1 };
    graphic->GetD3DDeviceContext()->UpdateSubresource(m_d3dBuffer, 0, &d3dBox, &dataVertex[0], 0, 0);

    Frameworks::EventPublisher::post(std::make_shared<Graphics::VertexBufferResourceUpdated>(m_name));
    return ErrorCode::ok;
}

error VertexBufferDx11::RangedUpdateBuffer(const ranged_buffer& buffer)
{
    assert(Graphics::IGraphicAPI::instance()->IsValidGraphicThread(std::this_thread::get_id()));
    assert(!buffer.data.empty());
    if (FATAL_LOG_EXPR(buffer.data.size() > m_bufferSize))
    {
        Frameworks::EventPublisher::post(std::make_shared<Graphics::VertexBufferUpdateFailed>(m_name, ErrorCode::bufferSize));
        return ErrorCode::bufferSize;
    }
    GraphicAPIDx11* graphic = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::instance());
    assert(graphic);
    if (FATAL_LOG_EXPR(!graphic->GetD3DDevice()))
    {
        Frameworks::EventPublisher::post(std::make_shared<Graphics::VertexBufferUpdateFailed>(m_name, ErrorCode::d3dDeviceNullPointer));
        return ErrorCode::d3dDeviceNullPointer;
    }

    unsigned int byte_offset = buffer.vtx_offset * m_sizeofVertex;
    unsigned int byte_length = buffer.vtx_count * m_sizeofVertex;
    if (byte_length > static_cast<unsigned int>(buffer.data.size())) byte_length = static_cast<unsigned int>(buffer.data.size());

    D3D11_BOX d3dBox = { byte_offset, 0, 0, byte_length + byte_offset, 1, 1 };
    graphic->GetD3DDeviceContext()->UpdateSubresource(m_d3dBuffer, 0, &d3dBox, &(buffer.data[0]), 0, 0);

    Frameworks::EventPublisher::post(std::make_shared<Graphics::VertexBufferResourceRangedUpdated>(
        m_name, buffer.vtx_offset, buffer.vtx_count));
    return ErrorCode::ok;
}
