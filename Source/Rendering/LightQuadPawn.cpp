﻿#include "LightQuadPawn.h"
#include "LightingMeshQueries.h"
#include "SceneGraph/Light.h"
#include "SceneGraph/LightInfo.h"
#include "LightingPawnDto.h"
#include "SceneGraph/LightEvents.h"
#include "RenderingErrors.h"

using namespace Enigma::GameCommon;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::SceneGraph;
using namespace Enigma::Rendering;

DEFINE_RTTI(GameCommon, LightQuadPawn, LightingPawn);

LightQuadPawn::LightQuadPawn(const SpatialId& id) : LightingPawn(id)
{
}

LightQuadPawn::LightQuadPawn(const SpatialId& id, const Engine::GenericDto& o) : LightingPawn(id, o)
{
    if ((!m_primitive) && (!m_hostLight.expired()))
    {
        LightingPawnDto dto(o);
        if ((m_hostLight.lock()->info().lightType() == LightInfo::LightType::Ambient) && dto.primitiveId().has_value())
        {
            m_primitive = std::make_shared<RequestAmbientLightMeshAssembly>(dto.primitiveId().value())->dispatch();
        }
        else if ((m_hostLight.lock()->info().lightType() == LightInfo::LightType::SunLight) && dto.primitiveId().has_value())
        {
            m_primitive = std::make_shared<RequestSunLightMeshAssembly>(dto.primitiveId().value())->dispatch();
        }
    }
}

LightQuadPawn::~LightQuadPawn()
{
}

std::shared_ptr<LightQuadPawn> LightQuadPawn::create(const SceneGraph::SpatialId& id)
{
    return std::make_shared<LightQuadPawn>(id);
}

std::shared_ptr<LightQuadPawn> LightQuadPawn::constitute(const SceneGraph::SpatialId& id, const Engine::GenericDto& dto)
{
    return std::make_shared<LightQuadPawn>(id, dto);
}

GenericDto LightQuadPawn::serializeDto()
{
    LightingPawnDto dto(SerializePawnDto());
    if (getHostLight()) dto.hostLightId(getHostLight()->id());
    return dto.toGenericDto();
}

error LightQuadPawn::_updateSpatialRenderState()
{
    if (!isRenderable()) return ErrorCode::ok;  // only renderable entity need
    if (m_hostLight.expired()) return ErrorCode::nullHostLight;

    ColorRGBA colorLight(m_hostLight.lock()->info().getLightColor());
    RenderLightingState lighting_state;
    if (m_hostLight.lock()->info().lightType() == LightInfo::LightType::Ambient)
    {
        lighting_state.setAmbientLightColor(colorLight);
    }
    else if (m_hostLight.lock()->info().lightType() == LightInfo::LightType::SunLight)
    {
        Vector3 lightDir = m_hostLight.lock()->info().getLightDirection();
        lighting_state.setSunLight(lightDir, colorLight);
    }
    m_spatialRenderState = SpatialRenderState(lighting_state);

    return ErrorCode::ok;
}


void LightQuadPawn::onLightInfoUpdated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<LightInfoUpdated>(e);
    if (!ev) return;
    if (getHostLight() == nullptr) return;
    if (ev->light() != getHostLight()) return;
    if ((ev->notifyCode() == LightInfoUpdated::NotifyCode::Color) || (ev->notifyCode() == LightInfoUpdated::NotifyCode::Direction))
    {
        notifySpatialRenderStateChanged();
    }
}
