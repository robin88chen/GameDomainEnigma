#include "LightAssemblers.h"
#include "SpatialAssembler.h"
#include "SceneGraphQueries.h"
#include "Light.h"

using namespace Enigma::SceneGraph;

LightAssembler::LightAssembler(const SpatialId& id, const LightInfo::LightType& type) : m_factory(Light::TYPE_RTTI), m_spatialAssembler(id)
{
    m_id = id;
    m_info.lightType(type);
}

SpatialAssembler& LightAssembler::spatial()
{
    return m_spatialAssembler;
}

LightAssembler& LightAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_factory = factory;
    m_spatialAssembler.factory(factory);
    return *this;
}

LightAssembler& LightAssembler::asNative(const std::string& file_at_path)
{
    m_factory.ClaimAsNative(file_at_path);
    m_spatialAssembler.factory(m_factory);
    return *this;
}

LightAssembler& LightAssembler::color(const MathLib::ColorRGBA& color)
{
    m_info.color(color);
    return *this;
}

LightAssembler& LightAssembler::position(const MathLib::Vector3& position)
{
    assert(m_info.lightType() == LightInfo::LightType::Point);
    m_info.position(position);
    return *this;
}

LightAssembler& LightAssembler::direction(const MathLib::Vector3& direction)
{
    assert((m_info.lightType() == LightInfo::LightType::Directional) || (m_info.lightType() == LightInfo::LightType::SunLight));
    m_info.direction(direction);
    return *this;
}

LightAssembler& LightAssembler::range(float range)
{
    assert(m_info.lightType() == LightInfo::LightType::Point);
    m_info.range(range);
    return *this;
}

LightAssembler& LightAssembler::attenuation(const MathLib::Vector3& attenuation)
{
    assert(m_info.lightType() == LightInfo::LightType::Point);
    m_info.attenuation(attenuation);
    return *this;
}

LightAssembler& LightAssembler::isEnable(bool is_enable)
{
    m_info.isEnable(is_enable);
    return *this;
}

LightDto LightAssembler::toLightDto() const
{
    LightDto dto(m_spatialAssembler.toGenericDto());
    dto.lightInfo(m_info.toGenericDto());
    return dto;
}

Enigma::Engine::GenericDto LightAssembler::toGenericDto() const
{
    return toLightDto().toGenericDto();
}

std::shared_ptr<Light> LightAssembler::constitute()
{
    return std::dynamic_pointer_cast<Light>(std::make_shared<RequestSpatialConstitution>(m_id, toGenericDto())->dispatch());
}
