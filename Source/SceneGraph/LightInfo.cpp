#include "LightInfo.h"
#include "LightInfo.h"
#include "LightInfoContracts.h"

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

LightInfo::LightInfo(const LightInfoContract& contract) : LightInfo(contract.LightType())
{
    m_type = contract.LightType();
    m_color = contract.Color();
    m_position = contract.Position();
    m_dir = contract.Direction();
    m_attenuation = contract.Attenuation();
    m_range = contract.Range();
    m_isEnable = contract.IsEnable();
}

LightInfoContract LightInfo::SerializeContract()
{
    LightInfoContract contract;
    contract.LightType() = m_type;
    contract.Color() = m_color;
    contract.Position() = m_position;
    contract.Direction() = m_dir;
    contract.Attenuation() = m_attenuation;
    contract.Range() = m_range;
    contract.IsEnable() = m_isEnable;
    return contract;
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
