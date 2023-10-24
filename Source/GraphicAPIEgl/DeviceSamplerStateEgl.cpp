#include "DeviceSamplerStateEgl.h"
#include "GraphicAPIEgl.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "GraphicKernel/GraphicThread.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

DeviceSamplerStateEgl::DeviceSamplerStateEgl(const std::string& name) : IDeviceSamplerState(name)
{
    m_sampler = 0;
}

DeviceSamplerStateEgl::~DeviceSamplerStateEgl()
{
    if (m_sampler != 0)
    {
        glDeleteSamplers(1, &m_sampler);
    }
}

error DeviceSamplerStateEgl::CreateFromData(const SamplerStateData& data)
{
    if (m_sampler != 0)
    {
        glDeleteSamplers(1, &m_sampler);
        m_sampler = 0;
    }
    glGenSamplers(1, &m_sampler);

    IDeviceSamplerState::CreateFromData(data);

    glSamplerParameteri(m_sampler, GL_TEXTURE_WRAP_S, AddressModeByData(m_data.m_addressModeU));
    glSamplerParameteri(m_sampler, GL_TEXTURE_WRAP_T, AddressModeByData(m_data.m_addressModeV));
    glSamplerParameteri(m_sampler, GL_TEXTURE_WRAP_R, AddressModeByData(m_data.m_addressModeW));
    glSamplerParameteri(m_sampler, GL_TEXTURE_MIN_FILTER, MinFilterByData(m_data.m_minFilter, m_data.m_mipFilter));
    glSamplerParameteri(m_sampler, GL_TEXTURE_MAG_FILTER, MagFilterByData(m_data.m_magFilter));
    glSamplerParameterf(m_sampler, GL_TEXTURE_MIN_LOD, m_data.m_minLOD);
    glSamplerParameterf(m_sampler, GL_TEXTURE_MAX_LOD, m_data.m_maxLOD);
    //! android 的 gles3 沒有 border color (即使是 texture parameter也沒有)
    //glSamplerParameterfv(m_sampler, GL_TEXTURE_BORDER_COLOR, (float*)m_data.m_borderColor);
    //glSamplerParameterf(m_sampler, GL_TEXTURE_LOD_BIAS, m_data.m_maxLOD);
    Frameworks::EventPublisher::post(std::make_shared<Graphics::SamplerStateResourceCreated>(m_name));
    return ErrorCode::ok;
}

error DeviceSamplerStateEgl::BindToShader(unsigned bindPoint)
{
    glBindSampler(bindPoint, m_sampler);
    return ErrorCode::ok;
}

future_error DeviceSamplerStateEgl::AsyncBindToShader(unsigned bindPoint)
{
    return Graphics::IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), bindPoint, this]()->error
            {
                return BindToShader(bindPoint);
            });
}

GLint DeviceSamplerStateEgl::AddressModeByData(SamplerStateData::AddressMode mode)
{
    if (mode == SamplerStateData::AddressMode::Clamp) return GL_CLAMP_TO_EDGE;
    if (mode == SamplerStateData::AddressMode::Mirror) return GL_MIRRORED_REPEAT;
    if (mode == SamplerStateData::AddressMode::Wrap) return GL_REPEAT;
    return GL_CLAMP_TO_EDGE;
}

GLint DeviceSamplerStateEgl::MinFilterByData(SamplerStateData::Filter min_fil, SamplerStateData::Filter mip_fil)
{
    if ((min_fil == SamplerStateData::Filter::Point) &&
        (mip_fil == SamplerStateData::Filter::None)) return GL_NEAREST;
    if ((min_fil == SamplerStateData::Filter::Linear) &&
        (mip_fil == SamplerStateData::Filter::None)) return GL_LINEAR;
    if ((min_fil == SamplerStateData::Filter::Point) &&
        (mip_fil == SamplerStateData::Filter::Point)) return GL_NEAREST_MIPMAP_NEAREST;
    if ((min_fil == SamplerStateData::Filter::Linear) &&
        (mip_fil == SamplerStateData::Filter::Point)) return GL_LINEAR_MIPMAP_NEAREST;
    if ((min_fil == SamplerStateData::Filter::Point) &&
        (mip_fil == SamplerStateData::Filter::Linear)) return GL_NEAREST_MIPMAP_LINEAR;
    if ((min_fil == SamplerStateData::Filter::Linear) &&
        (mip_fil == SamplerStateData::Filter::Linear)) return GL_LINEAR_MIPMAP_LINEAR;
    return GL_LINEAR_MIPMAP_LINEAR;
}

GLint DeviceSamplerStateEgl::MagFilterByData(SamplerStateData::Filter fil)
{
    if (fil == SamplerStateData::Filter::Point) return GL_NEAREST;
    if (fil == SamplerStateData::Filter::Linear) return GL_LINEAR;
    return GL_LINEAR;
}
