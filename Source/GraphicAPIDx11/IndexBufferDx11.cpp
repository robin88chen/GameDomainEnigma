#include "IndexBufferDx11.h"
#include "GraphicAPIDx11.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

IndexBufferDx11::IndexBufferDx11(const std::string& name) : IIndexBuffer(name)
{
    m_d3dBuffer = nullptr;
    m_bufferSize = 0;
}

IndexBufferDx11::~IndexBufferDx11()
{
    SAFE_RELEASE(m_d3dBuffer);
}

error IndexBufferDx11::Create(unsigned sizeBuffer)
{
    assert(Graphics::IGraphicAPI::Instance()->IsValidGraphicThread(std::this_thread::get_id()));
    m_bufferSize = sizeBuffer;
    assert(m_bufferSize > 0);
    GraphicAPIDx11* graphic = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::Instance());
    assert(graphic);
    if (FATAL_LOG_EXPR(!graphic->GetD3DDevice())) return ErrorCode::d3dDeviceNullPointer;

    D3D11_BUFFER_DESC bd;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0;
    bd.ByteWidth = m_bufferSize;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.MiscFlags = 0;
    bd.StructureByteStride = 0;

    HRESULT hr = graphic->GetD3DDevice()->CreateBuffer(&bd, NULL, &m_d3dBuffer);
    if (FATAL_LOG_EXPR(FAILED(hr))) return ErrorCode::deviceCreateIndexBuffer;

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::IndexBufferResourceCreated>(m_name));
    return ErrorCode::ok;
}

error IndexBufferDx11::Update(const uint_buffer& dataIndex)
{
    assert(Graphics::IGraphicAPI::Instance()->IsValidGraphicThread(std::this_thread::get_id()));
    assert(!dataIndex.empty());
    unsigned int dataSize = (unsigned int)dataIndex.size() * sizeof(unsigned int);
    if (FATAL_LOG_EXPR(dataSize > m_bufferSize)) return ErrorCode::bufferSize;

    GraphicAPIDx11* graphic = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::Instance());
    assert(graphic);
    if (FATAL_LOG_EXPR(!graphic->GetD3DDevice())) return ErrorCode::d3dDeviceNullPointer;
    D3D11_BOX d3dBox = { 0, 0, 0, dataSize, 1, 1 };
    graphic->GetD3DDeviceContext()->UpdateSubresource(m_d3dBuffer, 0, &d3dBox, &dataIndex[0], 0, 0);

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::IndexBufferResourceUpdated>(m_name));
    return ErrorCode::ok;
}

error IndexBufferDx11::RangedUpdate(const ranged_buffer& buffer)
{
    assert(Graphics::IGraphicAPI::Instance()->IsValidGraphicThread(std::this_thread::get_id()));
    assert(!buffer.data.empty());
    unsigned int dataSize = (unsigned int)buffer.data.size() * sizeof(unsigned int);
    if (FATAL_LOG_EXPR(dataSize > m_bufferSize)) return ErrorCode::bufferSize;

    GraphicAPIDx11* graphic = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::Instance());
    assert(graphic);
    if (FATAL_LOG_EXPR(!graphic->GetD3DDevice())) return ErrorCode::d3dDeviceNullPointer;

    unsigned int byte_offset = buffer.idx_offset * sizeof(unsigned int);
    unsigned int byte_length = buffer.idx_count * sizeof(unsigned int);
    if (byte_length > (unsigned int)buffer.data.size() * sizeof(unsigned int))
    {
        byte_length = (unsigned int)buffer.data.size() * sizeof(unsigned int);
    }

    D3D11_BOX d3dBox = { byte_offset, 0, 0, byte_offset + byte_length, 1, 1 };
    graphic->GetD3DDeviceContext()->UpdateSubresource(m_d3dBuffer, 0, &d3dBox, &(buffer.data[0]), 0, 0);

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::IndexBufferResourceRangedUpdated>(
        m_name, buffer.idx_offset, buffer.idx_count));
    return ErrorCode::ok;
}
