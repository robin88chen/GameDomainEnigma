#include "DeviceDepthStencilStateEgl.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "GraphicKernel/GraphicThread.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

DeviceDepthStencilStateEgl::DeviceDepthStencilStateEgl(const std::string& name) : IDeviceDepthStencilState(name)
{
}

DeviceDepthStencilStateEgl::~DeviceDepthStencilStateEgl()
{
}

error DeviceDepthStencilStateEgl::CreateFromData(const DepthStencilData& data)
{
    IDeviceDepthStencilState::CreateFromData(data);
    m_depthFunc = ToCmpFunc(m_data.m_depthCompare);
    m_frontFaceStencilFunc = ToCmpFunc(m_data.m_frontFaceOp.m_compare);
    m_backFaceStencilFunc = ToCmpFunc(m_data.m_backFaceOp.m_compare);
    m_frontFaceFailOp = ToStencilOp(m_data.m_frontFaceOp.m_failOp);
    m_frontFaceZFailOp = ToStencilOp(m_data.m_frontFaceOp.m_depthFailOp);
    m_frontFacePassOp = ToStencilOp(m_data.m_frontFaceOp.m_passOp);
    m_backFaceFailOp = ToStencilOp(m_data.m_backFaceOp.m_failOp);
    m_backFaceZFailOp = ToStencilOp(m_data.m_backFaceOp.m_depthFailOp);
    m_backFacePassOp = ToStencilOp(m_data.m_backFaceOp.m_passOp);

    Frameworks::EventPublisher::post(std::make_shared<Graphics::DepthStencilStateResourceCreated>(m_name));
    return ErrorCode::ok;
}

error DeviceDepthStencilStateEgl::BindToDevice()
{
    if (m_data.m_isDepthTestEnable)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    if (m_data.m_isDepthWriteEnable)
    {
        glDepthMask(GL_TRUE);
    }
    else
    {
        glDepthMask(GL_FALSE);
    }
    if (m_data.m_isStencilTestEnable)
    {
        glEnable(GL_STENCIL_TEST);
    }
    else
    {
        glDisable(GL_STENCIL_TEST);
    }
    glDepthFunc(m_depthFunc);
    glStencilMask(m_data.m_stencilWriteMask);
    glStencilFuncSeparate(GL_FRONT, m_frontFaceStencilFunc, static_cast<GLint>(m_data.m_stencilRefValue), m_data.m_stencilReadMask);
    glStencilOpSeparate(GL_FRONT, m_frontFaceFailOp, m_frontFaceZFailOp, m_frontFacePassOp);
    glStencilFuncSeparate(GL_BACK, m_backFaceStencilFunc, static_cast<GLint>(m_data.m_stencilRefValue), m_data.m_stencilReadMask);
    glStencilOpSeparate(GL_BACK, m_backFaceFailOp, m_backFaceZFailOp, m_backFacePassOp);
    return ErrorCode::ok;
}

GLenum DeviceDepthStencilStateEgl::ToCmpFunc(DepthStencilData::CompareFunc f)
{
    switch (f)
    {
    case DepthStencilData::CompareFunc::Never:
        return GL_NEVER;
    case DepthStencilData::CompareFunc::Less:
        return GL_LESS;
    case DepthStencilData::CompareFunc::Equal:
        return GL_EQUAL;
    case DepthStencilData::CompareFunc::LessEqual:
        return GL_LEQUAL;
    case DepthStencilData::CompareFunc::Greater:
        return GL_GREATER;
    case DepthStencilData::CompareFunc::NotEqual:
        return GL_NOTEQUAL;
    case DepthStencilData::CompareFunc::GreaterEqual:
        return GL_GEQUAL;
    case DepthStencilData::CompareFunc::Always:
        return GL_ALWAYS;
    }
    return GL_NEVER;
}

GLenum DeviceDepthStencilStateEgl::ToStencilOp(DepthStencilData::StencilOpCode op)
{
    switch (op)
    {
    case DepthStencilData::StencilOpCode::Keep:
        return GL_KEEP;
    case DepthStencilData::StencilOpCode::Zero:
        return GL_ZERO;
    case DepthStencilData::StencilOpCode::Replace:
        return GL_REPLACE;
    case DepthStencilData::StencilOpCode::Incr_Sat:
        return GL_INCR_WRAP;
    case DepthStencilData::StencilOpCode::Decr_Sat:
        return GL_DECR_WRAP;
    case DepthStencilData::StencilOpCode::Invert:
        return GL_INVERT;
    case DepthStencilData::StencilOpCode::Incr:
        return GL_INCR;
    case DepthStencilData::StencilOpCode::Decr:
        return GL_DECR;
    }
    return GL_KEEP;
}
