#include "Light.h"
#include "LightInfo.h"
#include "LightInfoContracts.h"
#include "SceneGraphErrors.h"
#include "SceneGraphEvents.h"
#include "SceneGraphContracts.h"
#include "Frameworks/EventPublisher.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;

DEFINE_RTTI(SceneGraph, Light, Spatial);

Light::Light(const std::string& spatialName, const LightInfo& lightInfo) : Spatial(spatialName), m_lightInfo(lightInfo)
{
}

Light::Light(const LightContract& contract) : Spatial(dynamic_cast<const SpatialContract&>(contract))
{
    m_lightInfo = LightInfo(LightInfoContract::FromContract(contract.LightInfo()));
}

Light::~Light()
{
    Frameworks::EventPublisher::Post(std::make_shared<LightInfoDeleted>(m_name, m_lightInfo.GetLightType()));
}

LightContract Light::SerializeContract()
{
    LightContract contract(Spatial::SerializeContract());
    contract.LightInfo() = m_lightInfo.SerializeContract().ToContract();
    return contract;
}

error Light::OnCullingVisible(Culler*, bool)
{
    return ErrorCode::ok;
}

error Light::_UpdateWorldData(const MathLib::Matrix4& mxParentWorld)
{
    error er = Spatial::_UpdateWorldData(mxParentWorld);
    if (er) return er;
    m_lightInfo.SetLightPosition(m_vecWorldPosition);

    _PropagateSpatialRenderState();

    return er;
}
void Light::SetLightColor(const MathLib::ColorRGBA& color)
{
    Info().SetLightColor(color);
    Frameworks::EventPublisher::Post(std::make_shared<LightInfoUpdated>(ThisLight(), LightInfoUpdated::NotifyCode::Color));
}

void Light::SetLightPosition(const MathLib::Vector3& vec)
{
    Info().SetLightPosition(vec);
    Frameworks::EventPublisher::Post(std::make_shared<LightInfoUpdated>(ThisLight(), LightInfoUpdated::NotifyCode::Position));
}

void Light::SetLightDirection(const MathLib::Vector3& vec)
{
    Info().SetLightDirection(vec);
    Frameworks::EventPublisher::Post(std::make_shared<LightInfoUpdated>(ThisLight(), LightInfoUpdated::NotifyCode::Direction));
}

void Light::SetLightAttenuation(const MathLib::Vector3& attenuation)
{
    Info().SetLightAttenuation(attenuation);
    Frameworks::EventPublisher::Post(std::make_shared<LightInfoUpdated>(ThisLight(), LightInfoUpdated::NotifyCode::Attenuation));
}

void Light::SetLightRange(float range)
{
    Info().SetLightRange(range);
    Frameworks::EventPublisher::Post(std::make_shared<LightInfoUpdated>(ThisLight(), LightInfoUpdated::NotifyCode::Range));
}

void Light::SetEnable(bool flag)
{
    Info().SetEnable(flag);
    Frameworks::EventPublisher::Post(std::make_shared<LightInfoUpdated>(ThisLight(), LightInfoUpdated::NotifyCode::Enable));
}
