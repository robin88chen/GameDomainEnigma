#include "LightQuadPawn.h"
#include "GameCommonErrors.h"
#include "SceneGraph/Light.h"
#include "SceneGraph/LightInfo.h"
#include "LightingPawnDto.h"

using namespace Enigma::GameCommon;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::SceneGraph;

DEFINE_RTTI(GameCommon, LightQuadPawn, LightingPawn);

LightQuadPawn::LightQuadPawn(const SpatialId& id) : LightingPawn(id)
{
}

LightQuadPawn::LightQuadPawn(const SpatialId& id, const Engine::GenericDto& o) : LightingPawn(id, o)
{
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
    dto.HostLightName() = GetHostLightName();
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
        lighting_state.SetAmbientLightColor(colorLight);
    }
    else if (m_hostLight.lock()->info().lightType() == LightInfo::LightType::SunLight)
    {
        Vector3 lightDir = m_hostLight.lock()->info().getLightDirection();
        lighting_state.SetSunLight(lightDir, colorLight);
    }
    m_spatialRenderState = SpatialRenderState(lighting_state);

    return ErrorCode::ok;
}

