#include "LightInfo.h"
#include "LightInfoPolicies.h"

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

LightInfo::LightInfo(const LightInfoPolicy& policy) : LightInfo(policy.m_type)
{
    m_color = policy.m_color;
    if (policy.m_position) m_position = policy.m_position.value();
    if (policy.m_direction) m_dir = policy.m_direction.value();
    if (policy.m_attenuation) m_attenuation = policy.m_attenuation.value();
    if (policy.m_range) m_range = policy.m_range.value();
    m_isEnable = policy.m_isEnable;
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
