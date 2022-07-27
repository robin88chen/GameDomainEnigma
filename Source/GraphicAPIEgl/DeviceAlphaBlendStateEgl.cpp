#include "DeviceAlphaBlendStateEgl.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "GraphicKernel/GraphicThread.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

DeviceAlphaBlendStateEgl::DeviceAlphaBlendStateEgl(const std::string& name) : IDeviceAlphaBlendState(name)
{
}

DeviceAlphaBlendStateEgl::~DeviceAlphaBlendStateEgl()
{
}

error DeviceAlphaBlendStateEgl::CreateFromData(const BlendStateData& data)
{
    IDeviceAlphaBlendState::CreateFromData(data);
    m_isBlendEnable = true;
    switch (m_data.m_blendType[0])
    {
    case BlendStateData::BlendType::Blend_Disable:
    {
        m_isBlendEnable = false;
    }
    break;
    case BlendStateData::BlendType::Blend_Opaque:
    {
        m_srcFactor = GL_ONE;
        m_dstFactor = GL_ZERO;
        m_blendOp = GL_FUNC_ADD;
    }
    break;
    case BlendStateData::BlendType::Blend_Additive:
    {
        m_srcFactor = GL_ONE;
        m_dstFactor = GL_ONE;
        m_blendOp = GL_FUNC_ADD;
    }
    break;
    case BlendStateData::BlendType::Blend_Transparent:
    {
        m_srcFactor = GL_SRC_ALPHA;
        m_dstFactor = GL_ONE_MINUS_SRC_ALPHA;
        m_blendOp = GL_FUNC_ADD;
    }
    break;
    default:
        break;
    }

    Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew Graphics::AlphaBlendStateResourceCreated(m_name) });
    return ErrorCode::ok;
}

error DeviceAlphaBlendStateEgl::BindToDevice()
{
    if (m_isBlendEnable)
    {
        glEnable(GL_BLEND);
        glBlendFunc(m_srcFactor, m_dstFactor);
        glBlendEquation(m_blendOp);
    }
    else
    {
        glDisable(GL_BLEND);
    }
    return ErrorCode::ok;
}
