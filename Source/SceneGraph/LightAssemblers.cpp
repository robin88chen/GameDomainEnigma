#include "LightAssemblers.h"
#include "SpatialAssembler.h"
#include "SceneGraphQueries.h"
#include "Light.h"

using namespace Enigma::SceneGraph;

static std::string TOKEN_LIGHT_TYPE = "LightType";
static std::string TOKEN_LIGHT_COLOR = "LightColor";
static std::string TOKEN_LIGHT_POSITION = "LightPosition";
static std::string TOKEN_LIGHT_DIRECTION = "LightDirection";
static std::string TOKEN_LIGHT_RANGE = "LightRange";
static std::string TOKEN_LIGHT_ATTENUATION = "LightAttenuation";
static std::string TOKEN_LIGHT_ENABLE = "LightIsEnable";
static std::string TOKEN_LIGHT_INFO = "LightInfo";

LightInfoAssembler::LightInfoAssembler(const LightInfo::LightType& type) : m_type(type)
{
}

void LightInfoAssembler::position(const MathLib::Vector3& position)
{
    assert(m_type == LightInfo::LightType::Point);
    m_position = position;
}

void LightInfoAssembler::direction(const MathLib::Vector3& direction)
{
    assert((m_type == LightInfo::LightType::Directional) || (m_type == LightInfo::LightType::SunLight));
    m_direction = direction;
}

void LightInfoAssembler::range(float range)
{
    assert(m_type == LightInfo::LightType::Point);
    m_range = range;
}

void LightInfoAssembler::attenuation(const MathLib::Vector3& attenuation)
{
    assert(m_type == LightInfo::LightType::Point);
    m_attenuation = attenuation;
}

Enigma::Engine::GenericDto LightInfoAssembler::assemble() const
{
    Engine::GenericDto dto;
    dto.addOrUpdate(TOKEN_LIGHT_TYPE, static_cast<unsigned int>(m_type));
    dto.addOrUpdate(TOKEN_LIGHT_COLOR, m_color);
    dto.addOrUpdate(TOKEN_LIGHT_POSITION, m_position);
    dto.addOrUpdate(TOKEN_LIGHT_DIRECTION, m_direction);
    dto.addOrUpdate(TOKEN_LIGHT_RANGE, m_range);
    dto.addOrUpdate(TOKEN_LIGHT_ATTENUATION, m_attenuation);
    dto.addOrUpdate(TOKEN_LIGHT_ENABLE, m_isEnable);
    return dto;
}

LightInfoDisassembler::LightInfoDisassembler()
{
    m_type = LightInfo::LightType::Unknown;
    m_range = 0.0f;
    m_isEnable = false;
}

void LightInfoDisassembler::disassemble(const Engine::GenericDto& dto)
{
    if (auto v = dto.tryGetValue<unsigned>(TOKEN_LIGHT_TYPE)) m_type = static_cast<LightInfo::LightType>(v.value());
    if (auto v = dto.tryGetValue<MathLib::ColorRGBA>(TOKEN_LIGHT_COLOR)) m_color = v.value();
    if (auto v = dto.tryGetValue<MathLib::Vector3>(TOKEN_LIGHT_POSITION)) m_position = v.value();
    if (auto v = dto.tryGetValue<MathLib::Vector3>(TOKEN_LIGHT_DIRECTION)) m_direction = v.value();
    if (auto v = dto.tryGetValue<float>(TOKEN_LIGHT_RANGE)) m_range = v.value();
    if (auto v = dto.tryGetValue<MathLib::Vector3>(TOKEN_LIGHT_ATTENUATION)) m_attenuation = v.value();
    if (auto v = dto.tryGetValue<bool>(TOKEN_LIGHT_ENABLE)) m_isEnable = v.value();
}

LightAssembler::LightAssembler(const SpatialId& id, const LightInfo::LightType& type) : SpatialAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(Light::TYPE_RTTI.getName());
    m_infoAssembler = std::make_shared<LightInfoAssembler>(type);
}

void LightAssembler::color(const MathLib::ColorRGBA& color)
{
    assert(m_infoAssembler);
    m_infoAssembler->color(color);
}

void LightAssembler::position(const MathLib::Vector3& position)
{
    assert(m_infoAssembler);
    m_infoAssembler->position(position);
}

void LightAssembler::direction(const MathLib::Vector3& direction)
{
    assert(m_infoAssembler);
    m_infoAssembler->direction(direction);
}

void LightAssembler::range(float range)
{
    assert(m_infoAssembler);
    m_infoAssembler->range(range);
}

void LightAssembler::attenuation(const MathLib::Vector3& attenuation)
{
    assert(m_infoAssembler);
    m_infoAssembler->attenuation(attenuation);
}

void LightAssembler::isEnable(bool is_enable)
{
    assert(m_infoAssembler);
    m_infoAssembler->isEnable(is_enable);
}

void LightAssembler::lightInfo(const LightInfo& info)
{
    assert(m_infoAssembler);
    assert(info.lightType() == m_infoAssembler->lightType());
    info.assemble(m_infoAssembler);
}

void LightAssembler::lightInfo(const std::shared_ptr<LightInfoAssembler>& info_assembler)
{
    assert(info_assembler);
    assert(m_infoAssembler);
    assert(info_assembler->lightType() == m_infoAssembler->lightType());
    m_infoAssembler = info_assembler;
}

Enigma::Engine::GenericDto LightAssembler::assemble() const
{
    Engine::GenericDto dto = SpatialAssembler::assemble();
    dto.addOrUpdate(TOKEN_LIGHT_INFO, m_infoAssembler->assemble());
    return dto;
}

LightDisassembler::LightDisassembler() : SpatialDisassembler()
{
    m_factoryDesc = Engine::FactoryDesc(Light::TYPE_RTTI.getName());
}

void LightDisassembler::disassemble(const Engine::GenericDto& dto)
{
    SpatialDisassembler::disassemble(dto);
    if (auto v = dto.tryGetValue<Engine::GenericDto>(TOKEN_LIGHT_INFO))
    {
        std::shared_ptr<LightInfoDisassembler> disassembler = std::make_shared<LightInfoDisassembler>();
        disassembler->disassemble(v.value());
        m_info.disassemble(disassembler);
    }
}
