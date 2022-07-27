#include "DeviceRasterizerStateEgl.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "GraphicKernel/GraphicThread.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

DeviceRasterizerStateEgl::DeviceRasterizerStateEgl(const std::string& name) : IDeviceRasterizerState(name)
{
}

DeviceRasterizerStateEgl::~DeviceRasterizerStateEgl()
{
}

error DeviceRasterizerStateEgl::CreateFromData(const RasterizerStateData& data)
{
    IDeviceRasterizerState::CreateFromData(data);
    m_isCullEnable = IsCullEnableByData(m_data.m_cullMode);
    m_cullMode = GL_BACK;
    m_frontFaceWise = FrontFaceWiseByData(m_data.m_cullMode);
    m_depthBias = PixeledDepthBiasByData(m_data.m_depthBias);
    m_depthSlopeFactor = m_data.m_slopeScaledDepthBias;

    Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew Graphics::RasterizerStateResourceCreated(m_name) });
    return ErrorCode::ok;
}

error DeviceRasterizerStateEgl::BindToDevice()
{
    if (m_isCullEnable)
    {
        glEnable(GL_CULL_FACE);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
    glCullFace(m_cullMode);
    glFrontFace(m_frontFaceWise);
    glPolygonOffset(m_depthSlopeFactor, m_depthBias);
    // gles no support for wireframe
    return ErrorCode::ok;
}

bool DeviceRasterizerStateEgl::IsCullEnableByData(RasterizerStateData::BackfaceCullMode mode)
{
    if (mode == RasterizerStateData::BackfaceCullMode::Cull_None)
    {
        return false;
    }
    return true;
}

GLenum DeviceRasterizerStateEgl::FrontFaceWiseByData(RasterizerStateData::BackfaceCullMode mode)
{
    switch (mode)
    {
    case RasterizerStateData::BackfaceCullMode::Cull_CCW:
        return GL_CW;
        break;
    case RasterizerStateData::BackfaceCullMode::Cull_CW:
        return GL_CCW;
        break;
    default:
        break;
    }
    return GL_CW;
}

float DeviceRasterizerStateEgl::PixeledDepthBiasByData(float bias) const
{
    int bits = Graphics::IGraphicAPI::Instance()->GetDepthSurfaceFormat().DepthBits();
    if ((bits == 16) || (bits == 24)) // D16UNORM, D24UNORM
    {
        return (bias * (float)pow(2, bits));
    }
    else if (bits == 32) // D32F
    {
        return (bias / (float)pow(2, exp(1.0f) - 23.0f));
    }
    return 0.0f;
}

