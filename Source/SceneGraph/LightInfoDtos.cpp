#include "LightInfoDtos.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;

static std::string TOKEN_LIGHT_TYPE = "LightType";
static std::string TOKEN_LIGHT_COLOR = "LightColor";
static std::string TOKEN_LIGHT_POSITION = "LightPosition";
static std::string TOKEN_LIGHT_DIRECTION = "LightDirection";
static std::string TOKEN_LIGHT_RANGE = "LightRange";
static std::string TOKEN_LIGHT_ATTENUATION = "LightAttenuation";
static std::string TOKEN_LIGHT_ENABLE = "LightIsEnable";

LightInfoDto::LightInfoDto()
{
    m_type = LightInfo::LightType::Unknown;
    m_isEnable = false;
}

LightInfoDto LightInfoDto::FromGenericDto(const GenericDto& dto)
{
    LightInfoDto light_dto;
    light_dto.LightType() = static_cast<LightInfo::LightType>(dto.TryGetValue<unsigned int>(TOKEN_LIGHT_TYPE).value());
    light_dto.Color() = dto.TryGetValue<MathLib::ColorRGBA>(TOKEN_LIGHT_COLOR).value();
    light_dto.Position() = dto.TryGetValue<MathLib::Vector3>(TOKEN_LIGHT_POSITION).value();
    light_dto.Direction() = dto.TryGetValue<MathLib::Vector3>(TOKEN_LIGHT_DIRECTION).value();
    light_dto.Range() = dto.TryGetValue<float>(TOKEN_LIGHT_RANGE).value();
    light_dto.Attenuation() = dto.TryGetValue<MathLib::Vector3>(TOKEN_LIGHT_ATTENUATION).value();
    light_dto.IsEnable() = dto.TryGetValue<bool>(TOKEN_LIGHT_ENABLE).value();
    return light_dto;
}

GenericDto LightInfoDto::ToGenericDto()
{
    GenericDto dto;
    dto.AddOrUpdate(TOKEN_LIGHT_TYPE, static_cast<unsigned int>(m_type));
    dto.AddOrUpdate(TOKEN_LIGHT_COLOR, m_color);
    dto.AddOrUpdate(TOKEN_LIGHT_POSITION, m_position);
    dto.AddOrUpdate(TOKEN_LIGHT_DIRECTION, m_direction);
    dto.AddOrUpdate(TOKEN_LIGHT_RANGE, m_range);
    dto.AddOrUpdate(TOKEN_LIGHT_ATTENUATION, m_attenuation);
    dto.AddOrUpdate(TOKEN_LIGHT_ENABLE, m_isEnable);
    return dto;
}
