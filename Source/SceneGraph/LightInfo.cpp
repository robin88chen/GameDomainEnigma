#include "LightInfo.h"
#include "SceneGraphEvents.h"
#include "Frameworks/EventPublisher.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;

LightInfo::LightInfo(const std::string& name, LightType type)
{
    m_name = name;
    m_type = type;
    m_color = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
    m_dir = Vector3::UNIT_Z;
    m_position = Vector3::ZERO;
    m_range = 10.0f;
    m_attenuation = Vector3(0.0f, 1.0f, 0.0f);
    m_isEnable = true;
}

LightInfo::~LightInfo()
{
}

void LightInfo::SetLightColor(const MathLib::ColorRGBA& color)
{
    m_color = color;
    Frameworks::EventPublisher::Post(std::make_shared<LightInfoUpdated>(shared_from_this(), LightInfoUpdated::NotifyCode::Color));
}

void LightInfo::SetLightPosition(const MathLib::Vector3& vec)
{
    m_position = vec;
    Frameworks::EventPublisher::Post(std::make_shared<LightInfoUpdated>(shared_from_this(), LightInfoUpdated::NotifyCode::Position));
}

void LightInfo::SetLightDirection(const MathLib::Vector3& vec)
{
    m_dir = vec;
    Frameworks::EventPublisher::Post(std::make_shared<LightInfoUpdated>(shared_from_this(), LightInfoUpdated::NotifyCode::Direction));
}

void LightInfo::SetLightAttenuation(const MathLib::Vector3& attenuation)
{
    m_attenuation = attenuation;
    Frameworks::EventPublisher::Post(std::make_shared<LightInfoUpdated>(shared_from_this(), LightInfoUpdated::NotifyCode::Attenuation));
}

void LightInfo::SetLightRange(float range)
{
    m_range = range;
    Frameworks::EventPublisher::Post(std::make_shared<LightInfoUpdated>(shared_from_this(), LightInfoUpdated::NotifyCode::Range));
}

void LightInfo::SetEnable(bool flag)
{
    m_isEnable = flag;
    Frameworks::EventPublisher::Post(std::make_shared<LightInfoUpdated>(shared_from_this(), LightInfoUpdated::NotifyCode::Enable));
}
