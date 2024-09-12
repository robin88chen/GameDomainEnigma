#include "LightInfo.h"
#include "LightAssemblers.h"

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

std::shared_ptr<LightInfoAssembler> LightInfo::assembler() const
{
    return std::make_shared<LightInfoAssembler>(m_type);
}

void LightInfo::assemble(const std::shared_ptr<LightInfoAssembler>& assembler) const
{
    assembler->color(m_color);
    if (m_type == LightType::Point)
    {
        assembler->position(m_position);
        assembler->range(m_range);
        assembler->attenuation(m_attenuation);
    }
    else if (m_type == LightType::Directional || m_type == LightType::SunLight)
    {
        assembler->direction(m_dir);
    }
    assembler->isEnable(m_isEnable);
}

std::shared_ptr<LightInfoDisassembler> LightInfo::disassembler() const
{
    return std::make_shared<LightInfoDisassembler>();
}

void LightInfo::disassemble(const std::shared_ptr<LightInfoDisassembler>& disassembler)
{
    m_type = disassembler->lightType();
    m_color = disassembler->color();
    m_position = disassembler->position();
    m_dir = disassembler->direction();
    m_range = disassembler->range();
    m_attenuation = disassembler->attenuation();
    m_isEnable = disassembler->isEnable();
}

void LightInfo::color(const MathLib::ColorRGBA& color)
{
    m_color = color;
}

void LightInfo::position(const MathLib::Vector3& vec)
{
    m_position = vec;
}

void LightInfo::direction(const MathLib::Vector3& vec)
{
    m_dir = vec.normalize();
}

void LightInfo::attenuation(const MathLib::Vector3& attenuation)
{
    m_attenuation = attenuation;
}

void LightInfo::range(float range)
{
    m_range = range;
}

void LightInfo::setEnable(bool flag)
{
    m_isEnable = flag;
}
