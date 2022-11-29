#include "LightInfo.h"
#include "LightInfoDtos.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;

LightInfo::LightInfo()
{
    m_type = LightType::Unknown;
    m_range = 1.0f;
    m_isEnable = false;
}
LightInfo::LightInfo(LightType type)
{
    m_type = type;
    m_color = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
    m_dir = Vector3::UNIT_Z;
    m_position = Vector3::ZERO;
    m_range = 10.0f;
    m_attenuation = Vector3(0.0f, 1.0f, 0.0f);
    m_isEnable = true;
}

LightInfo::LightInfo(const LightInfoDto& dto) : LightInfo(dto.LightType())
{
    m_type = dto.LightType();
    m_color = dto.Color();
    m_position = dto.Position();
    m_dir = dto.Direction();
    m_attenuation = dto.Attenuation();
    m_range = dto.Range();
    m_isEnable = dto.IsEnable();
}

LightInfoDto LightInfo::SerializeDto()
{
    LightInfoDto dto;
    dto.LightType() = m_type;
    dto.Color() = m_color;
    dto.Position() = m_position;
    dto.Direction() = m_dir;
    dto.Attenuation() = m_attenuation;
    dto.Range() = m_range;
    dto.IsEnable() = m_isEnable;
    return dto;
}

void LightInfo::SetLightColor(const MathLib::ColorRGBA& color)
{
    m_color = color;
}

void LightInfo::SetLightPosition(const MathLib::Vector3& vec)
{
    m_position = vec;
}

void LightInfo::SetLightDirection(const MathLib::Vector3& vec)
{
    m_dir = vec;
}

void LightInfo::SetLightAttenuation(const MathLib::Vector3& attenuation)
{
    m_attenuation = attenuation;
}

void LightInfo::SetLightRange(float range)
{
    m_range = range;
}

void LightInfo::SetEnable(bool flag)
{
    m_isEnable = flag;
}
