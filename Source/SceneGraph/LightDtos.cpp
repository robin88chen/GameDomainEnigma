#include "LightDtos.h"
#include "Light.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;

static std::string TOKEN_LIGHT_TYPE = "LightType";
static std::string TOKEN_LIGHT_COLOR = "LightColor";
static std::string TOKEN_LIGHT_POSITION = "LightPosition";
static std::string TOKEN_LIGHT_DIRECTION = "LightDirection";
static std::string TOKEN_LIGHT_RANGE = "LightRange";
static std::string TOKEN_LIGHT_ATTENUATION = "LightAttenuation";
static std::string TOKEN_LIGHT_ENABLE = "LightIsEnable";
static std::string TOKEN_LIGHT_INFO = "LightInfo";

LightInfoDto::LightInfoDto()
{
    m_type = LightInfo::LightType::Unknown;
    m_range = 0.0f;
    m_isEnable = false;
}

LightInfoDto LightInfoDto::fromGenericDto(const GenericDto& dto)
{
    LightInfoDto light_dto;
    if (auto v = dto.tryGetValue<unsigned>(TOKEN_LIGHT_TYPE)) light_dto.lightType(static_cast<LightInfo::LightType>(v.value()));
    if (auto v = dto.tryGetValue<MathLib::ColorRGBA>(TOKEN_LIGHT_COLOR)) light_dto.color(v.value());
    if (auto v = dto.tryGetValue<MathLib::Vector3>(TOKEN_LIGHT_POSITION)) light_dto.position(v.value());
    if (auto v = dto.tryGetValue<MathLib::Vector3>(TOKEN_LIGHT_DIRECTION)) light_dto.direction(v.value());
    if (auto v = dto.tryGetValue<float>(TOKEN_LIGHT_RANGE)) light_dto.range(v.value());
    if (auto v = dto.tryGetValue<MathLib::Vector3>(TOKEN_LIGHT_ATTENUATION)) light_dto.attenuation(v.value());
    if (auto v = dto.tryGetValue<bool>(TOKEN_LIGHT_ENABLE)) light_dto.isEnable(v.value());
    return light_dto;
}

GenericDto LightInfoDto::toGenericDto() const
{
    GenericDto dto;
    dto.addOrUpdate(TOKEN_LIGHT_TYPE, static_cast<unsigned int>(m_type));
    dto.addOrUpdate(TOKEN_LIGHT_COLOR, m_color);
    dto.addOrUpdate(TOKEN_LIGHT_POSITION, m_position);
    dto.addOrUpdate(TOKEN_LIGHT_DIRECTION, m_direction);
    dto.addOrUpdate(TOKEN_LIGHT_RANGE, m_range);
    dto.addOrUpdate(TOKEN_LIGHT_ATTENUATION, m_attenuation);
    dto.addOrUpdate(TOKEN_LIGHT_ENABLE, m_isEnable);
    return dto;
}

LightDto::LightDto() : SpatialDto()
{
    m_factoryDesc = FactoryDesc(Light::TYPE_RTTI.getName());
}

LightDto::LightDto(const SpatialDto& spatial_dto) : SpatialDto(spatial_dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), Light::TYPE_RTTI.getName()));
}

LightDto::LightDto(const Engine::GenericDto& dto) : SpatialDto(dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), Light::TYPE_RTTI.getName()));
    if (auto v = dto.tryGetValue<GenericDto>(TOKEN_LIGHT_INFO)) m_lightInfo = v.value();
}

GenericDto LightDto::toGenericDto() const
{
    GenericDto dto = SpatialDto::toGenericDto();
    dto.addOrUpdate(TOKEN_LIGHT_INFO, m_lightInfo);

    return dto;
}
