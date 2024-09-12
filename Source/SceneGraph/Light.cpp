#include "Light.h"
#include "LightInfo.h"
#include "LightAssemblers.h"
#include "SceneGraphErrors.h"
#include "LightEvents.h"
#include "Frameworks/EventPublisher.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;

DEFINE_RTTI(SceneGraph, Light, Spatial);

Light::Light(const SpatialId& id, const LightInfo& lightInfo) : Spatial(id), m_lightInfo(lightInfo)
{
    m_factoryDesc = Engine::FactoryDesc(Light::TYPE_RTTI.getName());
}

Light::~Light()
{
    Frameworks::EventPublisher::enqueue(std::make_shared<LightInfoDeleted>(m_id, m_lightInfo.lightType()));
}

std::shared_ptr<Light> Light::create(const SpatialId& id, const LightInfo& lightInfo)
{
    return std::make_shared<Light>(id, lightInfo);
}

std::shared_ptr<SpatialAssembler> Light::assembler() const
{
    return std::make_shared<LightAssembler>(m_id, m_lightInfo.lightType());
}

void Light::assemble(const std::shared_ptr<SpatialAssembler>& assembler)
{
    Spatial::assemble(assembler);
    if (auto lightAssembler = std::dynamic_pointer_cast<LightAssembler>(assembler))
    {
        lightAssembler->lightInfo(m_lightInfo);
    }
}

std::shared_ptr<SpatialDisassembler> Light::disassembler() const
{
    return std::make_shared<LightDisassembler>();
}

void Light::disassemble(const std::shared_ptr<SpatialDisassembler>& disassembler)
{
    Spatial::disassemble(disassembler);
    if (auto lightDisassembler = std::dynamic_pointer_cast<LightDisassembler>(disassembler))
    {
        m_lightInfo = lightDisassembler->lightInfo();
    }
}

error Light::onCullingVisible(Culler*, bool)
{
    return ErrorCode::ok;
}

error Light::_updateWorldData(const MathLib::Matrix4& mxParentWorld)
{
    error er = Spatial::_updateWorldData(mxParentWorld);
    if (er) return er;
    m_lightInfo.position(m_vecWorldPosition);

    _propagateSpatialRenderState();

    return er;
}
void Light::setLightColor(const MathLib::ColorRGBA& color)
{
    info().color(color);
    Frameworks::EventPublisher::enqueue(std::make_shared<LightInfoUpdated>(thisLight(), LightInfoUpdated::NotifyCode::Color));
}

void Light::setLightPosition(const MathLib::Vector3& vec)
{
    info().position(vec);
    Frameworks::EventPublisher::enqueue(std::make_shared<LightInfoUpdated>(thisLight(), LightInfoUpdated::NotifyCode::Position));
}

void Light::setLightDirection(const MathLib::Vector3& vec)
{
    info().direction(vec);
    Frameworks::EventPublisher::enqueue(std::make_shared<LightInfoUpdated>(thisLight(), LightInfoUpdated::NotifyCode::Direction));
}

void Light::setLightAttenuation(const MathLib::Vector3& attenuation)
{
    info().attenuation(attenuation);
    Frameworks::EventPublisher::enqueue(std::make_shared<LightInfoUpdated>(thisLight(), LightInfoUpdated::NotifyCode::Attenuation));
}

void Light::setLightRange(float range)
{
    info().range(range);
    Frameworks::EventPublisher::enqueue(std::make_shared<LightInfoUpdated>(thisLight(), LightInfoUpdated::NotifyCode::Range));
}

void Light::setEnable(bool flag)
{
    info().setEnable(flag);
    Frameworks::EventPublisher::enqueue(std::make_shared<LightInfoUpdated>(thisLight(), LightInfoUpdated::NotifyCode::Enable));
}
