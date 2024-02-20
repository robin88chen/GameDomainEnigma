#include "Light.h"
#include "LightInfo.h"
#include "LightInfoDtos.h"
#include "SceneGraphErrors.h"
#include "SceneGraphEvents.h"
#include "SceneGraphDtos.h"
#include "Frameworks/EventPublisher.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;

DEFINE_RTTI(SceneGraph, Light, Spatial);

Light::Light(const std::string& spatialName, const LightInfo& lightInfo) : Spatial(spatialName), m_lightInfo(lightInfo)
{
    m_factoryDesc = Engine::FactoryDesc(Light::TYPE_RTTI.getName());
}

Light::Light(const Engine::GenericDto& o) : Spatial(o)
{
    LightDto dto{ o };
    m_lightInfo = LightInfo(dto.lightInfo());
}

Light::~Light()
{
    Frameworks::EventPublisher::post(std::make_shared<LightInfoDeleted>(m_name, m_lightInfo.lightType()));
}

Enigma::Engine::GenericDto Light::serializeDto()
{
    LightDto dto(serializeSpatialDto());
    dto.lightInfo() = m_lightInfo.serializeDto().toGenericDto();
    return dto.toGenericDto();
}

error Light::onCullingVisible(Culler*, bool)
{
    return ErrorCode::ok;
}

error Light::_updateWorldData(const MathLib::Matrix4& mxParentWorld)
{
    error er = Spatial::_updateWorldData(mxParentWorld);
    if (er) return er;
    m_lightInfo.setLightPosition(m_vecWorldPosition);

    _propagateSpatialRenderState();

    return er;
}
void Light::setLightColor(const MathLib::ColorRGBA& color)
{
    info().setLightColor(color);
    Frameworks::EventPublisher::post(std::make_shared<LightInfoUpdated>(thisLight(), LightInfoUpdated::NotifyCode::Color));
}

void Light::setLightPosition(const MathLib::Vector3& vec)
{
    info().setLightPosition(vec);
    Frameworks::EventPublisher::post(std::make_shared<LightInfoUpdated>(thisLight(), LightInfoUpdated::NotifyCode::Position));
}

void Light::setLightDirection(const MathLib::Vector3& vec)
{
    info().setLightDirection(vec);
    Frameworks::EventPublisher::post(std::make_shared<LightInfoUpdated>(thisLight(), LightInfoUpdated::NotifyCode::Direction));
}

void Light::setLightAttenuation(const MathLib::Vector3& attenuation)
{
    info().setLightAttenuation(attenuation);
    Frameworks::EventPublisher::post(std::make_shared<LightInfoUpdated>(thisLight(), LightInfoUpdated::NotifyCode::Attenuation));
}

void Light::setLightRange(float range)
{
    info().setLightRange(range);
    Frameworks::EventPublisher::post(std::make_shared<LightInfoUpdated>(thisLight(), LightInfoUpdated::NotifyCode::Range));
}

void Light::setEnable(bool flag)
{
    info().setEnable(flag);
    Frameworks::EventPublisher::post(std::make_shared<LightInfoUpdated>(thisLight(), LightInfoUpdated::NotifyCode::Enable));
}
