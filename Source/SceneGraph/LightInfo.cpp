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

LightInfo::LightInfo(const Engine::GenericDto& o) : LightInfo(LightInfoDto::fromGenericDto(o).LightType())
{
    LightInfoDto dto = LightInfoDto::fromGenericDto(o);
    m_type = dto.LightType();
    m_color = dto.Color();
    m_position = dto.Position();
    m_dir = dto.direction();
    m_attenuation = dto.Attenuation();
    m_range = dto.Range();
    m_isEnable = dto.isEnable();
}

LightInfoDto LightInfo::serializeDto()
{
    LightInfoDto dto;
    dto.LightType() = m_type;
    dto.Color() = m_color;
    dto.Position() = m_position;
    dto.direction() = m_dir;
    dto.Attenuation() = m_attenuation;
    dto.Range() = m_range;
    dto.isEnable() = m_isEnable;
    return dto;
}

void LightInfo::setLightColor(const MathLib::ColorRGBA& color)
{
    m_color = color;
}

void LightInfo::setLightPosition(const MathLib::Vector3& vec)
{
    m_position = vec;
}

void LightInfo::setLightDirection(const MathLib::Vector3& vec)
{
    m_dir = vec.normalize();
}

void LightInfo::setLightAttenuation(const MathLib::Vector3& attenuation)
{
    m_attenuation = attenuation;
}

void LightInfo::setLightRange(float range)
{
    m_range = range;
}

void LightInfo::setEnable(bool flag)
{
    m_isEnable = flag;
}
